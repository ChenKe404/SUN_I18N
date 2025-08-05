#ifndef MY_WWFONT_H
#define MY_WWFONT_H

#include <wwfont.h>
#include "my_font.h"

class MYWWFont : public WWFontClass
{
public:
	MYWWFont();
	virtual ~MYWWFont();

	int Char_Pixel_Width(char chr) const override;
	int String_Pixel_Width(const char* u8str) const override;
	void String_Pixel_Rect(const char* u8str, Rect* pRect) const override;

	int Get_Char_Width() const override;
	int Get_Char_Height() const override;
	Point2D Print(const char* u8str, Surface* pSur, Rect* pRec, Point2D* pPt, ConvertClass* = nullptr, unsigned char[] = nullptr) override;

	void Attach(MyFont*);
	MyFont* get();
private:
	void Parser(const char*);
private:
	MyFont* _pFnt;
	MyFont::CharPtrs _last_chrs;	// 上次的文本的字符列表
	std::string _last_str;			// 上次的文本
};

struct MYWWFont;
MYWWFont* wwfont_ptr(int style);

#endif // !MY_WWFONT_H
