#ifndef RE_RESTATE_MISSION_H
#define RE_RESTATE_MISSION_H

#include "tspp_ex/restatemission.h"

struct RE_RestateMission
{
    using Self = RE_RestateMission;

    const char* Draw_Text(Surface*);
    DEF_HOOKMEM(0x005C0EC0, &Self::Draw_Text, false, H_Draw_Text)

    bool Start_Briefing_Screen(ScenarioClass* pSce);
    DEF_HOOKMEM(0x005c0440, &Self::Start_Briefing_Screen, false, H_Start_Briefing_Screen)
};


#endif // ! RE_RESTATE_MISSION_H
