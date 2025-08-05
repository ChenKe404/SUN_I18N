#include <wwfont.h>
#include "my_wwfont.h"
#include "dohook.h"

/*
* ��Ϸ�еĹ�����ʾ
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
	if (pPt->Y < pRect->Y)	// ���⻭��������������, ��������������ÿ�ζ��ػ�
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
	write_cmd(0x0044E3D2, font_ptr_0, OP_CALL);			// ��������
	write_cmd(0x0044E70D, fancy_text_print_0, OP_CALL);	// ��ӡ

	write_cmd(0x00637DBE, Re_Fetch_String, OP_CALL);	// ѡ�е糧ʱ�ĵ�Դ�ı� ����
	write_cmd(0x00637DEF, plain_text_print_0, OP_CALL);	// ѡ�е糧ʱ�ĵ�Դ�ı�
	
	write_cmd(0x00637E40, plain_text_print_1, OP_CALL);	// ������������Ϊ��Ҫ����
	/* �ĸ����ܰ�ť */
	write_cmd(0x00647BB5, Re_Fetch_String, OP_CALL);	// �۸�
	/* SidebarClass::Help_Text */
	write_cmd(0x005F4EBB, Re_Fetch_String, OP_CALL);	// �۸�
	write_cmd(0x005F4EE7, Re_Fetch_String, OP_CALL);	// �۸�+�Ƽ�����
	/* PowerClass::Help_Text */
	write_cmd(0x005ABD73, Re_Fetch_String, OP_CALL);	// ����/����
	/* DisplayClass::Help_Text */
	write_cmd(0x0047AFCC, Re_Fetch_String, OP_CALL);	// δ̽��֮����
	write_cmd(0x0047B0FC, Re_Fetch_String, OP_CALL);	// �з���λ
	write_cmd(0x0047B111, Re_Fetch_String, OP_CALL);	// �з�����
	write_cmd(0x0047B0E7, Re_Fetch_String, OP_CALL);	// �з�ʿ��
}