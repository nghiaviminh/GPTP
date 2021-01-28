#pragma once
#include <SCBW/api.h>

namespace hooks {

	s32 parseRequirementOpcodes(CUnit* unit, u32 datReqOffset, u16 id, u32 player, u16* datReqBase); //0x0046D610
	s8 UnitCreateAllowed(u16 unitId, CUnit* unit, int playerID);//0046E1C0	

	void injectParseRequirementOpcodesWrapper();
	void injectUnitCreateAllowedWrapper();

}