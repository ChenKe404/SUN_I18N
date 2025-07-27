#include "my_printanim.h"
#include <tspp_ex/mapselect.h>
#include "dohook.h"

static MyFont::CharPtrs g_chrs;
static MyFont::Lines g_lines;

static MYPagedPrintAnim* __stdcall create_anim(MapSelect* ms, const char* ansi, Rect* pRect)
{
	auto fnt = Re_MSFont::fetch(ms->pFnt);
	g_chrs = fnt->cs(to_u32local(ansi));
	g_lines.clear();
	fnt->measure(g_chrs, pRect->Width, -1, &g_lines);
	auto anim = new MYPagedPrintAnim(&g_chrs, &g_lines, fnt, *pRect);
	anim->nInterval = 4;
	ms->Attach_Anim(anim);
	return anim;
}

static void __stdcall delete_anim(MSAnim* ani)
{
	if (ani) return;
	ani->Set_Playing(false);
	delete ani;
}

__declspec(noinline) __declspec(naked)
void patch_create_anim()
{
	__asm {
		push edi			// rect
		mov ecx, [ebx+0x10]	// text
		push ecx
		push esi
		call create_anim
		mov [esp+0x10], eax

		mov eax, 0x00554416
		jmp eax
	}
}

static int __fastcall patch_fit_text(char* lpText, MSFont* pFnt3, int maxWidth)
{
	return maxWidth;	// 什么也不做, 避免更改原文内容
}

void init_patch_mapselect()
{
	write_cmd(0x005543C9, patch_create_anim, OP_JMP);
	write_cmd(0x00553F0C, patch_fit_text, OP_CALL);
}