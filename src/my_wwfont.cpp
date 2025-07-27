#include "my_wwfont.h"

MYWWFont::MYWWFont()
	: WWFontClass("myfont"),
	_pFnt(nullptr)
{
}

MYWWFont::~MYWWFont()
{}

int MYWWFont::Char_Pixel_Width(char chr) const
{
	if (!_pFnt) return 10;
	return _pFnt->c(chr).width;
}

int MYWWFont::String_Pixel_Width(const char* u8str) const
{
	if (!_pFnt) return 10;
	auto that = const_cast<MYWWFont*>(this);
	that->Parser(u8str);
	return _pFnt->measure(_last_chrs).Width;
}

void MYWWFont::String_Pixel_Rect(const char* u8str, Rect* pRect) const
{
	if (!_pFnt)
	{
		*pRect = { 0,0,10,10 };
		return;
	}
	auto that = const_cast<MYWWFont*>(this);
	that->Parser(u8str);
	*pRect = _pFnt->measure(_last_chrs);
}

int MYWWFont::Get_Char_Width() const
{
	if (!_pFnt) return 10;
	return _pFnt->maxCharWidth();
}

int MYWWFont::Get_Char_Height() const
{
	if (!_pFnt) return 10;
	return _pFnt->lineHeight();
}

Point2D MYWWFont::Print(const char* u8str, Surface* pSur, Rect* pRect, Point2D* pPt, ConvertClass*, unsigned char[])
{
	if (!_pFnt) 
		return {};
	Parser(u8str);

	int x = pPt->X + pRect->X;
	int y = pPt->Y + pRect->Y;
	_pFnt->draw(_last_chrs, pSur, x, y);
	return { x,y };
}

void MYWWFont::Attach(MyFont* pFnt)
{
	_pFnt = pFnt;
}

MyFont* MYWWFont::get()
{
	return _pFnt;
}

void MYWWFont::Parser(const char* u8str)
{
	if (_last_str.compare(u8str) == 0)
		return;
	std::u32string tmp;
	ck::Text::u8to32(u8str, tmp);
	_last_chrs = _pFnt->cs(tmp);
}

MYWWFont* wwfont_ptr(int style)
{
	static MYWWFont wwfnt;
	wwfnt.Attach(font_ptr(style));
	return &wwfnt;
}
