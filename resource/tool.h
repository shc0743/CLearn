/*     
十年生死两茫茫，写程序，到天亮，
千行代码，Bug何处藏。
纵使上线又怎样，朝令改，夕断肠。
领导每天新想法，天天改，日日忙。
相顾无言，惟有泪千行。
每晚灯火阑珊处，程序员，加班狂。
*/
#pragma once
#ifndef __cplusplus
#error "tool.h is a C++ header file"
#endif
#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<numbers>
#include<string>
#include<vector>
#include<random>
#include<deque>
#include<queue>
#include<map>
#include<stdlib.h>
#include<stdio.h>
#include<conio.h>
#include<errno.h>
#include<algorithm>
#include<signal.h>
#include<limits.h>
#include<locale.h>
#ifdef _WIN32
#ifndef _WINDOWS_
#include<Windows.h>
#include<WindowsX.h>
#endif
#include<TlHelp32.h>
#include<ShObjIdl.h>
#endif
#include<tchar.h>
#include<ctime>
#include<typeinfo>
#include<assert.h>
#include<source_location>
#include<thread>
#if 0
#include<format>
#endif
#include<version>
#include<chrono>
#include<mutex>
#include<span>
#include<any>
#ifdef _WIN32
#include<Rpc.h>
#pragma comment(lib,"Rpcrt4.lib")
#else
//编译链接时，需要加入 -luuid
#include <uuid/uuid.h>
#endif

#ifdef _MSVC_LANG
#if !(_MSVC_LANG > 201703L)
#error "The tool.h needs c++20 or later."
#endif
#elif defined __cplusplus
#if !(__cplusplus > 201703L)
#error "The tool.h needs c++20 or later."
#endif
#else
#error "No C++ supported"
#endif

//Main
#if (!(defined(_TOOL_SHC0743_MAIN)))
#define _TOOL_SHC0743_MAIN
using namespace std;

/*MFC Error Method*
* Debug
uafxcwd.lib;LIBCMTD.lib;
LIBCMTD.lib;uafxcwd.lib;
* Release
uafxcw.lib;LIBCMT.lib;
LIBCMT.lib;uafxcw.lib;
*End*/

//Define Reds
#if defined(UNICODE)
using STRING = wstring;
#define _C_AUTOTEXT(text) L ## text
#define STRCMP lstrcmpW
#define strcpyt lstrcpyW
#define strlen_t lstrlenW
#define tostr ws2s
#else
using STRING = string;
#define _C_AUTOTEXT(text) text
#define STRCMP strcmp
#define strcpyt strcpy
#define strlen_t strlen
#define tostr 
#endif

#if defined(UNICODE)
#define CF_AUTOTEXT CF_UNICODETEXT
#define _AUTOTEXT(text) L ## text
using C_TSTR = std::wstring;
using FSTREAM_T = std::wfstream;
using BASSTRING = wchar_t;
#define atoi_t(x) atoi(ws2s(x).c_str())
#else
#define CF_AUTOTEXT CF_TEXT
#define _AUTOTEXT(text) text
using C_TSTR = std::string;
using FSTREAM_T = std::fstream;
using BASSTRING = char;
#define atoi_t(x) atoi(x)
#endif

constexpr auto E_FATAL = 1;
constexpr auto E_ERR   = 2;
constexpr auto E_WARN  = 3;

#ifndef debugger
#ifdef _DEBUG
#define debugger fflush(stdin);\
				 puts("Debugger Paused.\nPress any key to continue...");\
				 ((void)(_getch()))
#else
#define debugger (void(0))
#endif
#endif

#ifndef fcreate
#define fcreate(fileName) fclose(fopen(fileName,"wb+"))
#endif

#ifndef codecat
#define codecat(a,b) a ## b
#endif

#ifndef codetostr
#define codetostr(c) #c
#endif

#ifndef force_cast
#define force_cast(type,var) (*reinterpret_cast<type*>(&(var)))
#endif

#ifndef echo
#define echo std::cout << 
#endif

#ifndef pause
#define pause puts("Press any key to continue...");(void)_getch();
#endif

#if 0
#ifndef AND
#define AND &
#endif
#ifndef OR
#define OR |
#endif
#ifndef NOT
#define NOT ~
#endif
#ifndef XOR
#define XOR ^
#endif
#ifndef LAND
#define LAND &&
#endif
#ifndef LOR
#define LOR ||
#endif
#ifndef LNOT
#define LNOT !
#endif
#endif

//参数1：待修改的字符串；参数2：要替换的内容；参数3：替换后的内容
//替换完后会返回给原字符串变量也就是strBase。
string& str_replace(std::string& strBase, 
	const std::string strSrc, const std::string strDes);
//参数1：待修改的字符串；参数2：要替换的内容；参数3：替换后的内容
//替换完后会返回给原字符串变量也就是strBase。
wstring& str_replace(std::wstring& strBase,
	const std::wstring strSrc, const std::wstring strDes);

//参数1：要分割的字符串；参数2：作为分隔符的字符
std::vector<std::string>& str_split(
	const std::string& src, 
	const std::string separator,
	std::vector<std::string>& dest);
//参数1：要分割的字符串；参数2：作为分隔符的字符
std::vector<std::wstring>& str_split(
	const std::wstring& src, 
	const std::wstring separator, 
	std::vector<std::wstring>& dest);

wstring s2ws(const string);
string ws2s(const wstring);
#define s2wc(x) (s2ws(x).c_str())
#define ws2c(x) (ws2s(x).c_str())

#ifdef UNICODE
inline wstring to__str(const string& str) { return s2ws(str); }
inline wstring to__str(const wstring& str) { return str; }
#else
inline string to__str(const string& str) { return str; }
inline string to__str(const wstring& str) { return ws2s(str); }
#endif

constexpr auto null = nullptr;

inline bool file_exists (const std::string& name) {
	struct stat buffer;   
	return (stat(name.c_str(), &buffer) == 0); 
}
inline bool file_exists (const std::wstring& name) {
	struct _stat64i32 stat;
	return (_wstat(name.c_str(), &stat) == 0); 
}

inline bool SetConsoleColor(WORD wAttr) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE) return FALSE;
	return SetConsoleTextAttribute(hConsole, wAttr);
}
/**
	第一个对应于背景，第二个对应于前景。
	0 = 黑色       8 = 灰色
	1 = 蓝色       9 = 淡蓝色
	2 = 绿色       A = 淡绿色
	3 = 浅绿色     B = 淡浅绿色
	4 = 红色       C = 淡红色
	5 = 紫色       D = 淡紫色
	6 = 黄色       E = 淡黄色
	7 = 白色       F = 亮白色
*/
#define SetConsColor(ColorVal) SetConsColor_(ColorVal)
#define SetConsColor_(ColorVal) SetConsoleColor(0x ## ColorVal)

class colorset {
public:
	colorset() = delete;
	colorset(const colorset&) = delete;
	colorset(colorset&&) = delete;
	~colorset() = default;
public:
	static void r() { SetConsColor(04); }
	static void g() { SetConsColor(02); }
	static void b() { SetConsColor(01); }
	static void rst() { SetConsColor(07); }
};

bool IsRunAsAdmin();
[[nodiscard("Ignoring the return value will useless the function")]] string GetProgramDir();
string GetProgramPath();
using ProgramInfo = struct{
	string dir;
	string path;
	string name;
};
ProgramInfo GetProgramInfo();

#ifdef _WIN32
/**
函数功能：释放资源文件

		  DWORD dwResName   指定要释放的资源ID号，如IDR_EXE
		  LPCSTR lpResType 指定释放的资源的资源类型
		  LPCSTR lpFilePathName 指定释放后的目标文件名

返回值：成功则返回TRUE,失败返回FALSE  
*/
bool FreeResFile(DWORD dwResName,string lpResType,string lpFilePathName);
#endif

void __stdcall ReStart(int _Code = 0);

//检测自身是否存在
bool CheckSelfProcessExists();
//检测指定进程存在数量
size_t CheckProcessCount(STRING pname);

//ChineseProgramming
#ifdef _CHINESEPROG
#ifndef _NOCHINESEPROG
#define _NOCHINESEPROG
using 整数 = int;
using 小数 = float;
using 浮点数 = double;
typedef char 字符;
typedef char* 字符串指针;
using 字符数组=char[];
using 字符串=std::string;
template <class T,int size>
using 数组=std::array<T,size>;
template <class T>
using 向量数组=std::vector<T>;
template <class T>
using 队列=std::deque<T>;
#endif
#endif
//EndCP

//Clear Display
#ifdef _WIN32
//Clear Windows Display
#define cls() (system("cls"))
#else defined __linux__
//Clear Linux Display
#define cls() (system("clear"))
#endif

//Sleep
#ifdef __linux__
#define Sleep(int_time_millsecond) (sleep((int_time_millsecond)/1000))
#elif _WIN32
#define sleep(int_time_second) (Sleep((DWORD)((int_time_second)*1000)))
#endif

using str_vector = vector<string>;

template <class T>
using var = T;
template <class T>
using CreateVar = T;

#define StartCreateObjArea() ;
#define CreateObjectNew(TYPENAME,VARNAME) ;TYPENAME* VARNAME = new TYPENAME;
#define EndObjectNew(__VAR__NAME__) ;delete __VAR__NAME__;
#define EndCreateObjArea() ;

#define CreateMFCDialogNoModal(MFCClass,DlgVar,CreateFrom) ;\
CreateObjectNew(MFCClass,DlgVar)\
DlgVar->Create(CreateFrom);\
DlgVar->ShowWindow(SW_NORMAL);
#define CreateMFCDialogModal(MFCClass,DlgVar) ;\
MFCClass DlgVar;\
DlgVar.DoModal();



template<typename str>
class CmdLine_t {
public:
	CmdLine_t() {
		this->data = new vector<str>;
	};
	CmdLine_t(str src) {
		this->data = new vector<str>;
		this->split(src);
	};
	CmdLine_t(const CmdLine_t<str>& cp) {
		this->data = new vector<str>(cp);
	};
	CmdLine_t(CmdLine_t<str>&& mv) {
		this->data = mv->data;
		mv->data = nullptr;
	};
	~CmdLine_t(){
		delete this->data;
	};
	using value_type = str;
public:
	bool split(str t) {
		CmdLine_t<str>& willbe = *this;
		const unsigned int len = (unsigned int)(t.length()); bool inquote = false;
		if (typeid(str) == typeid(wstring)) {
			wstring t2 = *reinterpret_cast<wstring*>(&t);
			wstring dat = L"";
			for (unsigned int i = 0; i < len; ++i) {
				if (t2[i] == L'"') { inquote = !inquote; continue; }
				if (!inquote && t2[i] == L' ' && dat.length() > 0) {
					willbe.data->push_back(*reinterpret_cast<str*>(&dat));
					dat = L""; continue;
				}
				if (!inquote && t2[i] == L' ') { dat = L""; continue; }
				dat += t2[i];
			}
			wstring dat2 = (dat);
			if (str_replace(dat2, L" ", L"").length() > 0)
				willbe.data->push_back(*reinterpret_cast<str*>(&dat));
		}
		else if (typeid(str) == typeid(string)) {
			string t2 = *reinterpret_cast<string*>(&t);
			string dat = "";
			for (unsigned int i = 0; i < len; ++i) {
				if (t2[i] == '"') { inquote = !inquote; continue; }
				if (!inquote && t2[i] == ' ' && dat.length() > 0) {
					willbe.data->push_back(*reinterpret_cast<str*>(&dat));
					dat = ""; continue;
				}
				if (!inquote && t2[i] == ' ') { dat = ""; continue; }
				dat += t2[i];
			}
			string dat2 = (dat);
			if (str_replace(dat2, " ", "").length() > 0)
				willbe.data->push_back(*reinterpret_cast<str*>(&dat));
		}
		else return false;
		return true;
	};
	/**
	@brief getopt
	@param opt Command line parameter name
	@param resBuffer Resolved params. If it cannot be resolved or no any param, it is not changed.
	@return 0-Failed. 1-Parsing succeeded with parameters. 2-Parsing succeeded without parameters.
	*/
	int getopt(str opt, str& resBuffer) {
#pragma warning(push)
#pragma warning(disable: 6276)
		str s, s2, stmp;
		if (typeid(str) == typeid(string))  s = reinterpret_cast<const str::value_type*>("-");
		if (typeid(str) == typeid(wstring)) s = reinterpret_cast<const str::value_type*>(L"-");
		if (typeid(str) == typeid(string)) s2 = reinterpret_cast<const str::value_type*>("=");
		if (typeid(str) == typeid(wstring)) s2 = reinterpret_cast<const str::value_type*>(L"=");
		size_t datasz = data->size();
		if (opt.length() == 1) {
			for (size_t i = 0; i < datasz; ++i) {
				if (data->at(i).substr(0, 2) == s + opt) {
					stmp = data->at(i);
					resBuffer = stmp.erase(0, 2);
					return 1;
				}
			}
			return 0;
		}
		else {
			for (size_t i = 0; i < datasz; ++i) {
				if (data->at(i).substr(0, opt.length() + 3) == s + s + opt + s2) {
					stmp = data->at(i);
					resBuffer = stmp.erase(0, opt.length() + 3);
					return 1;
				}
				stmp = s + s; stmp += opt;
				if (data->at(i) == stmp) return 2;
			}
			return 0;
		}
		return 0;
	};
	/**
	@brief The version of 'getopt' without the resbuffer parameter.
	@param opt Command line parameter name
	@see int getopt(str opt, str& resBuffer)
	*/
	int getopt(str opt) {
		str tmp; return getopt(opt, tmp);
	}
	///**
	//@brief The version of 'getopt' with str::value_type parameter.
	//@param opt Command line parameter name
	//@see int getopt(str opt, str& resBuffer)
	//*/
	//int getopt(str::value_type opt, str& resBuffer) {
	//	str s, s2, stmp;
	//	if (typeid(str) == typeid(string))  s = reinterpret_cast<const str::value_type*>("-");
	//	if (typeid(str) == typeid(wstring)) s = reinterpret_cast<const str::value_type*>(L"-");
	//	if (typeid(str) == typeid(string)) s2 = reinterpret_cast<const str::value_type*>("=");
	//	if (typeid(str) == typeid(wstring)) s2 = reinterpret_cast<const str::value_type*>(L"=");
	//	size_t datasz = data->size();
	//	for (size_t i = 0; i < datasz; ++i) {
	//		if (data->at(i).substr(0, 2) == s + opt) {
	//			stmp = data->at(i);
	//			resBuffer = stmp.erase(0, 2);
	//			return 1;
	//		}
	//	}
	//	return 0;
	//	return 0;
	//}
	///**
	//@brief The version of 'getopt' without the resbuffer parameter.
	//@param opt Command line parameter name
	//@see int getopt(str::value opt, str& resBuffer)
	//*/
	//int getopt(str::value_type opt) {
	//	str tmp; return getopt(opt, tmp);
	//}
#pragma warning(pop)
	size_t argc(){ return this->data->size(); };
	size_t size(){ return this->data->size(); };
	size_t length(){ return this->data->size(); };
	operator size_t() { return this->data->size(); };
public:
	str& operator[](unsigned long long zIndex) { 
		return this->data->operator[](zIndex); 
	};
	const str at(unsigned long long zIndex) const { 
		return this->data->at(zIndex); 
	};
	str& at(unsigned long long zIndex, str newData) { 
		return this->data->at(zIndex) = newData; 
	};
public:
	using v_it = typename vector<str>::iterator;
	using r_it = typename reverse_iterator<v_it>;
	using c_it = typename vector<str>::const_iterator;
	v_it begin() { return data->begin(); };
	v_it end() { return data->end(); };
	r_it rbegin() { return data->rbegin(); };
	r_it rend() { return data->rend(); };
	c_it cbegin() const { return data->cbegin(); };
	c_it cend() const { return data->cend(); };

	vector<str>& __get_data__() {
		return *data;
	};
protected:
	vector<str>* data;
};
using CmdLineW = CmdLine_t<wstring>;
using CmdLineA = CmdLine_t< string>;
#ifdef UNICODE
using CmdLine = CmdLineW;
#else
using CmdLine = CmdLineA;
#endif

//通过进程ID获取进程句柄
//PROCESS_ALL_ACCESS 获取所有权限
//PROCESS_QUERY_INFORMATION 获取进程的令牌、退出码和优先级等信息
HANDLE GetProcessHandle(int nID);

//通过进程名（带后缀.exe）获取进程句柄
HANDLE GetProcessHandle(LPCTSTR lpName);

//通过进程名（带后缀.exe）获取进程id
DWORD GetProcessIdW(wstring lpName);
inline DWORD GetProcessIdA(string lpName) {
	return GetProcessIdW(s2ws(lpName));
};
#ifdef UNICODE
#define GetProcessId GetProcessIdW
#else
#define GetProcessId GetProcessIdA
#endif
//通过进程名（带后缀.exe）获取进程id
vector<DWORD>* GetProcessIdFromAll(LPCTSTR lpName);

//***************************************************
// 函数名称: EnableDebugPrivilege
// 函数说明: 管理员权限提权
// 方    式: public
// 输入参数: void
// 输出参数: bool
// 返 回 值: true-提权成功，false-提权失败
// 作    者: 
// 创建时间: 2018-11-5
// 备    注: 
//***************************************************
bool EnableDebugPrivilege();

void StartExecute(const STRING& paras);

#if 0
class StringEx : public string {
public:
	StringEx() {};
	StringEx(std::string str) { *this = str; }
	StringEx(std::wstring str) { *this = /*(StringEx)*/ws2s(str); }
	operator char* () { return (char*)(this->c_str()); }
	operator const char* () { return (this->c_str()); }
//  StringEx& operator=(string str) { *this = (StringEx)str; return *this; };
//  StringEx& operator=(string&);
	StringEx& operator=(StringEx&);
	StringEx& operator=(wstring&);
	StringEx& operator=(const char*);
	StringEx operator+(string&);
	StringEx operator+(StringEx&);
	StringEx operator+(wstring&);
	StringEx operator+=(string& argv) { return ((string)*this) += argv; };
	StringEx operator+=(StringEx& argv) { return ((string)*this) += (string)argv; };
	StringEx operator+=(wstring& argv) { return ((string)*this) += ws2s(argv); };
};
#endif

template<typename str,typename fs>
class _TempFile : public fs {
protected:
	str FilePathName;
	str WorkPath;
	bool NoRemove;
public:
	static wstring GetRdmFileNameW();
	static  string GetRdmFileNameA();
public:
	_TempFile();
	_TempFile(str fileName);
	~_TempFile();
public:
	void open(str fileName);
	void close() {
		return fs::close();
	};
public:
	str GetFilePathName();
};
using TempFileW = _TempFile<wstring, wfstream>;
using TempFileA = _TempFile< string,  fstream>;
#ifdef UNICODE
using TempFile = TempFileW;
#else
using TempFile = TempFileA;
#endif

int ExecBatch(STRING Batch);
int ExecBatch(vector<STRING>& Batch);

template<typename T> int AssertEx_(T d,
	int Line, 
	const char* File,
	bool AutoExec,
	bool bForce = false
) {
	bool b = (bool)d;
	if (b) return IDOK;
	SetConsColor(47);
	cerr << "AssertionError: File " << File << ",Line " << Line;
	SetConsColor(07);
	if (bForce) exit(IDABORT);
	stringstream ss; ss.str("");
	ss << "AssertionError!\n";
	ss << "The data is false." << endl;
	ss << endl << "At " << Line << ":" << File << ", AssertionError";
	auto r = ::MessageBoxA(NULL, ss.str().c_str(),
		("AssertionError"), MB_ICONERROR | MB_ABORTRETRYIGNORE | MB_DEFBUTTON1);
	if (AutoExec) {
		if (r == IDABORT) exit(IDABORT);
		return r;
	}
	else return r;
}
#define AssertEx(data) AssertEx_(data,__LINE__,__FILE__,false)
 
class ServiceMgr__{
public:
	unsigned   Start(string  ServiceName);
	unsigned ReStart(string  ServiceName);
	unsigned   Stop (string  ServiceName);
	unsigned   Query(LPCTSTR ServiceName);
public:
	static constexpr unsigned OK                        = 0;
	static constexpr unsigned ERROR_UNKNOWN             = 1;
	static constexpr unsigned ERROR_OPEN_SCMANAGER      = 2;
	static constexpr unsigned ERROR_OPEN_SERVICE        = 3;
	static constexpr unsigned ERROR_QUERY_SERVICESTATUS = 4;
	static constexpr unsigned ERROR_STOP_SERVICE        = 5;
	static constexpr unsigned ERROR_START_SERVICE       = 6;
	static constexpr unsigned ERROR_IS_START            = 6;
	static constexpr unsigned ERROR_IS_STOP             = 7;
public:
	static constexpr unsigned STATUS_START              = 16 + 1;
	static constexpr unsigned STATUS_PAUSE              = 16 + 2;
	static constexpr unsigned STATUS_STOP               = 16 + 3;
};
extern ServiceMgr__ ServiceMgr;

class Process_t;
class Process_t {
public:
	Process_t() {
		this->flush();
		ntdll = LoadLibrary(_T("ntdll.dll"));
		assert(ntdll); EnableDebugPrivilege();
	};
	~Process_t() {
		prs.~vector();
		FreeLibrary(ntdll);
	};
public:
	typedef DWORD(WINAPI* SUSPENDPROCESS)(HANDLE);
	typedef DWORD(WINAPI*  RESUMEPROCESS)(HANDLE);
	using NTSTATUS = LONG;
	using ProcessHandle = void*;
	using PROCESSINFOCLASS = PROCESS_INFORMATION_CLASS;
	typedef struct
	{
		DWORD ExitStatus; // 接收进程终止状态
		DWORD PebBaseAddress; // 接收进程环境块地址
		DWORD AffinityMask; // 接收进程关联掩码
		DWORD BasePriority; // 接收进程的优先级类
		ULONG UniqueProcessId; // 接收进程ID
		ULONG InheritedFromUniqueProcessId; //接收父进程ID
	} PROCESS_BASIC_INFORMATION;
	using _NtTerminateProcess = DWORD(CALLBACK*)(HANDLE, UINT);
	using __NtSetInformationProcess = NTSTATUS(NTAPI*)(
		HANDLE           ProcessHandle,
		PROCESSINFOCLASS ProcessInformationClass,
		PVOID            ProcessInformation,
		ULONG            ProcessInformationLength);
	using __NtQueryInformationProcess = NTSTATUS(NTAPI*)(
		HANDLE           ProcessHandle,
		PROCESSINFOCLASS ProcessInformationClass,
		PVOID            ProcessInformation,
		ULONG            ProcessInformationLength,
		PULONG           ReturnLength);
	static constexpr PROCESSINFOCLASS ProcessBasicInformation      = (PROCESSINFOCLASS)0;
	static constexpr PROCESSINFOCLASS ProcessDebugPort             = (PROCESSINFOCLASS)7;
	static constexpr PROCESSINFOCLASS ProcessWow64Information      = (PROCESSINFOCLASS)26;
	static constexpr PROCESSINFOCLASS ProcessImageFileName         = (PROCESSINFOCLASS)27;
	static constexpr PROCESSINFOCLASS ProcessBreakOnTermination    = (PROCESSINFOCLASS)29;
	static constexpr PROCESSINFOCLASS ProcessSubsystemInformation  = (PROCESSINFOCLASS)75;
public:
	class ProcessInfo {
	public:
		ProcessInfo(
			STRING name = _T(""),
			DWORD id = 0, 
			DWORD parent = 0, 
			HANDLE handle = nullptr) :
			m_name(name),
			m_id(id),
			m_par(parent),
			handle(handle) {};
		ProcessInfo(const ProcessInfo& cpy) :
			m_name(cpy.m_name),
			m_id(cpy.m_id),
			m_par(cpy.m_par),
			handle(cpy.handle) {};
		ProcessInfo(const HANDLE hdl):m_name(_T("")),m_id(0),m_par(0),handle(hdl) {};
		ProcessInfo& operator=(ProcessInfo& cpy) {
			this->m_name = cpy.m_name;
			this->m_id = cpy.m_id;
			this->m_par = cpy.m_par;
			this->handle = cpy.handle;
			return cpy;
		};
	private:
		STRING m_name;
		DWORD  m_id;
		DWORD  m_par;
		HANDLE handle;
	public:
		friend class Process_t;
	public:
		operator HANDLE()  const { return this->handle; };
		operator DWORD()   const { return this->m_id; };
		operator STRING()  const { return this->m_name; };
		bool operator!()   const { return !((bool)(this->handle)); }
	public:
		bool valid() const {
			return (this->handle) ? (this->handle != INVALID_HANDLE_VALUE) : false;
		};
		const STRING& name() const { return m_name; };
		DWORD id() const { return this->m_id; };
		DWORD parent() const { return this->m_par; };
		HANDLE Handle() const { return this->handle; };
	};
	class thread_t {
	public:
		friend class Process_t;
	private:
		thread_t() {};
	public:
		void exit(DWORD dwExitCode = 0) {
			return ExitThread(dwExitCode);
		};
		HANDLE open(DWORD thrd_id, DWORD access = THREAD_ALL_ACCESS) {
			return ::OpenThread(access, FALSE, thrd_id);
		};
		bool kill(HANDLE hThread, DWORD dwExitCode = 0) {
#pragma warning(disable: 6258)
			return TerminateThread(hThread, dwExitCode);
		};
		DWORD suspend(HANDLE hThread) {
			return SuspendThread(hThread);
		};
		DWORD  resume(HANDLE hThread) {
			return  ResumeThread(hThread);
		};
	} thread;
public:

	bool flush(void);

	void find(vector<Process_t::ProcessInfo>& dest);
	void find(STRING pname, vector<Process_t::ProcessInfo>& dest);
	Process_t::ProcessInfo find(DWORD pid);
	Process_t::ProcessInfo GetCurrentProcess(void);

	void findWindows(ProcessInfo process, vector<HWND>& dest);

	DWORD GetParentProcessId(DWORD dwProcessId);

	bool kill(const Process_t::ProcessInfo& exe, UINT exitCode = 0);
	inline bool kill(const DWORD pid, UINT exitCode = 0) {
		return kill(find(pid), exitCode);
	};
	bool killall(STRING name);

	void exit(int _Code = 0) {
		return ::exit(_Code);
	};

	[[nodiscard("Ignoring the return value will made C6335. "
		"If you want to only start a process, use \"StartOnly\".")]]
	PROCESS_INFORMATION Start (const STRING& paras, STARTUPINFO& si);
	[[nodiscard("Ignoring the return value will made C6335. "
		"If you want to only start a process, use \"StartOnly\".")]]
	inline PROCESS_INFORMATION Start(const STRING& paras) {
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		return Start(paras, si);
	};
	bool StartOnly (const STRING& paras, STARTUPINFO& si);
	inline bool StartOnly(const STRING& paras) {
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		return StartOnly(paras, si);
	};

	inline PROCESS_INFORMATION ReStart(const ProcessInfo& paras) {
		kill(paras);
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		return Start(paras, si);
	};

	BOOL CloseProcessHandle(PROCESS_INFORMATION h) {
		return ::CloseHandle(h.hProcess) && ::CloseHandle(h.hThread);
	}

	void CloseProcessHandles(vector<ProcessInfo>& processes) {
		for (auto i : processes) ::CloseHandle(i);
	}

	size_t count();
	size_t count(const ProcessInfo& paras);

	DWORD suspend(HANDLE hProcess);
	DWORD  resume(HANDLE hProcess);

	bool CloseAllWindows(ProcessInfo pi, bool wait = false, DWORD waittime = 2333);

	NTSTATUS NtSetInformationProcess(
		HANDLE                    ProcessHandle,
		PROCESS_INFORMATION_CLASS ProcessInformationClass,
		PVOID                     ProcessInformation,
		ULONG                     ProcessInformationLength);
	__kernel_entry NTSTATUS NtQueryInformationProcess(
		HANDLE           ProcessHandle,
		PROCESSINFOCLASS ProcessInformationClass,
		PVOID            ProcessInformation,
		ULONG            ProcessInformationLength,
		PULONG           ReturnLength);

	bool crash(HANDLE hProcess, bool wait = false);

protected:
	bool InitSuspend();
protected:
	SUSPENDPROCESS ZwSuspendProcess;
	RESUMEPROCESS  ZwResumeProcess;
	__NtSetInformationProcess _NtSetInformationProcess;
	__NtQueryInformationProcess _NtQueryInformationProcess;
private:
	HMODULE ntdll;
	vector<Process_t::ProcessInfo> prs;
};
#ifndef _not_usingProcess_t_info
using ProcessInfo = Process_t::ProcessInfo;
#endif
extern Process_t Process;

inline void CloseHandles(vector<HANDLE>& handles) {
	for (auto i : handles) ::CloseHandle(i);
}

namespace std {
	using sstream    =  stringstream;
	using isstream   =  istringstream;
	using osstream   =  ostringstream;
	using wsstream   =  wstringstream;
	using wisstream  =  wistringstream;
	using wosstream  =  wostringstream;
};

template<typename err_type>
bool ExceptionPrintToStdErr(
	const err_type& err,
	const size_t LINE = 0,
	const char* FILE = "",
	bool(__stdcall* CallNextErrorHandle)(const err_type&) = nullptr) {
	try {
		const std::exception* exc;
		exc = dynamic_cast<const std::exception*>(&err);
		if (exc == nullptr || exc == NULL) return false;;
	}
	catch (const std::bad_cast&) {
		return false;
	}
	cerr << "[exception]" << typeid(err).name() << ":" << err.what() << "\n\tAt " <<
		LINE << ":" << FILE << " ,Error " << typeid(err).name() << endl;
	if(!CallNextErrorHandle) return true;
	return CallNextErrorHandle(err);
};

bool WindowShowOnTaskBar(HWND hWnd, bool bShow = true);

errno_t CopyText(const wstring& text, UINT uFormat = CF_UNICODETEXT);
inline errno_t CopyText(const string& text, UINT uFormat = CF_UNICODETEXT) {
	return CopyText(s2ws(text), uFormat);
};
string CopyText(errno_t err);

constexpr long long mkversion(long long _Main, long long _1, long long _2) {
	return _2 + (_1 << 8) + (_Main << 16);
}
constexpr long long mkversionl(long long _Main, long long _1, long long _2) {
	return _2 + (_1 << 16) + (_Main << 32);
}

inline constexpr bool operator ""_b(unsigned long long d) {
	return static_cast<bool>(d);
}

/****************************************************************************
*   函数功能：获取标准UUID，含中划线
*   示例: 70fe761d-ce3c-465e-a260-bb0422bfa670
*   @param 无
*
*   @return UUID
*****************************************************************************/
std::string GenerateUUID();
/****************************************************************************
*   函数功能：获取UUID，不含中划线
*   示例: 70fe761dce3c465ea260bb0422bfa670，共计32位字符
*   @param 无
*
*   @return UUID
*****************************************************************************/
std::string GenerateUUIDWithoutDelim();

void PrintTextEachCharW(wstring text, DWORD timeout = 0);
void PrintTextEachCharA( string text, DWORD timeout = 0);
#ifdef UNICODE
inline void PrintTextEachChar(wstring text, DWORD timeout = 0) {
	return PrintTextEachCharW(text, timeout);
#else
inline void PrintTextEachChar( string text, DWORD timeout = 0) {
	return PrintTextEachCharA(text, timeout);
#endif
}

string GetCurrentUserSecurityId();

LPWSTR ErrorCodeToStringW(DWORD ErrorCode);
LPSTR  ErrorCodeToStringA(DWORD ErrorCode);
inline LPWSTR LastErrorStrW() {
	return ErrorCodeToStringW(GetLastError());
}
inline LPSTR  LastErrorStrA() {
	return ErrorCodeToStringA(GetLastError());
}
#ifdef UNICODE
inline LPTSTR ErrorCodeToString(DWORD ErrorCode) {
	return ErrorCodeToStringW(ErrorCode);
}
inline LPTSTR LastErrorStr() {
	return ErrorCodeToString(GetLastError());
}
#else
inline LPTSTR ErrorCodeToString(DWORD ErrorCode) {
	return ErrorCodeToStringA(ErrorCode);
}
inline LPTSTR LastErrorStr() {
	return ErrorCodeToString(GetLastError());
}
#endif

inline HANDLE CreateFileSimpleW(
	LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwCreationDisposition,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL,
	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL,
	HANDLE hTemplateFile = NULL
) {
	return CreateFileW(lpFileName,
		dwDesiredAccess, 
		dwShareMode, 
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}

[[nodiscard("Ignore the return value will caugh Memory Leak")]] 
void* GetBufMemory(size_t& maxsize);

#define AutoZeroMemory(_v) memset(&(_v),0,sizeof(_v))

#define srand_with_time() srand((unsigned)time(0))

wstring GetProcessStdOutputW(wstring cmd_line);
inline string GetProcessStdOutputA(string cmd_line) {
	return ws2s(GetProcessStdOutputW(s2ws(cmd_line)));
}

inline bool is_number(string str) {
	for (auto i : str) {
		if (!isdigit(i)) return false;
	}
	return true;
}

ULONGLONG MyGetFileSizeW(wstring filename);
inline ULONGLONG MyGetFileSizeA(string filename) {
	return MyGetFileSizeW(s2ws(filename));
}

/**
IsFileOrDirectory (ASCII)
@brief Check a path is file or directory
@param path The file or directory path.
@return -1 -> Directory    0  -> Not Exists    1  -> File
*/
signed char IsFileOrDirectory(string path);
/**
IsFileOrDirectory (Unicode)
@brief 检测某string是目录还是文件
@param path 文件(夹)路径
@return -1 -> 目录    0  -> No such file or directory    1  -> 文件
*/
signed char IsFileOrDirectory(wstring path);

bool LoadLibAsProcessW(HANDLE hProcess, wstring LibPath);
inline bool LoadLibAsProcessA(HANDLE hProcess, string LibPath) {
	return ::LoadLibAsProcessW(hProcess, s2ws(LibPath));
}

#endif // defined(_TOOL_SHC0743_MAIN)
