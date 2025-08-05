#include "my_scoreanim.h"
#include "tspp_ex/definitions.h"
#include <dsaudio.h>
#include <tibsun/tibsun_globals.h>

MYScoreAnim::MYScoreAnim(
	const MyFont::CharPtrs& chrs,
	MyFont* pFnt,
	int x, int y
) : mChrs(chrs),
	pFnt(pFnt)
{
	nX = x;
	nY = y;
	nTime = GetTime();
	nInterval = 4;
	aText = nullptr;
	nCurFrame = 0;
	nFrame = (int)chrs.size();
	bFlag = 0;
	pFullFnt = nullptr;
}

MYScoreAnim::~MYScoreAnim()
{
#ifdef _DEBUG
	__asm { sub esp,4 }		// 栈平衡
#endif // !_DEBUG
}

bool MYScoreAnim::Draw(int nFrame)
{
	if (nTime == -1)
		nTime = GetTime();

	if ((GetTime() - nTime) < nInterval)    // 超过帧时间间隔才绘制一帧
		return false;

	nTime = GetTime();

	++nCurFrame;
	pFnt->draw(mChrs.begin(), mChrs.begin() + nCurFrame, g_SurHidden, nX, nY, -1, -1);
	auto pAud = (const void*)g_pAudText1B[2 * (rand() % 3)];
	if (pAud)
	{
		auto v = f64toi64(g_FltAudSpeed * 128.0);
		Audio.Play_Sample(pAud, 255, v);
	}
	
	return nCurFrame >= mChrs.size();
}
