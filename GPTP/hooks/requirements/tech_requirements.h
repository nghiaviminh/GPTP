#pragma once
#include <SCBW/api.h>

namespace hooks {

	s32 parseRequirementOpcodes(CUnit* unit, u32 datReqOffset, u16 id, u32 player, u16* datReqBase); //0x0046D610

	void injectParseRequirementOpcodesWrapper();

}