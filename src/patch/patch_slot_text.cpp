#include "dohook.h"

/*
* 科技槽科技名称文本
*/

static void draw_slot_text(char* u8str, Point2D* pPt, Rect* pRect, int nSlotWidth)
{
	if (!g_bSlotText)
		return;
	int x = pRect->X + pPt->X;
	int y = pRect->Y + pPt->Y + 7;	// 绘制槽文本默认有一行的高度(8), 这里直接到槽的底部
	std::u32string u32str;
	to_u32str(u8str, u32str);
	auto chrs = g_fnt12o.cs(u32str);

	auto cs = ColorScheme::As_Pointer("LightGrey", 1);

	auto& opts = g_fnt12o.options();
	auto old = opts.spacingX;
	opts.spacingX = 1;
	auto box = g_fnt12o.measure(chrs, nSlotWidth, -1);
	auto old_mix = g_fnt12o.mixColor();
	g_fnt12o.setMixColor(to_color(cs->HSV));
	g_fnt12o.draw(chrs, g_SurSidebar, x, y - box.Height, nSlotWidth);
	g_fnt12o.setMixColor(old_mix);
	opts.spacingX = old;
}

class ColorScheme;
static Point2D draw_ready(const char* ansi, XSurface* pSur, Rect* pRect, Point2D* pPt, ColorScheme* a5, unsigned, int nStyle, ...)
{
	using AL = MyFont::Align;

	auto u32str = to_u32local(ansi);
	auto fnt = font_ptr(nStyle);
	auto chrs = fnt->cs(u32str);
	int x = pRect->X + pPt->X;
	int y = pRect->Y + pPt->Y;

	auto& opt = fnt->options();
	auto old = opt.align;
	if (nStyle & TPF_CENTER)
		opt.align = AL::AL_HCENTER;
	else if (nStyle & TPF_RIGHT)
		opt.align = AL::AL_RIGHT;
	else
		opt.align = AL::AL_LEFT;

	auto old_mix = fnt->mixColor();
	fnt->setMixColor(to_color(a5->HSV));
	fnt->draw(chrs, pSur, x, y);
	fnt->setMixColor(old_mix);

	opt.align = old;
	return { 0,0 };
}

static Point2D draw_onhold(int uID, XSurface* pSur, Rect* pRect, Point2D* pPt, ColorScheme* a5, unsigned, int nStyle, ...)
{
	auto u8str = H_Fetch_String(uID);
	return draw_ready(u8str, pSur, pRect, pPt, a5, 0, nStyle);
}

void init_patch_slot_text()
{
	write_cmd(0x005F5361, draw_slot_text, OP_CALL);
	write_cmd(0x005F5483, draw_ready, OP_CALL);
	write_cmd(0x005F55CD, draw_onhold, OP_CALL);
	/* 超武就绪 */
	write_cmd(0x0060BF17, Re_Fetch_String, OP_CALL);	// Release
	write_cmd(0x0060BF25, Re_Fetch_String, OP_CALL);	// Ready
	write_cmd(0x0060BEF9, Re_Fetch_String, OP_CALL);	// Charging...
	write_cmd(0x0060BF25, Re_Fetch_String, OP_CALL);	// Ready
	write_cmd(0x0060BEEA, Re_Fetch_String, OP_CALL);	// Activated
}