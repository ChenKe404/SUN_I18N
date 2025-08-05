#include <wwfont.h>
#include "my_wwfont.h"
#include "dohook.h"

/*
* 游戏中的工具提示
*/

static WWFontClass* __fastcall font_ptr_0(int style)
{
	return wwfont_ptr(style);
}

static Point2D __stdcall fancy_text_print_0(const char* u8str, XSurface* pSur, Rect* pRect, Point2D* pPt, ColorScheme* a5, unsigned a6, int nStyle, ...)
{
	Point2D pt{ 0,0 };
	if (!u8str)
		return pt;

	using AL = MyFont::Align;

	auto wwfnt = wwfont_ptr(nStyle);
	auto fnt = wwfnt->get();

	auto old = fnt->mixColor();
	fnt->setMixColor(to_color(a5->HSV));
	pt = wwfnt->Print(u8str,pSur,pRect,pPt);
	fnt->setMixColor(old);
	return pt;
}

static Point2D plain_text_print_0(char const* u8str, XSurface* pSur, Rect* pRect, Point2D* pPt, int a5, unsigned, int nStyle, ColorSchemeType cst, int)
{
	if (pPt->Y < pRect->Y)	// 避免画到顶部功能条上, 顶部功能条不是每次都重绘
		return { 0,0 };
	static std::u32string u32str;
	to_u32str(u8str, u32str);
	return MYSimple_Text_Print(u32str, pSur, pRect, pPt, g_ArrCS[cst], nStyle);
}

static Point2D plain_text_print_1(int uID, XSurface* pSur, Rect* pRect, Point2D* pPt, int a5, unsigned, int nStyle, ColorSchemeType cst, int)
{
	return plain_text_print_0(Re_Fetch_String(uID), pSur, pRect, pPt, a5, 0, nStyle, COLORSCHEME_NONE, 0);
}

void init_patch_tooltip()
{
	write_cmd(0x0044E3D2, font_ptr_0, OP_CALL);			// 返回字体
	write_cmd(0x0044E70D, fancy_text_print_0, OP_CALL);	// 打印

	write_cmd(0x00637DBE, Re_Fetch_String, OP_CALL);	// 选中电厂时的电源文本 翻译
	write_cmd(0x00637DEF, plain_text_print_0, OP_CALL);	// 选中电厂时的电源文本
	
	write_cmd(0x00637E40, plain_text_print_1, OP_CALL);	// 将生产建筑设为主要建筑
	/* 四个功能按钮 */
	write_cmd(0x00647BB5, Re_Fetch_String, OP_CALL);	// 价格
	/* SidebarClass::Help_Text */
	write_cmd(0x005F4EBB, Re_Fetch_String, OP_CALL);	// 价格
	write_cmd(0x005F4EE7, Re_Fetch_String, OP_CALL);	// 价格+科技名称
	/* PowerClass::Help_Text */
	write_cmd(0x005ABD73, Re_Fetch_String, OP_CALL);	// 电力/负载
	/* DisplayClass::Help_Text */
	write_cmd(0x0047AFCC, Re_Fetch_String, OP_CALL);	// 未探索之地形
	write_cmd(0x0047B0FC, Re_Fetch_String, OP_CALL);	// 敌方单位
	write_cmd(0x0047B111, Re_Fetch_String, OP_CALL);	// 敌方建筑
	write_cmd(0x0047B0E7, Re_Fetch_String, OP_CALL);	// 敌方士兵
}