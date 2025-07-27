#ifndef TSPP_EX_SCORESCREEN_H
#define TSPP_EX_SCORESCREEN_H

#include <lib/vector.h>
#include <blit/dsurface.h>
#include "scoreanim.h"
#include "sfxentry.h"
#include "scorefont.h"

struct ScoreScreen
{
	void Start_Score_Screen();
	int Play_Anim(int num);

	int nOX;
	int nOY;
	DSurface* pSur;
	ScoreFullFontClass* pFullFnt;
	ScoreBigFontClass* pBigFnt;
	ConvertClass* pDrawer;
	int field_18;
	DynamicVectorClass<ScoreAnimClass*> mAnims;
	DynamicVectorClass<SfxEntry*> mSfx;
};

#endif  // !TSPP_EX_SCORESCREEN_H

