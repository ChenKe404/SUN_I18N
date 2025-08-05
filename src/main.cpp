#include <iostream>
#include <dohook.h>
#include <minidumpapiset.h>

#pragma comment(lib, "dbghelp.lib")

char tmp[2048];
template<typename ..._Args>
void output_debug(const char* fmt, _Args... args)
{
    sprintf(tmp, fmt, std::forward<_Args>(args)...);
    OutputDebugStringA(tmp);
    OutputDebugStringA("\n");
}

extern void init_patch();

static LONG WINAPI _UnhandledExceptionFilter_(PEXCEPTION_POINTERS pExceptionInfo)
{
    time_t t = time(NULL);
    struct tm tm;
    localtime_s(&tm, &t);

    char name[MAX_PATH];
    sprintf(name, "dump_i18n_%04d%02d%02d_%02d%02d%02d.dmp",
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec
    );

    auto path = g_module_dir + "/dump/";
    if (!CreateDirectoryA(path.c_str(), NULL))
        return EXCEPTION_EXECUTE_HANDLER;
    path.append(name);
    HANDLE hFile = CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION mdei{ 0 };
        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = pExceptionInfo;
        mdei.ClientPointers = FALSE;

        MINIDUMP_TYPE type = MINIDUMP_TYPE(
            MiniDumpNormal | MiniDumpWithDataSegs
        );
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, type, &mdei, NULL, NULL);

        CloseHandle(hFile);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

HINSTANCE hInstance;
extern "C" BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved
) {
    switch(fdwReason) {
    case DLL_PROCESS_ATTACH:
    {
        hInstance = hinstDLL;
        get_module_dir();

        SetUnhandledExceptionFilter(_UnhandledExceptionFilter_);

        load_setting();
        if (!equalnc(g_setting.loc.c_str(), "en-us"))
        {
            bool flag = true;
            if (!init_resource())
            {
                flag = false;
                if (!confirm("Some resources cannot be loaded, do you want continue play this game without i18n patch?"))
                    exit(1);
            }
            if (flag)
            {
                hook_on_dohook();
                init_patch();
            }
        }
    }
    break;
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        break;
    }
    return TRUE;
}

