// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"
#include <algorithm>
// 当使用预编译的头时，需要使用此源文件，编译才能成功。

std::string g_module_dir;

void get_module_dir()
{
    char buffer[1024];
    GetModuleFileNameA(0, buffer, 1024);
    g_module_dir = buffer;
    std::replace(g_module_dir.begin(), g_module_dir.end(), '\\', '/');
    auto pos = g_module_dir.rfind('/');
    g_module_dir = g_module_dir.substr(0, pos);
    if (g_module_dir.back() != '/')
        g_module_dir.push_back('/');
}

std::vector<std::string> enum_children(
    const std::string& dir,
    int which
)
{
    WIN32_FIND_DATAA fd;
    HANDLE hFind = ::FindFirstFileA((dir + "/*").c_str(), &fd);

    std::vector<std::string> ret;
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do {
            if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
                continue;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (which > 0)
                    ret.push_back(fd.cFileName);
            }
            else if (which != 1)
                ret.push_back(fd.cFileName);
        } while (::FindNextFileA(hFind, &fd));

        ::FindClose(hFind);
    }
    return ret;
}

std::string find_child(
    const std::string& dir,
    const std::string& name,
    bool directory,
    bool recursive
)
{
    WIN32_FIND_DATAA fd;
    HANDLE hFind = ::FindFirstFileA((dir + "/*").c_str(), &fd);

    std::string ret;
    std::vector<std::string> subs;
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do {
            if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
                continue;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
				if (directory && _stricmp(fd.cFileName, name.c_str()) == 0)
				{
					ret = dir + "/" + name;
					return ret;
				}
				if (recursive)
					subs.push_back(fd.cFileName);
            }
            else if (!directory && _stricmp(fd.cFileName, name.c_str()) == 0)
            {
                ret = dir + "/" + name;
                return ret;
            }
        } while (::FindNextFileA(hFind, &fd));

        ::FindClose(hFind);
    }

    if (recursive)
    {
        std::string path;
        for (auto& it : subs)
        {
            path = find_child(dir + "/" + it, name, directory, recursive);
            if (!path.empty())
            {
                ret = path;
                return ret;
            }
        }
    }

    return ret;
}


inline int multi2wide(const char* multi, std::wstring& wide, UINT CP)
{
    int sz = MultiByteToWideChar(CP, 0, multi, -1, nullptr, 0);
    if (sz < 1) return 0;
    wide.resize(sz);
    auto ret = MultiByteToWideChar(CP, 0, multi, -1, (wchar_t*)wide.data(), sz);
    if (ret > 0) wide.resize(sz - 1);   // 输出不包括nul结尾
    return ret;
}

inline int wide2multi(const wchar_t* wide, std::string& multi, UINT CP)
{
    auto sz = WideCharToMultiByte(CP, 0, wide, -1, nullptr, 0, 0, 0);
    if (sz < 1) return 0;
    multi.resize(sz);
    auto ret = WideCharToMultiByte(CP, 0, wide, -1, (char*)multi.data(), sz, 0, 0);
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
