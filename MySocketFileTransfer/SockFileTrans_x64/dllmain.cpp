// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "sft_runtime.h"
#include "../../resource/tool.h"

using std::string;
using std::to_string;
using SockFileTrans::ip;
using SockFileTrans::ush;
using SockFileTrans::remote_info;
using SockFileTrans::t::callback;

size_t BUF_SIZE_MAX = SockFileTrans::buffer_size_max;

#define MAIN_VERSION 2
#define  SEC_VERSION 3
#define  SUB_VERSION 3

long long version();

//WSADATA wsaData;
//int wsa;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		//WSAStartup(MAKEWORD(2, 2), &wsaData);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

string lasterr_buffer;
string LastErr() {
	return lasterr_buffer;
}
void SetLastErr(string err) {
	lasterr_buffer = err;
}


long long version() {
	return SOCK_FTRANS_VERSION(MAIN_VERSION, SEC_VERSION, SUB_VERSION);
}

void about(char* buffer, size_t bufLength) {
	//char ver_buffer[15] = { 0 };
	//ltoa((long)version(), ver_buffer, 16);
	string str;
	str += "Socket File Transfer Dynamic Link Library\nVersion " +
		to_string(version()) + "\n========\n";
	str += "Author: shc0743(https://github.com/shc0743/)\n"
		"========\n"
		"Compile_Time: " __TIMESTAMP__ "\n"
		"C++ version: " + to_string(_MSVC_LANG) + "\n"
		"File: " __FILE__ "\n"
		"Line: " + to_string(__LINE__) + "\n"
		"========\nCopyright 2021(c)shc0743.All rights reserved.\n"
		"Open source version: https://github.com/shc0743/SockFileTrans ";
	strcpy_s(buffer, bufLength, str.c_str());
}


void SetBufferSizeMax(size_t max_buffer_size) {
	::BUF_SIZE_MAX = max_buffer_size;
}


bool SendFile(ip ip, ush port, string file_name, callback cb) {
	//先检查文件是否存在
	HANDLE hFile = ::CreateFileW(s2wc(file_name), GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile) {
		SetLastErr("Cannot open file!\n");
		return false;
	}
	//FILE* fp = nullptr;
	//fopen_s(&fp, file_name.c_str(), "rb");  //以二进制方式打开文件
	//if (fp == NULL) {
	//	SetLastErr("Cannot open file!\n");
	//	return false;
	//}
	SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);
	if (!servSock) {
		::CloseHandle(hFile);
		return false;
	}

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	//sockAddr.sin_addr.s_addr = inet_addr(ip);
	//if (sockAddr.sin_addr.s_addr == INADDR_NONE) {
	//	::CloseHandle(hFile);
	//	closesocket(servSock);
	//	SetLastErr("无法转换ip");
	//	return false;
	//}
	int _inet_pton_result = inet_pton(AF_INET, (PCSTR)ip, &sockAddr.sin_addr.s_addr);
	if (_inet_pton_result != 1) {
		SetLastErr("inet_pton error: " + to_string(_inet_pton_result));
		return false;
	}
	sockAddr.sin_port = htons(port);
	bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	listen(servSock, 16);

	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);

	//fseek(fp, 0, SEEK_END); //定位到文件末 
	//size_t nTotal = ftell(fp); //文件长度
	//fseek(fp, 0, SEEK_SET); //恢复到文件头
	size_t nTotal = MyGetFileSizeA(file_name);

	SOCKET clntSock = accept(servSock, &clntAddr, &nSize);

	cb(0, nTotal);

	//循环发送数据，直到文件结尾
	size_t bufsize = BUF_SIZE_MAX;
	char* buffer = (char*)
		//calloc(BUF_SIZE, 1);
		GetBufMemory(bufsize);
	if (buffer == NULL) {
		::CloseHandle(hFile);
		closesocket(clntSock);
		closesocket(servSock);
		SetLastErr("无法分配内存");
		return false;
	}
	DWORD nCount = 0; size_t nTotalCount = 0;
	//while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
	while ((::ReadFile(hFile, buffer, DWORD(bufsize - 1), &nCount, NULL)) && nCount > 0) {
		send(clntSock, buffer, (int)nCount, 0);
		nTotalCount += (size_t)nCount;
		if (cb) cb(nTotalCount, nTotal);
	}

	shutdown(clntSock, SD_SEND);  //文件读取完毕，断开输出流，向客户端发送FIN包
	recv(clntSock, buffer, (int)bufsize, 0);  //阻塞，等待客户端接收完毕

	//fclose(fp);
	::CloseHandle(hFile);
	closesocket(clntSock);
	closesocket(servSock);
	free(buffer);

	return true;
}
DWORD SendFileAsync_(void* args) {
	ip ip; ush port; 
	string file_name;
	void(*callback)(bool);
	::callback callback_;

	remote_info* i = (remote_info*)args;
	ip = i->ip;
	port = i->port;
	file_name = i->fname;
	callback = i->callback;
	callback_ = i->callback_;

	callback(SendFile(ip, port, file_name,callback_));

	delete i;
	return 0;
}
HANDLE SendFileAsync(
	ip ip, 
	ush port, 
	string file_name,
	void(*callback)(bool), 
	::callback callb) {

	remote_info* i = new remote_info;
	i->ip = ip;
	i->port = port;
	i->fname = file_name;
	i->callback = callback;
	i->callback_ = callb;

	HANDLE h = CreateThread(NULL, 0, SendFileAsync_, i, CREATE_SUSPENDED, NULL);
	if (h) ResumeThread(h);
	return h;
}

bool RecvFile(ip ip, ush port, string file_name, callback cb) {
	HANDLE hFile = ::CreateFileW(s2wc(file_name), GENERIC_WRITE, 0,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile) {
		SetLastErr("Cannot open file!\n");
		return false;
	}
	//FILE* fp = fopen(file_name.c_str(), "wb");  //以二进制方式打开（创建）文件
	//if (fp == NULL) {
	//	SetLastErr("Cannot open file!\n");
	//	return false;
	//}
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!sock) { 
		::CloseHandle(hFile);
		return false;
	}

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	//sockAddr.sin_addr.s_addr = inet_addr(ip);
	//if (sockAddr.sin_addr.s_addr == INADDR_NONE) {
	//	::CloseHandle(hFile);
	//	closesocket(servSock);
	//	SetLastErr("无法转换ip");
	//	return false;
	//}
	int _inet_pton_result = inet_pton(AF_INET, (PCSTR)ip, &sockAddr.sin_addr.s_addr);
	if (_inet_pton_result != 1) {
		SetLastErr("inet_pton error: " + to_string(_inet_pton_result));
		return false;
	}
	sockAddr.sin_port = htons(port);
	connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	cb(0, 0);

	//循环接收数据，直到文件传输完毕
	//char buffer[BUF_SIZE] = { 0 };  //文件缓冲区
	size_t bufsize = BUF_SIZE_MAX;
	char* buffer = (char*)
		//calloc(BUF_SIZE, 1);
		GetBufMemory(bufsize);
	if (buffer == NULL) {
		::CloseHandle(hFile);
		closesocket(sock);
		SetLastErr("无法分配内存");
		return false;
	}
	int nCount = 0; size_t nRecvd = 0;
	while ((nCount = recv(sock, buffer, (int)bufsize, 0)) > 0) {
		//fwrite(buffer, nCount, 1, fp);
		::WriteFile(hFile, buffer, nCount, NULL, NULL);
		nRecvd += (size_t)nCount;
		if (cb) cb(nRecvd, 0);
	}

	//文件接收完毕后直接关闭套接字，无需调用shutdown()
	//fclose(fp);
	::CloseHandle(hFile);
	closesocket(sock);
	free(buffer);
	if (cb) cb(nRecvd, nRecvd);

	return true;
}
DWORD RecvFileAsync_(void* args) {
	ip ip; ush port;
	string file_name;
	void(*callback)(bool);
	::callback callback_;

	remote_info* i = (remote_info*)args;
	ip = i->ip;
	port = i->port;
	file_name = i->fname;
	callback = i->callback;
	callback_ = i->callback_;

	callback(RecvFile(ip, port, file_name, callback_));

	delete i;
	return 0;
}
HANDLE RecvFileAsync(
	ip ip,
	ush port,
	string file_name,
	void(*callback)(bool),
	::callback callb) {

	remote_info* i = new remote_info;
	i->ip = ip;
	i->port = port;
	i->fname = file_name;
	i->callback = callback;
	i->callback_ = callb;

	HANDLE h = CreateThread(NULL, 0, RecvFileAsync_, i, CREATE_SUSPENDED, NULL);
	if (h) ResumeThread(h);
	return h;
}

