#ifndef PCH_H
#define PCH_H

#include <windows.h>
#include <stdint.h>

#include "common/patch.h"
#include "hook/hook.h"
#include "resource.h"
#include "tspp_ex/globals.h"
#include <tspp/src/lib/rgb.h>
#include <tspp/src/lib/hsv.h>

extern "C" {
    __declspec(dllexport) void sun();
}

extern HINSTANCE hInstance;
extern std::string g_module_dir;
extern std::vector<std::string> g_search_dirs;
extern int g_cliX, g_cliY;  // 主窗口客户区起始坐标

extern struct {
    bool hide_unit_select;
    bool show_subtitle;
    std::string loc;
} g_setting;

// 标准化字符串中的换行符为unix标准
void unix_line_feed(char* str);

void get_module_dir();

void load_setting();

int ansi_to_wide(const char* ansi, std::wstring& out);

int wide_to_ansi(const wchar_t* wide, std::string& out);

int u8_to_wide(const char* u8str, std::wstring& out);

int wide_to_u8(const wchar_t* wide, std::string& out);

int wide_to_u32(const wchar_t* wide, std::u32string& out);

inline void u8_to_u32(const char* u8str, std::u32string& out) {
    ck::Text::u8to32(u8str, out);
}

inline int ansi_to_u32(const char* ansi, std::u32string& out) {
    std::wstring wide;
    if (ansi_to_wide(ansi, wide) < 1)
        return 0;
    return wide_to_u32(wide.c_str(), out);
}

inline int ansi_to_u8(const char* ansi, std::string& out) {
    std::wstring wide;
    if(ansi_to_wide(ansi, wide) < 1)
        return 0;
    return wide_to_u8(wide.c_str(), out);
}

inline int u8_to_ansi(const char* u8str, std::string& out) {
    std::wstring wide;
    if (u8_to_wide(u8str, wide) < 1)
        return 0;
    return wide_to_ansi(wide.c_str(), out);
}

inline bool equal(const char* a, const char* b) {
    return strcmp(a, b) == 0;
}

inline bool equalnc(const char* a, const char* b) {
    return _stricmp(a, b) == 0;
}

inline ck::color to_color(const RGBClass& rgb) {
    return ck::rgb(rgb.Red, rgb.Green, rgb.Blue);
}

inline ck::color to_color(const HSVClass& hsv) {
    return to_color((RGBClass)hsv);
}

std::string find_file(
    const std::string& root,
    const std::string& filename,
    bool recursive
);

bool warning(const std::string& text, const std::string& title = "Warning");
bool info(const std::string& text, const std::string& title = "Info");
bool error(const std::string& text, const std::string& title = "Error");
bool confirm(const std::string& text, const std::string& title = "Confirm");

template<typename C>
static void prepare_text(C* lpText)
{
    auto len = std::char_traits<C>::length(lpText);
    for (int i = 0;i < len;++i)
    {
        if (lpText[i] == '@')
            lpText[i] = '\n';
    }
}


#endif // PCH_H
