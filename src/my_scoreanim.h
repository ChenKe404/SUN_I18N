#ifndef MY_SCOREANIM_H
#define MY_SCOREANIM_H

#include "tspp_ex/scoreanim.h"

struct MYScoreAnim : public ScoreAnimClass
{
	using Super = ScoreAnimClass;

	MYScoreAnim(
		const MyFont::CharPtrs& chrs,
		MyFont* pFnt,
		int x, int y
	);

	~MYScoreAnim();

	bool Draw(int nFrame) override;
private:
	MyFont::CharPtrs mChrs;
	MyFont* pFnt;
};

#endif // !MY_SCOREANIM_H
