#include "my_wordanim.h"
#include <dsaudio.h>

MYWordAnim::MYWordAnim(
    MyFont* pFnt,
    const MyFont::CharPtrs* pChrs,
    const MyFont::Lines* pLines,
    const Rect& rect
) : pFnt(pFnt),
    pChrs(pChrs), pLines(pLines),
    nLine(0),
    mRectBound(rect)
{
    bPlaying = true;
    nTime = GetTime();
    nInterval = 4;
}

MYWordAnim::~MYWordAnim()
{
#ifdef _DEBUG
    __asm { sub esp, 4 }		// 栈平衡
#endif // _DEBUG
}

bool MYWordAnim::Draw_Frame(XSurface* pSur, Rect* pRect)
{
    if (nTime == -1)
        nTime = GetTime();

	if ((GetTime() - nTime) < nInterval)    // 超过帧时间间隔才绘制一帧
		return false;

    if (!bPlaying || pLines->empty())
    {
        *pRect = { 0,0,0,0 };
        return true;
    }

    nTime = GetTime();

    const auto& line = pLines->at(nLine);

	*pRect = mRectBound;

	pFnt->draw(*pChrs, line, pSur, mRectBound.X, mRectBound.Y, pRect);
	nLine++;

    auto aud = (const void*)MixFileClass::Retrieve(g_aAudBleep1);
    if (aud && line.width > 0)
    {
        auto v = f64toi64(g_FltAudSpeed * 64.0);
        Audio.Play_Sample(aud, 10, v);
    }

    return nLine >= pLines->size();
}

int MYWordAnim::Refresh(XSurface* pSur, const Rect* pRect)
{
    return 0;
}

Rect* MYWordAnim::Get_Rect(Rect* pRect)
{
    *pRect = mRectBound;
    return pRect;
}

bool MYWordAnim::func7()
{
    return false;
}
