#ifndef TSPP_EX_DEFINITIONS_H
#define TSPP_EX_DEFINITIONS_H

#include <palette.h>
#include "msfont.h"
#include <wwfont.h>

inline int64_t f64toi64(double v) { return (int64_t)v; }

int GetTime();

// 将文本按照最大宽度适应换行;
// 在需要换行处插入字符\n
int Fit_Text(char* lpText, MSFont* pFnt3, int maxWidth);

// 一次显示的文本不能超过背景的高度, 否则就需要翻页;
// 将要换页处的字符\n替换为\f
// @return 多出来的页数
int More_Page(char* lpText, MSFont* pFnt3, int pageHeight);

Rect* Fancy_Text_Print(
    Rect* pRect, const char* aFormat, Surface* pSur,
    Point2D* pPt, Point2D* pPt1,
    ColorScheme* pCS,
    int a7, TextPrintType tpt, ...
);

// 用系统字体和系统API绘制控件文本
LONG Ctrl_Draw_Text(
    COLORREF color,
    void* a2,
    Rect* pRect,
    LPCSTR lpString,
    int length,
    int align_h,
    int align_v,
    XSurface* a8
);

template<typename C>
inline int strlen(const C* lpText) 
{ return std::char_traits<C>::length(lpText); }

#endif // ! TSPP_EX_DEFINITIONS_H
