#include <common/patch.h>
#include <TSpp/src/vqa/vqa.h>
#include "resource.h"
#include "subtitle.h"
#include "hook/hook.h"

static VQAClass* g_vqa;
struct VQAFile
{
	VQAClass* vqa;
	int field_4;
	Surface* pSur;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	Rect rcSrc;
	Rect rcDraw;
	bool field_44;
	bool field_45;
	int16_t field_46;
};

static bool ingame = false;
SubtitleDrawer g_subtitle;

static void __stdcall vqa_load(VQAFile* vqaf)
{
	if (!vqaf) return;
	ingame = false;
	g_vqa = vqaf->vqa;
	g_subtitle.load(g_vqa->Filename);
	g_subtitle.setAlignH(MyFont::Align::AL_HCENTER);
	g_subtitle.setMovieRect(vqaf->rcDraw);
	g_subtitle.setSurface(g_SurPrimary);
}

static void __stdcall vqa_frame(VQAClass* vqa)
{
	if (vqa != g_vqa)
		return;
	auto progress = (vqa->CurrentFrame / (double)vqa->TotalFrames) * 100;
	g_subtitle.draw(progress);
}

struct Ingame
{
	void ingame_vqa_load()
	{
		using Fx = void(Ingame::*)();
		auto fn = ck::whatever_cast<Fx>(0x00564630);
		auto that = (VQAFile*)this;
		(this->*fn)();

		ingame = true;
		g_vqa = that->vqa;
		g_subtitle.load(g_vqa->Filename);
		g_subtitle.setAlignH(MyFont::Align::AL_RIGHT);
		auto rc = g_SurComposite->Get_Rect();
		if (rc.Width > 800)
		{
			rc.Width /= 2;
			rc.X += rc.Width;
		}
		g_subtitle.setMovieRect(rc);
	}

	bool ingame_vqa_frame(bool* a2)
	{
		using Fx = bool(Ingame::*)(bool*);
		auto fn = ck::whatever_cast<Fx>(0x0066BC30);
		auto that = (VQAClass*)this;
		const auto ret = (this->*fn)(a2);
		if (that == g_vqa)
		{
			g_subtitle.setSurface(g_SurLogic);	// g_SurLogic指向的表面会改变, 最好每帧设置
			vqa_frame(that);
		}
		return ret;
	}
};

__declspec(noinline) __declspec(naked)
void patch_vqa_load()
{
	static uintptr_t _ecx, _adr = 0x00563835;

	__asm {
		mov _ecx, ecx
		mov ecx, esi
		push ecx
		call vqa_load

		mov ecx, _ecx
		push 0
		push ecx
		mov  dl, 1

		jmp _adr
	}
}

__declspec(noinline) __declspec(naked)
void patch_vqa_frame()
{
	__asm {
		cmp  eax, ebx
		jz  LOC_RET
		call eax
		push esi
		call vqa_frame
	LOC_RET:
		mov ecx, 0x0066BB0A
		jmp ecx
	LOC_RET_END:
	}
}

void init_patch_movie()
{
	write_cmd(0x00563830, patch_vqa_load, OP_JMP);
	write_cmd(0x0066BB04, patch_vqa_frame, OP_JMP);

	write_cmd(0x00563C2C, &Ingame::ingame_vqa_load, OP_CALL);
	write_cmd(0x005645E5, &Ingame::ingame_vqa_frame, OP_CALL);
}