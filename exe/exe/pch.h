// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"
#include "afxdialogex.h"
#include <string>
#include <vector>

extern std::string g_module_dir;
void get_module_dir();

std::vector<std::string> enum_children(
	const std::string& dir,
	int which	// 0: 文件 1:目录 2:文件和目录
);

std::string find_child(
	const std::string& dir,
	const std::string& name, 
	bool directory,
	bool recursive
);

int ansi_to_wide(const char* ansi, std::wstring& out);
int wide_to_ansi(const wchar_t* wide, std::string& out);
int u8_to_wide(const char* u8str, std::wstring& out);
int wide_to_u8(const wchar_t* wide, std::string& out);

inline bool equal(const char* a, const char* b) {
	return strcmp(a, b) == 0;
}

inline bool equalnc(const char* a, const char* b) {
	return _stricmp(a, b) == 0;
}


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#endif //PCH_H
