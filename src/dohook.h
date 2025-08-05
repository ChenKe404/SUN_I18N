#ifndef DO_HOOK_H
#define DO_HOOK_H

#include <textprint.h>
#include "re_restatemission.h"

// 重新实现的Fetch_String
// @uID  资源ID
// @return utf8编码的译文或原文(如果译文不存在)
const char* Re_Fetch_String(UINT uID);
extern ck::Hook <
    0x00472350,
    Re_Fetch_String,
    false
> H_Fetch_String;

BOOL WINAPI Re_ShowWindow(HWND hWnd, int nCmdShow);
extern ck::Hook <
    ShowWindow,
    Re_ShowWindow,
    false
> H_ShowWindow;

int __fastcall Re_Draw_Dlg_Text(Surface* pSur, const char* u8str, RECT* pRect, const char* aFontName, COLORREF a5, char a6, int a7);
extern ck::Hook <
    0x0059E100,
    Re_Draw_Dlg_Text,
    false
> H_Draw_Dlg_Text;

struct Re_BufferIOFileClass
{
    using Self = Re_BufferIOFileClass;

    int Set_Name(const char* Source);
    DEF_HOOKMEM(0x00421090, &Self::Set_Name, false, H_Set_Name)
};

struct Re_MSFont
{
    using Self = Re_MSFont;

    MSFont* constructor(bool bSide);
    DEF_HOOKMEM(0x00571E50, &Self::constructor, false, H_constructor)

    void destructor();
    DEF_HOOKMEM(0x00571F60, &Self::destructor, false, H_destructor)

    static MyFont* fetch(MSFont*);
};

inline void hook_on_dohook()
{
    H_ShowWindow.rehook();
    H_Draw_Dlg_Text.rehook();

    Re_BufferIOFileClass::H_Set_Name.rehook();

    RE_RestateMission::H_Draw_Text.rehook();
    RE_RestateMission::H_Start_Briefing_Screen.rehook();

    Re_MSFont::H_constructor.rehook();
    Re_MSFont::H_destructor.rehook();
}

#endif // !DO_HOOK_H
