#include <point.h>
#include <blit/xsurface.h>
#include <dohook.h>
#include "resource.h"
#include "re_msglist.h"

/*
* 游戏中的任务消息
*/

class ColorScheme;

static Point2D simple_text_print(const char* utf8, XSurface* pSur, Rect* pRect, Point2D* pPt, ColorScheme* a5, unsigned, int nStyle, int)
{
	std::u32string u32;
	to_u32str(u32, utf8);
	return MYSimple_Text_Print(u32, pSur, pRect, pPt, a5, nStyle);
}

static Point2D conquer_clip_text_print(const char* utf8, XSurface* pSur, Rect* pRect, Point2D* pPt, ColorScheme*, unsigned, int nStyle, int, const int*)
{
	return simple_text_print(utf8, pSur, pRect, pPt, 0, 0, nStyle, 0);
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
	Re_MessageList::H_Init.rehook();
	Re_MessageList::H_Add_Message.rehook();
	// 遭遇战文本
	write_cmd(0x004BF5FF, Re_Fetch_String, OP_CALL);	// 被击败
	write_cmd(0x004BF682, Re_Fetch_String, OP_CALL);	// 被击败
	write_cmd(0x004BDDE5, Re_Fetch_String, OP_CALL);	// 结盟
	write_cmd(0x004BE016, Re_Fetch_String, OP_CALL);	// 宣战

	write_cmd(0x0064D187, simple_text_print, OP_CALL);	// 游戏中的左上角消息
	write_cmd(0x0064D1E0, conquer_clip_text_print, OP_CALL);	// 消息的另一个分支, 暂时注释, 遇到再细看
	write_cmd(0x005ADF94, fancy_text_print, OP_CALL);	// 进度条提示文本
}