#include "tspp_ex/scoreanim.h"
#include "my_scoreanim.h"
#include "tspp_ex/scorescreen.h"
#include "scenario.h"
#include "dohook.h"

static const char* u8str;

ScoreAnimClass* __stdcall make_score_anim(int nX, int nY, int align)
{
    std::u32string u32str;
    to_u32str(u8str, u32str);
    auto& fnt = Scen->IsGDI ? g_fullfnt3gdi : g_fullfnt3nod;
    auto chrs = fnt.cs(u32str);
    if (align == 1)  // ÖÐ
        nX -= fnt.measure(chrs).Width / 2;
    else if (align == 2)  // ÓÒ
        nX -= 0;
    else  // ×ó
        nX -= fnt.measure(chrs).Width;
    return new MYScoreAnim(chrs, &fnt, nX, nY);
}

// align ÎÄ±¾¶ÔÆë 0:¿¿×ó 1:¾ÓÖÐ  2:¿¿ÓÒ
#define PATCH_SCORE_PRINT(ox, oy, align, func) \
__asm { push align } \
__asm { mov eax, [ebp + 4] } \
__asm { add eax, oy } \
__asm { push eax } \
__asm { mov eax, [ebp + 0] } \
__asm { add eax, ox } \
__asm { push eax } \
__asm { call func }  \
__asm { mov edi, eax }

__declspec(noinline) __declspec(naked)
void patch_score_print_0()  // MISSION EFFICIENCY
{
    __asm { mov u8str, eax }
    PATCH_SCORE_PRINT(552, 13, 1, make_score_anim)
    __asm {
        mov eax, 0x005E3854
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_score_print_1()  // CURRENCY
{
    __asm { mov u8str, eax }
    PATCH_SCORE_PRINT(560, 193, 1, make_score_anim)
    __asm {
        mov eax, 0x005E3AB3
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_score_print_2()  // MISSION TIME LAPSE
{
    __asm { mov u8str, eax }
    PATCH_SCORE_PRINT(542, 366, 1, make_score_anim)
    __asm {
        mov eax, 0x005E3D73
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_score_print_3()  // CASUALTIES
{
    __asm { mov u8str, eax }
    PATCH_SCORE_PRINT(316, 300, 1, make_score_anim)
    __asm {
        mov eax, 0x005E401D
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_score_print_4()  // Structures ×ó
{
    __asm { mov u8str, eax }
    PATCH_SCORE_PRINT(233, 247, 1, make_score_anim)
    __asm {
        mov eax, 0x005E40F8
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_score_print_5()  // Structures ÓÒ
{
    PATCH_SCORE_PRINT(366, 247, 1, make_score_anim)
    __asm {
        add esp, 4  // Õ»Æ½ºâ
        mov eax, 0x005E4192
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_score_print_6()  // Units ×ó
{
    __asm { mov u8str, eax }
    PATCH_SCORE_PRINT(290, 247, 1, make_score_anim)
    __asm {
        mov eax, 0x005E427D
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_score_print_7()  // Units ÓÒ
{
    PATCH_SCORE_PRINT(422, 247, 1, make_score_anim)
    __asm {
        mov eax, 0x005E432B
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_score_print_8()  // Best Scores
{
    __asm { mov u8str, eax }
    PATCH_SCORE_PRINT(84, 217, 1, make_score_anim)
    __asm {
        mov eax, 0x005E45ED
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_score_print_9()  // Click to Continue
{
    __asm { mov u8str, eax }
    PATCH_SCORE_PRINT(320, 357, 1, make_score_anim)
    __asm {
        mov esi, edi
        mov eax, 0x005E4AC5
        jmp eax
    }
}

/*
__declspec(noinline) __declspec(naked)
void patch_on()
{
    __asm {
        mov eax, 0x005E4A2C
        jmp eax
    }
}
*/

__declspec(noinline) __declspec(naked)
static void hook_fetch_on()
{
    static void* _rst, *_eax;
    __asm {
        mov _rst, 0x005E2EDA
        mov eax, [ecx]
        call dword ptr[eax + 0x78]
        mov _eax, eax
    }
    H_Fetch_String.rehook();
    __asm {
        mov eax, _eax
        jmp dword ptr [_rst]
    }
}

__declspec(noinline) __declspec(naked)
static void hook_fetch_off()
{
    __asm {
        mov eax, 0x004FB4F0
        mov ecx, 0x007482C0
        call eax
    }
    H_Fetch_String.unhook();
    __asm {
        mov eax, 0x005E4B14
        jmp eax
    }
}

void init_patch_score_print()
{
    write_cmd(0x005E2ED5, hook_fetch_on, OP_JMP); // fetch  hook on
    write_cmd(0x005E4B09, hook_fetch_off, OP_JMP); // fetch  hook off

    write_cmd(0x005E37DC, patch_score_print_0, OP_JMP); // MISSION EFFICIENCY
    write_cmd(0x005E3A39, patch_score_print_1, OP_JMP); // CURRENCY
    write_cmd(0x005E3CF9, patch_score_print_2, OP_JMP); // MISSION TIME LAPSE
    write_cmd(0x005E3F9F, patch_score_print_3, OP_JMP); // CASUALTIES

    write_cmd(0x005E409E, patch_score_print_4, OP_JMP); // Structures ×ó
    write_cmd(0x005E4133, patch_score_print_5, OP_JMP); // Structures ÓÒ
    
    write_cmd(0x005E420A, patch_score_print_6, OP_JMP); // Units ×ó
    write_cmd(0x005E42B8, patch_score_print_7, OP_JMP); // Units ÓÒ

    write_cmd(0x005E4568, patch_score_print_8, OP_JMP); // Best Scores
    write_cmd(0x005E4A36, patch_score_print_9, OP_JMP); // Click to continue
    //write_cmd(0x005E49F0, patch_on, OP_JMP); // ÏÔÊ¾ Click to continue
}
