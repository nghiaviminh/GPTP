#include "unit_requirements.h"
#include <hook_tools.h>
#include <DebugUtils.h>

namespace {
	void __declspec(naked) getUnitCreateAllowedWrapper() //0x0046E1C0
	{
		static s32 result;
		static s16 unitId;
		static CUnit* unit;
		static s32 playerId;
		__asm {
			PUSH EBP
			MOV EBP, ESP
			PUSH EAX
			MOV EAX, [EBP + 0x08]
			MOV playerId, EAX
			POP EAX
			MOV unitId, AX
			MOV unit, ESI
			PUSHAD
		}
		result = hooks::UnitCreateAllowed(unitId, unit, playerId);
		DebugOut("UnitCreateAllowedLogic returned %d for: untitID %d, creatingUnit %d, playerID %d\n\n\n", result, unitId, unit->id, playerId);
		__asm {
			POPAD
			MOV EAX, result
			MOV ESP, EBP
			POP EBP
			RETN 4
		}
	}
}

namespace hooks {
	void injectUnitCreateAllowedWrapper() {
		jmpPatch(getUnitCreateAllowedWrapper, 0x0046E1C0, 0);
	}
}