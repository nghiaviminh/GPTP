#include "tech_requirements.h"

#include <hook_tools.h>
#include <DebugUtils.h>

namespace {
	void __declspec(naked) getParseRequirementOpcodesWrapper() //0x0046D610
	{
		static CUnit* unit;
		static u32 datReqOffset;
		static u32 techID;
		static u32 playerId;
		static u16* datReqBase;
		static s32 result;

		__asm {
			push ebp
			mov ebp, esp
			mov datReqOffset, eax
			mov unit, esi
			mov eax, [ebp + 8]
			mov techID, eax
			mov eax, [ebp + 12]
			mov playerId, eax
			mov eax, [ebp + 16]
			mov datReqBase, eax
			pushad
		}
		result = hooks::parseRequirementOpcodes(unit, datReqOffset, techID, playerId, datReqBase);
		DebugOut("parseRequirementOpcodes returned %d for: techID %d, unit %d, playerID %d (datReqOffset=%d, datReqBase=%p)\n\n\n", result, techID, unit->id, playerId, datReqOffset, datReqBase);
		__asm {
			popad
			mov eax, result
			pop ebp
			retn 12
		}
	}
}

namespace hooks {
	s32 parseRequirementOpcodes(CUnit* unit, u32 datReqOffset, u16 techID, u32 player)
	{
		return s32();
	}
	void injectParseRequirementOpcodesWrapper() {
		jmpPatch(getParseRequirementOpcodesWrapper, 0x0046D610, 0);
	}
}