#include <colorscheme.h>
#include <textprint.h>
#include <dohook.h>
#include <lib/fetchres.h>
#include "tspp_ex/definitions.h"

static Point2D __stdcall fancy_text_print_0(int uID, XSurface* pSur, Rect* pRect, Point2D* pPt, ColorScheme* a5, unsigned a6, int style, ...)
{
	using AL = MyFont::Align;
	static char buffer[256];
	static std::u32string u32str;

	va_list va;
	va_start(va, style);
	auto u8str = Re_Fetch_String(uID);
	vsprintf(buffer, u8str, va);
	va_end(va);

	to_u32str(buffer, u32str);
	int x = pRect->X + pPt->X;
	int y = pRect->Y + pPt->Y;

	auto fnt = font_ptr(style);

	auto& opts = fnt->options();
	auto old = opts.align;
	opts.align = AL::AL_HCENTER;

	auto chrs = fnt->cs(u32str);
	auto old_mix = fnt->mixColor();
	fnt->setMixColor(to_color(a5->HSV));
	fnt->draw(chrs, pSur, x, y, -1, -1);
	fnt->setMixColor(old_mix);
	opts.align = old;

	return { x,y };
}

static const char* __stdcall modify_mission_save_name(const char* src)
{
	static std::string ret;
	auto u8 = g_text.u8(src);
	if (u8 == src)
		return src;
	if (u8_to_ansi(u8, ret) < 1)
		return src;
	return ret.c_str();
}

static int __stdcall get_window_text(HWND hWnd, LPSTR lpBuffer, int nMaxCount)
{
	GetWindowTextA(hWnd, lpBuffer, nMaxCount);
	auto u8 = to_u8local(lpBuffer);
	std::string str;
	u8_to_ansi(u8.c_str(), str);
	int sz = std::min((int)str.size(), nMaxCount - 1);
	memcpy(lpBuffer, str.c_str(), sz);
	lpBuffer[sz] = '\0';
	return sz;
}

static LRESULT __stdcall send_message(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	auto text = (const char*)lParam;
	auto u8 = g_text.u8(text);
	std::string ansi;
	if (u8 != text && u8_to_ansi(u8, ansi) > 0)
		lParam = (LPARAM)ansi.c_str();
	return SendMessageA(hWnd, Msg, wParam, lParam);
}

static const char* fetch_string_ansi(UINT uID) {
	auto str = Fetch_String(uID);
	auto u8 = g_text.u8(str);
	if (u8 == str) return str;
	static std::string ansi;
	if (u8_to_ansi(u8, ansi) < 1)
		return str;
	return ansi.c_str();
}

///////////////////////////////////////
// fetch_string_ansi_hook
const char* fetch_string_ansi_hook(UINT uID);
static ck::Hook <
	0x00472350,
	fetch_string_ansi_hook,
	false
> h_fetch_string_ansi;

static const char* fetch_string_ansi_hook(UINT uID) {
	auto str = h_fetch_string_ansi(uID);
	auto u8 = g_text.u8(str);
	if (u8 == str) return str;
	static std::string ansi;
	if (u8_to_ansi(u8, ansi) < 1)
		return str;
	return ansi.c_str();
}

static void fetch_string_ansi_hook_on() {
	h_fetch_string_ansi.rehook();
}

static void fetch_string_ansi_hook_off() {
	h_fetch_string_ansi.unhook();
}

///////////////////////////////////////
// fetch_string_u8_hook
const char* fetch_string_u8_hook(UINT uID);
static ck::Hook <
	0x00472350,
	fetch_string_u8_hook,
	false
> h_fetch_string_u8;

static const char* fetch_string_u8_hook(UINT uID) {
	return g_text.u8(h_fetch_string_u8(uID));
}

static void __fastcall fetch_string_u8_hook_on() {
	h_fetch_string_u8.rehook();
}

static void __fastcall fetch_string_u8_hook_off() {
	h_fetch_string_u8.unhook();
}

static int __cdecl patch_sprintf(char* buffer,const char* fmt, const char* str1, const char* str2)
{
	std::string u8s1 = to_u8local(str1);
	str1 = u8s1.c_str();
	std::string u8s2;
	if (ansi_to_u8(str2, u8s2) > 0)
		str2 = u8s2.c_str();

	return sprintf(buffer, fmt, str1, str2);
}

static const char* patch_empty_slot(UINT uID)
{
	auto u8 = Re_Fetch_String(uID);
	static std::string ansi;
	u8_to_ansi(u8, ansi);
	return ansi.c_str();
}

LONG __fastcall patch_tooltip(
	COLORREF color,
	void* a2,
	Rect* pRect,
	LPCSTR lpString,
	int length,
	int align_h,
	int align_v,
	XSurface* a8
)
{
	std::string str;
	auto u8 = g_text.u8(lpString);
	if (u8 != lpString)
	{
		u8_to_ansi(u8, str);
		lpString = str.c_str();
		length = (int)str.size();
	}
	return Ctrl_Draw_Text(color, a2, pRect, lpString, length, align_h, align_v, a8);
}

static BOOL __stdcall text_out(HDC hdc, int x, int y, LPCSTR lpString, int c)
{
	std::wstring wstr;
	if (u8_to_wide(lpString, wstr) > 0)
		return TextOutW(hdc, x, y, wstr.c_str(), (int)wstr.size());
	return TextOutA(hdc, x, y, lpString, c);
}

__declspec(noinline) __declspec(naked)
void patch_mission_save_name()
{
	__asm {
		mov edi, ecx
		mov dword ptr[edi + 4], 2

		push eax
		call modify_mission_save_name
		mov[edi + 0Ch], eax
		mov ecx, edi

		mov eax, 0x0050488E
		jmp eax
	}
}

__declspec(noinline) __declspec(naked)
void patch_send_message_0()
{
	__asm {
		push    esi
		push    0
		push    0x180
		push    ebx
		call send_message
		mov ecx, 0x004E33DC
		jmp ecx
	}
}

__declspec(noinline) __declspec(naked)
void patch_group_box_text()
{
	__asm {
		call get_window_text

		mov eax, 0x0059C549
		jmp eax
	}
}

__declspec(noinline) __declspec(naked)
void patch_cmd_name_hook()
{
	static void *_eax, *_edi;
	__asm {
		mov _eax, eax
		mov _edi, edi
		call fetch_string_ansi_hook_on	// 开启hook
		mov ecx, _edi
		mov eax, _eax
		call dword ptr[eax + 8]			// fetch
		mov _eax, eax
		call fetch_string_ansi_hook_off	// 关闭hook
		mov eax, _eax

		mov ecx, 0x0058A76F
		jmp ecx
	}
}

__declspec(noinline) __declspec(naked)
void patch_cmd_desc_hook()
{
	static void *_eax, *_ecx, *_edx;
	__asm {
		mov	ebp, eax
		mov _ecx, ecx
		mov _edx, edx
		call fetch_string_u8_hook_on	// 开启hook
		mov ecx, _ecx
		mov edx, _edx
		call dword ptr[edx + 0x10]		// fetch
		mov _eax, eax
		call fetch_string_u8_hook_off	// 关闭hook
		mov eax, _eax

		mov ecx, 0x0058ACB5
		jmp ecx
	}
}

__declspec(noinline) __declspec(naked)
void patch_tactical_text()
{
	__asm {
		call text_out
		mov eax, 0x00611D8D
		jmp eax
	}
}

void init_patch_misc_text()
{
	write_cmd(0x0060E5DC, fancy_text_print_0, OP_CALL);		// 左上角选项按钮:  Options
	write_cmd(0x0060E839, fancy_text_print_0, OP_CALL);		// 右上角时间
	write_cmd(0x0060E896, fancy_text_print_0, OP_CALL);		// 右上角时间
	write_cmd(0x00504884, patch_mission_save_name, OP_JMP);	// 当"保存游戏"窗口弹出时, 翻译文本框内的任务名称
	write_cmd(0x005053CE, patch_empty_slot, OP_CALL);		// 保存游戏"空槽位"
	write_cmd(0x00504EC9, patch_sprintf, OP_CALL);			// 删除任务提示中的文本
	write_cmd(0x004E33D1, patch_send_message_0, OP_JMP);	// 战役列表
	write_cmd(0x00575F57, fetch_string_ansi, OP_CALL);		// 局域网游戏"大厅"
	write_cmd(0x0059C543, patch_group_box_text, OP_JMP);	// GroupBox 文本
	/* 键盘命令 */
	write_cmd(0x0058A76A, patch_cmd_name_hook, OP_JMP);		// 命令名称 hook
	write_cmd(0x0058ACB0, patch_cmd_desc_hook, OP_JMP);		// 命令描述 hook

	write_cmd(0x005921A0, patch_tooltip, OP_CALL);			// ui工具提示
	write_cmd(0x00611C15, Re_Fetch_String, OP_CALL);		// 任务完成/胜利/失败 翻译
	write_cmd(0x00611D87, patch_tactical_text, OP_JMP);		// 任务完成/胜利/失败 绘制
}