#include "tspp_ex/scoreanim.h"
#include <msengine.h>
#include <multiscore.h>
#include "my_printanim.h"
#include "dohook.h"

static const char* g_string;
static MyFont::CharPtrs g_chrs;
static int g_width;

static constexpr auto nInterval = 3;

void __stdcall draw_text(MultiScore* ms, int ox, int x, int y)
{
    std::u32string u32str;
    auto u8 = g_text.u8(g_string);
    if (u8 != g_string)
        g_string = u8;
    to_u32str(g_string, u32str);

    auto fnt = Re_MSFont::fetch(ms->pFnt);
    g_chrs = fnt->cs(u32str);
    g_width = fnt->measure(g_chrs).Width;
    x += (ox - g_width) / 2;
    fnt->draw(g_chrs, ms->pSur, x, y);
}

void __stdcall print_anim(MultiScore* ms, int ox, int x, int y)
{
    x += (ox - g_width) / 2;
    auto fnt = Re_MSFont::fetch(ms->pFnt);
    auto anim = new MYPrintAnim(&g_chrs, fnt, x + ms->nX, y + ms->nY);
    anim->nInterval = nInterval;
    ms->Attach_Anim(anim);
    ms->Play_Anim(anim, 300000);
}

// 直接格式化到静态缓冲区, 忽略第一个参数
void __stdcall format(char*, const char* fmt, ...)
{
    static char buffer[1024];
    va_list va;
    va_start(va, fmt);
    auto u8 = g_text.u8(fmt);
    if (u8 != fmt)
        fmt = u8;
    vsprintf(buffer, fmt, va);
    va_end(va);

    std::u32string u32str;
    to_u32str(buffer, u32str);
    g_chrs = g_fullfnt3.cs(u32str);
}

void __stdcall play_print_anim_0(MultiScore* ms, int x, int y)
{
    auto fnt = Re_MSFont::fetch(ms->pFnt);
    auto anim = new MYPrintAnim(&g_chrs, fnt, x + ms->nX, y + ms->nY);
    anim->nInterval = nInterval;
    ms->Attach_Anim(anim);
    ms->Play_Anim(anim, 300000);
}

void __stdcall play_print_anim_1(MultiScore* ms, int x, int y)
{
    auto fnt = Re_MSFont::fetch(ms->pFnt);
    x += 625 - fnt->measure(g_chrs).Width;
    auto anim = new MYPrintAnim(&g_chrs, fnt, x + ms->nX, y + ms->nY);
    anim->nInterval = nInterval;
    ms->Attach_Anim(anim);
    ms->Play_Anim(anim, 300000);
}

void __stdcall draw_text_1(MultiScore* ms, int x, int y)
{
    std::u32string u32str;
    u8_to_u32(g_string, u32str);

    auto fnt = Re_MSFont::fetch(ms->pFnt);
    g_chrs = fnt->cs(u32str);
    fnt->draw(g_chrs, ms->pSur, x, y);
}

// 玩家名称可以从游戏的窗口中输入, 所以使用的是系统本地多字节编码
// 这里将其转码为utf8, 后续在draw_text_1转为字符列表
char* __cdecl patch_strncpy(char* Destination, const char* Source, size_t Count)
{
    std::string u8;
    ansi_to_u8(Source, u8);
    auto sz = std::min(u8.size(), Count);
    return strncpy(Destination, u8.data(), sz);
}

void __stdcall print_anim_1(MultiScore* ms, int x, int y)
{
    using AL = MyFont::Align;
    auto fnt = Re_MSFont::fetch(ms->pFnt);
    auto& opts = fnt->options();
    const auto old = opts.align;
    opts.align = AL::AL_LEFT | AL::AL_VCENTER;  // 垂直居中
    auto anim = new MYPrintAnim(&g_chrs, fnt, x + ms->nX, y + ms->nY);
    anim->nInterval = nInterval;
    ms->Attach_Anim(anim);
    ms->Play_Anim(anim, 300000);
    opts.align = old;
}

void __stdcall print_anim_2(MultiScore* ms, int y)
{
    std::u32string u32str;
    to_u32str(g_string, u32str);
    auto fnt = Re_MSFont::fetch(ms->pFnt);
    g_chrs = fnt->cs(u32str);
    g_width = fnt->measure(g_chrs).Width;
    auto x = 320 - g_width / 2;
    auto anim = new MYPrintAnim(&g_chrs, fnt, x + ms->nX, y + ms->nY);
    anim->nInterval = nInterval;
    ms->Attach_Anim(anim);
    ms->Play_Anim(anim, 300000);
}

#define DRAW_TEXT(ox,x,y) \
__asm { mov g_string, eax } \
__asm { mov ecx, y } \
__asm { push ecx } \
__asm { mov ecx, x } \
__asm { push ecx } \
__asm { mov ecx, ox } \
__asm { push ecx } \
__asm { push esi } \
__asm { call draw_text }

#define PRINT_ANIM(ox,x,y) \
__asm { mov ecx, y } \
__asm { push ecx } \
__asm { mov ecx, x } \
__asm { push ecx } \
__asm { mov ecx, ox } \
__asm { push ecx } \
__asm { push esi } \
__asm { call print_anim }

__declspec(noinline) __declspec(naked)
void patch_multiscore_print_0()  // Game: %d
{
    __asm {
        mov ecx, 15
        push ecx
        mov ecx, 15
        push ecx
        push esi
        call play_print_anim_0

        mov eax, 0x00568D10
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_multiscore_print_1()  // Time:%02d:%02d:%02d
{
    __asm {
        mov ecx, 15
        push ecx
        mov ecx, 0
        push ecx
        push esi
        call play_print_anim_1

        mov eax, 0x00568EB4
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_multiscore_print_2a()  // Players
{
    DRAW_TEXT(100, 15, 45);
    __asm {
        mov eax, 0x00568F5F
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_multiscore_print_2b()  // Players
{
    PRINT_ANIM(100, 15, 45);
    __asm {
        mov eax, 0x00569072
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_multiscore_print_3a()  // Losses
{
    DRAW_TEXT(110, 147, 45);
    __asm {
        mov eax, 0x00569187
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_multiscore_print_3b()  // Losses
{
    PRINT_ANIM(110, 147, 45);
    __asm {
        mov eax, 0x005692A3
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_multiscore_print_4a()  // Kills
{
    DRAW_TEXT(110, 269, 45);
    __asm {
        mov eax, 0x005693B0
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_multiscore_print_4b()  // Kills
{
    PRINT_ANIM(110, 269, 45);
    __asm {
        mov eax, 0x005694CC
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_multiscore_print_5a()  // Economy
{
    DRAW_TEXT(110, 391, 45);
    __asm {
        mov eax, 0x005695D9
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_multiscore_print_5b()  // Economy
{
    PRINT_ANIM(110, 391, 45);
    __asm {
        mov eax, 0x005696F5
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_multiscore_print_6a()  // Score
{
    DRAW_TEXT(90, 530, 45);
    __asm {
        mov eax, 0x0056980A
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_multiscore_print_6b()  // Score
{
    PRINT_ANIM(90, 530, 45);
    __asm {
        mov eax, 0x00569926
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_player_name_0()
{
    // 玩家名称起始垂直坐标设置为90
    __asm {
        mov ebx, 90
        mov eax, 0x005699C3
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_player_name_1()
{
    __asm {
        mov edx, [ebp+0]
        mov g_string, edx
        push ebx            // y
        mov ecx, 20
        push ecx            // x
        push esi
        call draw_text_1

        push ebx
        mov ecx, 20
        push ecx
        push esi
        call print_anim_1

        mov eax, 0x00569AFD
        jmp eax
    }
}

__declspec(noinline) __declspec(naked)
void patch_click_to_continue()
{
    __asm {
        mov g_string, eax
        mov ecx, 370
        push ecx
        push esi
        call print_anim_2

        mov eax, 0x00568728
        jmp eax
    }
}

void init_patch_multiscore_print()
{
    write_cmd(0x00568C25, format, OP_CALL);
    write_cmd(0x00568C2D, patch_multiscore_print_0, OP_JMP);    // Game: %d
    write_cmd(0x00568DB1, format, OP_CALL);
    write_cmd(0x00568DB9, patch_multiscore_print_1, OP_JMP);    // Time:%02d:%02d:%02d

    write_cmd(0x00568F2F, patch_multiscore_print_2a, OP_JMP);   // Players
    write_cmd(0x00568FFD, patch_multiscore_print_2b, OP_JMP);   // Players

    write_cmd(0x00569154, patch_multiscore_print_3a, OP_JMP);   // Losses
    write_cmd(0x0056922B, patch_multiscore_print_3b, OP_JMP);   // Losses

    write_cmd(0x00569382, patch_multiscore_print_4a, OP_JMP);   // Kills
    write_cmd(0x00569454, patch_multiscore_print_4b, OP_JMP);   // Kills

    write_cmd(0x005695AB, patch_multiscore_print_5a, OP_JMP);   // Economy
    write_cmd(0x0056967D, patch_multiscore_print_5b, OP_JMP);   // Economy

    write_cmd(0x005697D7, patch_multiscore_print_6a, OP_JMP);   // Score
    write_cmd(0x005698AE, patch_multiscore_print_6b, OP_JMP);   // Score

    write_cmd(0x005699BE, patch_player_name_0, OP_JMP);
    write_cmd(0x00569A73, patch_player_name_1, OP_JMP);
    write_cmd(0x004BAB6D, Re_Fetch_String, OP_CALL);    // 翻译电脑玩家的名字
    write_cmd(0x005DE424, Re_Fetch_String, OP_CALL);    // 翻译电脑玩家的名字
    write_cmd(0x005DE2CC, patch_strncpy, OP_CALL);      // 转码人类玩家的名字

    write_cmd(0x0056866B, Re_Fetch_String, OP_CALL);    // 翻译"点击左键继续"
    write_cmd(0x00568670, patch_click_to_continue, OP_JMP);
}