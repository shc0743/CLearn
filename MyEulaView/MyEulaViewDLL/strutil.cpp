#include "strutil.h"
#include "../../resource/tool.h"


#if 0
std::string str_to_u8str(std::string codepage_str) {
	int size = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, codepage_str.c_str(),
		codepage_str.length(), nullptr, 0);
	std::wstring utf16_str(size, '\0');
	MultiByteToWideChar(CP_ACP, MB_COMPOSITE, codepage_str.c_str(),
		codepage_str.length(), &utf16_str[0], size);
	return str_to_u8str(utf16_str);
}

std::string str_to_u8str(std::wstring utf16_str) {
	int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
		utf16_str.length(), nullptr, 0,
		nullptr, nullptr);
	std::string utf8_str(utf8_size, '\0');
	WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
		utf16_str.length(), (LPSTR)(utf8_str[0]), utf8_size,
		nullptr, nullptr);
	return utf8_str;
}
#else
/* https://mariusbancila.ro/blog/2008/10/20/writing-utf-8-files-in-c/ */
std::string str_to_u8str(const wchar_t* buffer, DWORD len) {
	using namespace std;

	int nChars = ::WideCharToMultiByte(CP_UTF8, 0, buffer, len, NULL, 0, NULL, NULL);
	if (nChars == 0) return "";

	string newbuffer;
	newbuffer.resize(nChars);
	::WideCharToMultiByte(
		CP_UTF8, 0, buffer, len,
		const_cast<char*>(newbuffer.c_str()),
		nChars, NULL, NULL);

	return newbuffer;
}
std::string str_to_u8str(const std::wstring& str) {
	return str_to_u8str(str.c_str(), DWORD(str.size()));
}
std::string str_to_u8str(const std::string& str) {
	return str_to_u8str(s2ws(str));
}
#endif


