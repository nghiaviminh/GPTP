#include "requirements.h"
#include <hook_tools.h>

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
		__asm {
			popad
			mov eax, result
			pop ebp
			retn 12
		}
	}
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
	s32 parseRequirementOpcodes(CUnit* unit, u32 datReqOffset, u16 techID, u32 player)
	{
		return s32();
	}
	void injectParseRequirementOpcodesWrapper() {
		jmpPatch(getParseRequirementOpcodesWrapper, 0x0046D610, 0);
	}
	void injectUnitCreateAllowedWrapper() {
		jmpPatch(getUnitCreateAllowedWrapper, 0x0046E1C0, 0);
	}
}