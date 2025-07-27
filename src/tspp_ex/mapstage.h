#ifndef TSPP_EX_MAPSTAGE_H
#define TSPP_EX_MAPSTAGE_H

#include <lib/vector.h>

struct MapStage {};
struct MSAnimEntry {};
struct MSSfxEntry {};
struct MapStageList
{
	DynamicVectorClass<MapStage*> mStages;
	DynamicVectorClass<MSAnimEntry*> mAnims;
	DynamicVectorClass<MSSfxEntry*> mSfx;
};

#endif // !TSPP_EX_MAPSTAGE_H
