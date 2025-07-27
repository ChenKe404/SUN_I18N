#ifndef TSPP_EX_MAPSELECT_H
#define TSPP_EX_MAPSELECT_H

#include <msengine.h>

struct MapStage;
struct MapSelect : public MSEngine
{
	int nX;
	int nY;
	DynamicVectorClass<MapStage*> mStages;
	Rect mRect1;
	const char* field_78;
	int field_7C;
	int field_80;
	int field_84;
	int field_88;
	int field_8C;
	int field_90;
	int field_94;
	int field_98;
	int field_9C;
	int field_A0;
	int field_A4;
	int field_A8;
	ConvertClass* pDrawer1;
	ConvertClass* pDrawer;
	MSFont* pFnt;
	Rect mRect;
	int field_C8;
	int nTime;
	int field_D0;
	int field_D4;
	int field_D8;
	int pPCX;
};

#endif // !TSPP_EX_MAPSELECT_H
