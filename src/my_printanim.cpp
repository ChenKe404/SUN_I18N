#include "my_printanim.h"
#include <dsaudio.h>
#include <scenario.h>
#include <tibsun/tibsun_globals.h>

////////////////////////////////////////////////
//// MYPrintAnim
MYPrintAnim::MYPrintAnim(
	const MyFont::CharPtrs* pChrs,
    MyFont* pFnt,
    int x, int y
) : pChrs(pChrs),
	pFnt(pFnt),
	nCurChar(0)
{
    nX = x;
    nY = y;
	bPlaying = true;
	nTime = GetTime();
	nInterval = 4;
}

MYPrintAnim::~MYPrintAnim()
{
#ifdef _DEBUG
    __asm { sub esp, 4 }		// 栈平衡
#endif // !_DEBUG
}

bool MYPrintAnim::Set_Playing(bool playing)
{
    bPlaying = playing;
    return bPlaying;
}

bool MYPrintAnim::Draw_Frame(XSurface* pSur, Rect* pRect)
{
    if (nTime == -1)
        nTime = GetTime();

    if ((GetTime() - nTime) < nInterval)    // 超过帧时间间隔才绘制一帧
        return false;

    if (!bPlaying || pChrs->empty())
    {
        *pRect = { 0,0,0,0 };
        return true;
    }

    nTime = GetTime();

    ++nCurChar;
	pFnt->draw(pChrs->begin(), pChrs->begin() + nCurChar, pSur, nX, nY, -1, -1, &mRect);
	*pRect = mRect;

    auto pAud = (const void*)g_pAudText1A[2 * (rand() % 3)];
    if (pAud)
    {
        auto v = f64toi64(g_FltAudSpeed * 64);
        Audio.Play_Sample(pAud, 10, v);
    }

    return nCurChar >= pChrs->size();
}

int MYPrintAnim::Refresh(XSurface*, const Rect*)
{
	return 0;
}

Rect* MYPrintAnim::Get_Rect(Rect* pRect)
{
    *pRect = mRect;
    return pRect;
}

bool MYPrintAnim::func7()
{
	return false;
}

////////////////////////////////////////////////
//// MYPagedPrintAnim
MYPagedPrintAnim::MYPagedPrintAnim(
    const MyFont::CharPtrs* pChrs,
    const MyFont::Lines* pLines,
    MyFont* pFnt,
    const Rect& rect
) : MYPrintAnim(pChrs,pFnt, rect.X, rect.Y),
    pLines(pLines),
    nOY(0),
    nCurLine(0),
    nPerPage(0)
{
    nPerPage = rect.Height / (pFnt->lineHeight() + pFnt->options().spacingY);
    mRect = rect;
}

MYPagedPrintAnim::~MYPagedPrintAnim()
{
    int aa = 0;
}

bool MYPagedPrintAnim::Draw_Frame(XSurface* pSur, Rect* pRect)
{
    if (nTime == -1)
        nTime = GetTime();

    if ((GetTime() - nTime) < nInterval)    // 超过帧时间间隔才绘制一帧
        return false;

    if (!bPlaying || pChrs->empty() || pLines->empty())
    {
        *pRect = { 0,0,0,0 };
        return true;
    }

    nTime = GetTime();

    MyFont::Line line = pLines->at(nCurLine);
    int right = line.right;
    line.right = line.left + nCurChar + 1;
    pFnt->draw(*pChrs, line, pSur, nX, nY + nOY, pRect);

    ++nCurChar;

    if (line.right >= right)    // 行绘制结束
    {
        ++nCurLine;
        nCurChar = 0;

        if (nCurLine >= nPerPage && nCurLine < pLines->size())
        {
            nOY -= pFnt->lineHeight() + pFnt->options().spacingY;

            pSur->Copy_From(mRect, *g_SurAlternate, mRect);
            int i = nCurLine - nPerPage;
            if (i >= 0)
            {
                ++i;
                for (;i < nCurLine;++i)
                {
                    pFnt->draw(*pChrs, pLines->at(i), pSur, nX, nY + nOY);
                }
                *pRect = mRect;
            }
        }
    }

    return nCurLine >= pLines->size();
}
