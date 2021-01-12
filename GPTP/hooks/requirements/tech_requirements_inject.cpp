#include "tech_requirements.h"

#include <hook_tools.h>
#include <DebugUtils.h>

namespace {
	void __declspec(naked) injectParseRequirementOpcodesWrapper() //0x0046D610
	{
		static s32 result;
		static s32 datReqOffset;
		static s16 techID;
		static CUnit* unit;
		static s32 playerId;
		//__asm {

		//}
		result = hooks::parseRequirementOpcodes(unit, datReqOffset, techID, playerId);
		DebugOut("parseRequirementOpcodes returned %d for: techID %d, unit %d, playerID %d\n\n\n", result, techID, unit->id, playerId);
		//__asm {

		//}
	}
}

namespace hooks {
	void injectParseRequirementOpcodesWrapper() {
		//jmpPatch(parseRequirementOpcodes, 0x0046D610, 0);
	}
}