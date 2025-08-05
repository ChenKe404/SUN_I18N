#ifndef MY_FONT_H
#define MY_FONT_H

#include <lib/rect.h>
#include <blit/dsurface.h>
#include <ckfont/drawer.h>

class FontDrawer;
class MyFont
{
public:
	using Align = ck::FontDrawer::Align;
	using Char = ck::Font::Char;
	using CharPtrs = ck::Font::CharPtrList;
	using Options = ck::FontDrawer::Options;
	using Line = ck::FontDrawer::Line;
	using Lines = ck::FontDrawer::Lines;
public:
	MyFont();
	MyFont(const MyFont&);
	MyFont(MyFont&&) = delete;
	~MyFont();

	MyFont& operator=(const MyFont&);
	MyFont& operator=(MyFont&&) = delete;

	void attach(ck::Font* fnt);

	int lineHeight() const;
	int maxCharWidth() const;
	int charWidth(char32_t) const;

	// 设置绘制选项
	// @return 当前选项
	Options& options();

	const Char& c(char32_t) const;

	// 获得字符串的字符指针数组
	CharPtrs cs(const char32_t* u32str) const;

	inline CharPtrs cs(const std::u32string& u32str) const
	{ return cs(u32str.c_str()); }

	void setMixColor(ck::color);
	ck::color mixColor() const;

	Rect measure(
		const CharPtrs& chrs,
		int maxWidth = -1,
		int maxHeight = -1,
		Lines* out_lines = nullptr
	);

	bool draw(
		CharPtrs::const_iterator begin,
		CharPtrs::const_iterator end,
		Surface* pSur,
		int x, int y,
		int maxWidth = -1,
		int maxHeight = -1,
		Rect* out_rect = nullptr
	);

	inline bool draw(
		const CharPtrs& chrs,
		Surface* pSur,
		int x, int y,
		int maxWidth = -1,
		int maxHeight = -1,
		Rect* out_rect = nullptr
	)
	{
		return draw(chrs.begin(), chrs.end(), pSur, x, y, maxWidth, maxHeight, out_rect);
	}

	bool draw(
		const CharPtrs& chrs,
		const Line& line,
		Surface* pSur,
		int x, int y,
		Rect* out_rect = nullptr
	);
private:
	ck::Font* _fnt;
	FontDrawer* _drawer;
	Options _opts;
};

MyFont* font_ptr(int style);

class ColorScheme;
Point2D MYSimple_Text_Print(const char32_t* u32str, XSurface* pSur, Rect* pRect, Point2D* pPt, ColorScheme* cs, int nStyle);

inline Point2D MYSimple_Text_Print(const std::u32string& u32str, XSurface* pSur, Rect* pRect, Point2D* pPt, ColorScheme* cs, int nStyle)
{ return MYSimple_Text_Print(u32str.c_str(), pSur, pRect, pPt, cs, nStyle); }

#endif // !MY_FONT_H

