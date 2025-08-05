#include "pch.h"
#include <filesystem>
#include <mini/ini.h>
namespace fs = std::filesystem;
using namespace mINI;

void sun() {}

// 标准化字符串中的换行符为unix标准
void unix_line_feed(char* str)
{
    const auto len = strlen(str);
    for (int i = 0;i < len;++i)
    {
        auto& c = str[i];
        if (c == '\r')
        {
            if (i >= len - 1 || str[i + 1] != '\n')  // \r
                c = '\n';
            else // \r\n
            {
                for (int j = i + 1;j < len;++j)
                {
                    str[j - 1] = str[j];
                }
                str[len - 1] = '\0';
            }
        }
    }
}

inline int multi2wide(const char* multi, std::wstring& wide, UINT CP)
{
    int sz = MultiByteToWideChar(CP, 0, multi, -1, nullptr, 0);
    if (sz < 1) return 0;
    wide.resize(sz);
    auto ret = MultiByteToWideChar(CP, 0, multi, -1, wide.data(), sz);
    if (ret > 0) wide.resize(sz - 1);   // 输出不包括nul结尾
    return ret;
}

inline int wide2multi(const wchar_t* wide, std::string& multi, UINT CP)
{
    auto sz = WideCharToMultiByte(CP, 0, wide, -1, nullptr, 0, 0, 0);
    if (sz < 1) return 0;
    multi.resize(sz);
    auto ret = WideCharToMultiByte(CP, 0, wide, -1, multi.data(), sz, 0, 0);
    if (ret > 0) multi.resize(sz - 1);   // 输出不包括nul结尾
    return ret;
}

int ansi_to_wide(const char* ansi, std::wstring& out)
{
    return multi2wide(ansi, out, GetACP());
}

int wide_to_ansi(const wchar_t* wide, std::string& out)
{
    return wide2multi(wide, out, GetACP());
}

int u8_to_wide(const char* u8str, std::wstring& out)
{
    return multi2wide(u8str, out, CP_UTF8);
}

int wide_to_u8(const wchar_t* wide, std::string& out)
{
    return wide2multi(wide, out, CP_UTF8);
}

int wide_to_u32(const wchar_t* wide, std::u32string& out)
{
    out.clear();
    ck::Text::u16to32(wide, out);
    return out.size();
}

std::string find_file(
    const std::string& root,
    const std::string& filename,
    bool recursive
)
{
    std::string ret;
    if (!fs::is_directory(root))
        return ret;
    std::vector<std::string> dirs;
    for (auto& it : fs::directory_iterator(root))
    {
        if (recursive && it.is_directory())
        {
            dirs.push_back(it.path().string());
            continue;
        }
        if (!it.is_regular_file())
            continue;
        auto& path = it.path();
        auto name = path.filename().string();
        if (stricmp(name.c_str(), filename.c_str()) == 0)
        {
            ret = path.string();
            return ret;
        }
	}

    if (recursive)
    {
        for (auto& it : dirs)
        {
            ret = find_file(root + "/" + it, filename, recursive);
            if (!ret.empty())
                return ret;
        }
    }
    return ret;
}

std::string g_module_dir;
void get_module_dir()
{
    if (!g_module_dir.empty())
        return;
    char buffer[1024]{ 0 };
    GetModuleFileNameA(NULL, buffer, 1024);
    g_module_dir = buffer;
    std::replace(g_module_dir.begin(), g_module_dir.end(), '\\', '/');
    auto pos = g_module_dir.rfind('/');
    g_module_dir = g_module_dir.substr(0, pos);
    if (g_module_dir.back() != '/')
        g_module_dir.push_back('/');
}

decltype(g_setting) g_setting;
void load_setting()
{
    INIFile fi(g_module_dir + "SUN_I18N.ini");
    INIStructure ini;
    fi.read(ini);
    auto& sec = ini["Setting"];
    g_setting.hide_unit_select = equalnc(sec["HideUnitSelect"].c_str(), "Yes");
    g_setting.show_subtitle = equalnc(sec["MovieSubtitle"].c_str(), "Yes");
    g_setting.loc = sec["Lang"];
    if (g_setting.loc.empty())
        g_setting.loc = "en-us";
}

bool warning(const std::string& text, const std::string& title)
{
    return MessageBoxA(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONWARNING) == IDOK;
}

bool info(const std::string& text, const std::string& title)
{
    return MessageBoxA(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION) == IDOK;
}

bool error(const std::string& text, const std::string& title)
{
    return MessageBoxA(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONERROR) == IDOK;
}

bool confirm(const std::string& text, const std::string& title)
{
    return MessageBoxA(NULL, text.c_str(), title.c_str(), MB_YESNO | MB_ICONQUESTION) == IDYES;
}
