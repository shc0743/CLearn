#pragma once
#include<string>
#include<vector>
#include<exception>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include<WS2tcpip.h>
#pragma comment(lib,"wsock32.lib")
#include<Windows.h>

#define SOCK_FTRANS_VERSION(main_version,sec_version,ver) \
(((main_version)*65536) + ((sec_version)*256) + (ver))

namespace SockFileTrans {
	using ush = unsigned short;
	using lpchar = char*;
	using clpchar = const lpchar;
	constexpr unsigned long long buffer_size_max = 1073741824;
	using std::string;
	using std::to_string;

        static std::vector<std::string>& str_split(
		const std::string& src,
		const std::string separator,
		std::vector<std::string>& dest)	{
		string str = src;
		string substring;
		string::size_type start = 0, index;
		dest.clear();
		index = str.find_first_of(separator, start);
		do {
			if (index != string::npos)
			{
				substring = str.substr(start, index - start);
				dest.push_back(substring);
				start = index + separator.size();
				index = str.find(separator, start);
				if (start == string::npos) break;
			}
		} while (index != string::npos);
		substring = str.substr(start);
		dest.push_back(substring);
		return dest;
	}


	inline long long version(int main_version,int sec_version,int ver) {
		return (
			(long long)((long long)(main_version)*65536) + 
			(long long)((long long)(sec_version)*256) + 
			(long long)((long long)(ver)));
	};
	inline long long version(HMODULE dll) {
		auto _ = ((long long(*)())GetProcAddress(dll, "version"));
		if (!_) return 0; return _();
	};
	inline void about(HMODULE dll, char* buffer, size_t bufLength) {
		auto _ = ((void(*)(char*, size_t))GetProcAddress(dll, "about"));
		if (!_) return; return _(buffer, bufLength);
	};
	inline void SetBufferSizeMax(HMODULE dll, size_t max_buffer_size) {
		auto _ = ((void(*)(size_t))GetProcAddress(dll, "SetBufferSizeMax"));
		if (!_) return; return _(max_buffer_size);
	};

	class ip {
	public:
		ip() :_1(0), _2(0), _3(0), _4(0) {};
		ip(ush _1, ush _2, ush _3, ush _4) :
			_1(_1), _2(_2), _3(_3), _4(_4) {
				stringbuf = (
					to_string(_1) + ':' +
					to_string(_2) + ':' +
					to_string(_3) + ':' +
					to_string(_4) + ':'
				);
			};
		ip(const string& str) {
			this->operator=(str);
		};
		~ip() {};
		ip& operator=(const ip& src) {
			_1 = src._1;
			_2 = src._2;
			_3 = src._3;
			_4 = src._4;
			stringbuf = (
				to_string(_1) + ':' +
				to_string(_2) + ':' +
				to_string(_3) + ':' +
				to_string(_4) + ':'
			);
			return *this;
		};
		ip& operator=(const string& str) {
			std::vector<std::string> vc;
			str_split(str, ".", vc);
			try {
				_1 = (ush)atoi(vc[0].c_str());
				_2 = (ush)atoi(vc[1].c_str());
				_3 = (ush)atoi(vc[2].c_str());
				_4 = (ush)atoi(vc[3].c_str());
			}
			catch (std::exception&) {
				(void)void(0);
			}
			stringbuf = str;
			return *this;
		};
		operator clpchar() const{
			//return (clpchar)((
			//	to_string(_1) + ':' +
			//	to_string(_2) + ':' +
			//	to_string(_3) + ':' +
			//	to_string(_4) + ':'
			//).c_str());
			return (clpchar)(stringbuf.c_str());
		};
	private:
		ush _1;
		ush _2;
		ush _3;
		ush _4;
		string stringbuf;
	};

	namespace t {
		using callback = void(*)(size_t completed, size_t total);
		using LastErr = string(*)();

		using SendFile = bool(*)(ip, ush, string, t::callback);
		using RecvFile = bool(*)(ip, ush, string, t::callback);
		using SendFileAsync = HANDLE(*)(ip, ush, string, void(*)(bool), t::callback);
		using RecvFileAsync = HANDLE(*)(ip, ush, string, void(*)(bool), t::callback);
	};

	using remote_info = struct _remote_info_t {
		ip ip;
		ush port = 0;
		string fname;
		void(*callback)(bool) = nullptr;
		t::callback callback_ = nullptr;
	};

	static string LastErr(HMODULE dll) {
		t::LastErr e = (t::LastErr)GetProcAddress(dll, "LastErr");
		if (!e) return "";
		return e();
	};

	static bool init(
		HMODULE dll,
		t::SendFile* sd = nullptr, t::RecvFile* rv = nullptr,
		t::SendFileAsync* sda = nullptr, t::RecvFileAsync* rva = nullptr,
		t::LastErr* lee = nullptr) {
		if (!dll) return false;
		if(sd ) *sd  = (t::SendFile)GetProcAddress(dll, "SendFile");
		if(rv ) *rv  = (t::RecvFile)GetProcAddress(dll, "RecvFile");
		if(sda) *sda = (t::SendFileAsync)GetProcAddress(dll, "SendFileAsync");
		if(rva) *rva = (t::RecvFileAsync)GetProcAddress(dll, "RecvFileAsync");
		if(lee) *lee = (t::LastErr)GetProcAddress(dll, "LastErr");
		return /*sd && rv && sda && rva && lee*/true;
	};
};
