#include <iostream>
#include <dohook.h>

bool init(int argc,char* argv[]);

char tmp[2048];

template<typename ..._Args>
void output_debug(const char* fmt, _Args... args)
{
    sprintf(tmp, fmt, std::forward<_Args>(args)...);
    OutputDebugStringA(tmp);
    OutputDebugStringA("\n");
}

bool init(int argc,char* argv[]);
ck::Hook <
    0x004DFB70,
    init,
    false
    > H_Init_Game;
bool init(int argc,char* argv[])
{
    OutputDebugStringA("Hook Success!!!");

    return H_Init_Game(argc,argv);
}

extern void init_patch();

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
                H_Init_Game.rehook();
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
