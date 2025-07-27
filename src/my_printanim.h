#ifndef MY_PRINTANIM_H
#define MY_PRINTANIM_H

#include "tspp_ex/anim.h"

struct MYPrintAnim : public MSAnim
{
    MYPrintAnim(
        const MyFont::CharPtrs* pChrs,
        MyFont* pFnt,
        int x, int y
    );

    ~MYPrintAnim();

    bool Set_Playing(bool) override;

    bool Draw_Frame(XSurface*, Rect*) override;

    int Refresh(XSurface*, const Rect*) override;

    Rect* Get_Rect(Rect* pRect) override;

    bool func7() override;

protected:
    const MyFont::CharPtrs* pChrs;
    MyFont* pFnt;
    int nCurChar;           // 当前字符
    Rect mRect;
};

struct MYPagedPrintAnim : public MYPrintAnim
{
    MYPagedPrintAnim(
        const MyFont::CharPtrs* pChrs,
        const MyFont::Lines* pLines,
        MyFont* pFnt,
        const Rect& rect
    );

    ~MYPagedPrintAnim();

    bool Draw_Frame(XSurface*, Rect*) override;
protected:
    const MyFont::Lines *pLines;
    
    int nOY;
    int nCurLine;       // 当前页第一行的索引
    int nPerPage;       // 每页行数
};

#endif // !MY_PRINTANIM_H
