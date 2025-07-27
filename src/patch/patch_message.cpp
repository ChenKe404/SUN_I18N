#include <point.h>
#include <blit/xsurface.h>
#include <dohook.h>
#include "resource.h"

/*
* ��Ϸ�е�������Ϣ
*/

class ColorScheme;

static Point2D simple_text_print(const char* anstr, XSurface* pSur, Rect* pRect, Point2D* pPt, ColorScheme* a5, unsigned, int nStyle, int)
{
	return MYSimple_Text_Print(to_u32local(anstr), pSur, pRect, pPt, a5, nStyle);
}

static Point2D conquer_clip_text_print(const char* anstr, XSurface* pSur, Rect* pRect, Point2D* pPt, ColorScheme*, unsigned, int nStyle, int, const int*)
{
	return simple_text_print(anstr, pSur, pRect, pPt, 0, 0, nStyle, 0);
}

static Point2D fancy_text_print(const char* ansi, XSurface* pSur, Rect* pRect, Point2D* pPt, ColorScheme* a5, unsigned, int nStyle, ...)
{
	if (!ansi) return { 0,0 };
	static char buffer[64];
	auto u8 = to_u8local(ansi);

	va_list va;
	va_start(va, nStyle);
	vsprintf(buffer, u8.c_str(), va);
	va_end(va);

	std::u32string u32str;
	u8_to_u32(buffer, u32str);
	return MYSimple_Text_Print(u32str, pSur, pRect, pPt, a5, nStyle);
}

void init_patch_message()
{
	write_cmd(0x0064D187, simple_text_print, OP_CALL);	// ��Ϸ�е����Ͻ���Ϣ; ע��:�����Ϣ���и��ǹ̶���
	//write_cmd(0x0064D1E0, conquer_clip_text_print, OP_CALL);	// ��Ϣ����һ����֧, ��ʱע��, ������ϸ��
	write_cmd(0x005ADF94, fancy_text_print, OP_CALL);	// ��������ʾ�ı�
}