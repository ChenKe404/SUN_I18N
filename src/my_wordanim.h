#ifndef MY_WORDANIM_H
#define MY_WORDANIM_H

#include "tspp_ex/anim.h"

struct MYWordAnim : public MSAnim
{
    MYWordAnim(
        MyFont* pFnt,
        const MyFont::CharPtrs* pChrs,
        const MyFont::Lines* pLines,
        const Rect& rect
	);

    ~MYWordAnim();

    bool Draw_Frame(XSurface*, Rect*) override;

    int Refresh(XSurface*, const Rect*) override;

    Rect* Get_Rect(Rect* pRect) override;

    bool func7() override;

private:
    MyFont* pFnt;
    const MyFont::CharPtrs* pChrs;
    const MyFont::Lines* pLines;
    
    int nLine;           // 当前行
    Rect mRectBound;
};

#endif // MY_WORDANIM_H
