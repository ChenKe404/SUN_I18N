#ifndef TSPP_EX_DEFINITIONS_H
#define TSPP_EX_DEFINITIONS_H

#include <palette.h>
#include "msfont.h"
#include <wwfont.h>

inline int64_t f64toi64(double v) { return (int64_t)v; }

int GetTime();

// ���ı������������Ӧ����;
// ����Ҫ���д������ַ�\n
int Fit_Text(char* lpText, MSFont* pFnt3, int maxWidth);

// һ����ʾ���ı����ܳ��������ĸ߶�, �������Ҫ��ҳ;
// ��Ҫ��ҳ�����ַ�\n�滻Ϊ\f
// @return �������ҳ��
int More_Page(char* lpText, MSFont* pFnt3, int pageHeight);

Rect* Fancy_Text_Print(
    Rect* pRect, const char* aFormat, Surface* pSur,
    Point2D* pPt, Point2D* pPt1,
    ColorScheme* pCS,
    int a7, TextPrintType tpt, ...
);

// ��ϵͳ�����ϵͳAPI���ƿؼ��ı�
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
