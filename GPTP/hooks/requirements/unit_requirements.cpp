#include "unit_requirements.h"
#include <DebugUtils.h>
#include "SCBW/structures/Player.h"

namespace {
	s8 UnitCreateAllowedLogic(u16 unitId, CUnit* parentUnit, int playerID, u16* unitsDat, u16* requirementsTable);
	void parseUnitsDatReqs();
}

namespace hooks {
	s8 UnitCreateAllowed(u16 unitId, CUnit* parentUnit, int playerID) { //0046E1C0
		u16* unitsDat = units_dat::unitsDat43;
		u16* requirementsTable = requirements::units;

		// Overriding probe for now as proof of concept
		/*
		if (unitId == UnitId::ProtossProbe) {
			u16 unitsDatCustom[228];
			unitsDatCustom[UnitId::ProtossProbe] = 11;
			unitsDat = unitsDatCustom;

			u16 customRequirementsTable[1000];
			customRequirementsTable[10] = UnitId::ProtossProbe;
			customRequirementsTable[11] = UnitCreateRequirementOpcodes::CurrentUnitIs;
			customRequirementsTable[12] = UnitId::ProtossNexus;
			customRequirementsTable[13] = UnitId::ProtossPylon;
			customRequirementsTable[14] = UnitCreateRequirementOpcodes::IsNotConstructingAddon;
			customRequirementsTable[15] = UnitCreateRequirementOpcodes::EndOfSublist;
			requirementsTable = customRequirementsTable;
		}
		*/
		return UnitCreateAllowedLogic(unitId, parentUnit, playerID, unitsDat, requirementsTable);
	}
}

namespace {
	s8 UnitCreateAllowedLogic(u16 unitId, CUnit* parentUnit, int playerID, u16* unitsDat, u16* requirementsTable) {
		DebugOut("UnitCreateAllowedLogic: untitID %d, creatingUnit %d, playerID %d", unitId, parentUnit->id, playerID);
		// TODO map variables and rewrite process accordingly
		s16 v5;
		s16 v6;
		s16 opcodeIndex;
		u16 currentOpcode; // used to be s16
		s16 v9;
		u16 v10;
		s16 v11;
		s16 v13;
		s16 v14;
		s16 v15 = 0;
		s16 v16;
		s16 v17;
		u8 v18;
		s16 v19;
		char v20;
		s16 v21;
		s16 v22;
		s16 v24;
		DWORD v25;
		s16 v26;
		s16 v27;
		s16 v28;
		s16 v29;

		opcodeIndex = (s16)unitsDat[unitId];
		currentOpcode = requirementsTable[opcodeIndex];

		*lastInternalErr = InternalErrors::UnknownError0;
		if (unitId >= UnitId::None) {
			*lastInternalErr = InternalErrors::UnknownError16;
			DebugOut(">Return NotAllowed");
			return 0;
		}
		if (playerID != parentUnit->playerId) {
			*lastInternalErr = InternalErrors::UnknownError1;
			DebugOut(">Return NotAllowed");
			return 0;
		}
		if (!(parentUnit->status & UnitStatus::Completed)) {
			*lastInternalErr = InternalErrors::UnknownError20;
			DebugOut(">Return NotAllowed");
			return 0;
		}
		if (parentUnit->isFrozen()) {
			*lastInternalErr = InternalErrors::UnitFrozen;
			DebugOut(">Return NotAllowed");
			return 0;
		}
		if (!UnitAvailability->available[parentUnit->playerId][parentUnit->id]) {
			*lastInternalErr = InternalErrors::UnknownError2;
			DebugOut(">Return NotAllowed");
			return 0;
		}
		if (parentUnit->isQueueSlotActive(4)) {
		LABEL_86_ERROR_3_RETURN_0:
			*lastInternalErr = InternalErrors::UnknownError3;
			DebugOut(">Return NotAllowed");
			return 0;
		}
		if (parentUnit->status & UnitStatus::IsHallucination) {
			*lastInternalErr = InternalErrors::UnknownError22;
			DebugOut(">Return Disallowed");
			return 0;
		}

		if (unitsDat[unitId] == UnitCreateRequirementOpcodes::EndOfSublist) {
			DebugOut("EndOfSublist (%d):", currentOpcode);
			parseUnitsDatReqs();
		}

		//DebugOut("opcode index = %d", opcodeIndex);
		if (unitsDat[unitId] == 0) {
		LABEL_93_ERROR_23_RETURN_0:
			*lastInternalErr = 23;
			DebugOut(">Return NotAllowed");
			return 0;
		}
		v27 = 1;
		v26 = 1;
		//LOWORD(v8) = requirementsTable[v7];
		//ByteOperations::SetLowWord(v8, requirements::requirementsTable[v7]);

		v28 = 0;
		if (currentOpcode == UnitCreateRequirementOpcodes::EndOfSublist) {
			DebugOut(">Return Allowed");
			return 1;
		}
	LABEL_18_ENTER_MAIN_BODY:
		//DebugOut("  MAIN_BODY");
		v9 = 0;
		v29 = 0;
		while (2)
		{
			//DebugOut("v8 = %d", currentOpcode);
			switch (currentOpcode) {
			case UnitCreateRequirementOpcodes::CurrentUnitIs:
			{
				u16 nextOpcodeUnitId = requirementsTable[opcodeIndex + 1];
				DebugOut("CurrentUnitIs (%d) unitId = %d:", currentOpcode, nextOpcodeUnitId);
				//DebugOut("  current unit is = %d", nextOpcodeUnitId);
				// Weirdly, nextOpcodeUnitId is sometimes still the old opcode value - but only while debugging (or when there's 
				// an OR statement?) this is causing access violation reading completed_unit_counts. Work around this for now by checking unit Ids
				if (PLAYER::numberOfCompletedUnitsOfType(parentUnit->playerId, nextOpcodeUnitId) > 0) {
					v27 = 0;
				}
				if (nextOpcodeUnitId == parentUnit->id)
				{
					v28 = 0;
					v26 = 0;
					v11 = v9 + 1;
					DebugOut(">True");
					goto LABEL_72_NEXT_OPCODE;
				}
				if (v26) {
					v28 = 1;
				}
				DebugOut(">False");
				goto LABEL_73_PERHAPS_FINAL_CHECK;
			}
			case UnitCreateRequirementOpcodes::MustHave:
				DebugOut("MustHave (%d):", currentOpcode);
				v24 = requirementsTable[opcodeIndex++];
				v25 = PLAYER::numberOfCompletedUnitsOfType(playerID, currentOpcode) + PLAYER::numberOfUnitsOfType(playerID, currentOpcode);
				DebugOut(">Checking next opcode");
				goto LABEL_71_SUM_VALUES;
			case UnitCreateRequirementOpcodes::MustHaveAddon:
			{
				DebugOut("MustHaveAddon (%d):", currentOpcode);
				if (*CHEAT_STATE & CheatFlags::ModifyThePhaseVariance)
				{
					++opcodeIndex;
					v11 = v9 + 1;
				}
				else
				{
					CUnit* addonUnit = parentUnit->building.addon;
					if (addonUnit != nullptr)
					{
						if (addonUnit != nullptr || (++opcodeIndex, addonUnit->id != requirementsTable[opcodeIndex]))
						{
							if (!v28) {
								*lastInternalErr = 4;
							}
							DebugOut(">Return Gray");
							return -1;
						}
					}
					v11 = v9 + 1;
				}
				DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			}
			case UnitCreateRequirementOpcodes::IsNotLiftedOff:
				DebugOut("IsNotLiftedOff (%d):", currentOpcode);
				if (!(parentUnit->status & UnitStatus::GroundedBuilding))
				{
					*lastInternalErr = InternalErrors::UnknownError7;
					DebugOut(">Return NotAllowed");
					return 0;
				}
				v11 = v9 + 1;
				DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case UnitCreateRequirementOpcodes::IsNotTrainingOrMorphing:
				DebugOut("IsNotTrainingOrMorphing (%d):", currentOpcode);
				if (parentUnit->unitIsTrainingOrMorphing() || parentUnit->building.upgradeType != TechId::None || parentUnit->building.techType != UpgradeId::None) {
					DebugOut(">False");
					goto LABEL_87;
				}
				++v28;
				DebugOut(">True");
				goto LABEL_73_PERHAPS_FINAL_CHECK;
			case UnitCreateRequirementOpcodes::IsNotConstructingAddon:
				DebugOut("IsNotConstructingAddon (%d):", currentOpcode);
				if (parentUnit->isConstructingAddon() || parentUnit->isAttemptingProtossBuild()) {
					DebugOut(">False");
					goto LABEL_87;
				}
				v11 = v15 + 1;
				DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case UnitCreateRequirementOpcodes::IsNotResearching:
				DebugOut("IsNotResearching (%d):", currentOpcode);
				if (parentUnit->building.techType != TechId::None) {
					DebugOut(">False");
					goto LABEL_87;
				}
				v11 = v9 + 1;
				DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case UnitCreateRequirementOpcodes::IsNotUpgrading:
				DebugOut("IsNotUpgrading (%d):", currentOpcode);
				if (parentUnit->building.upgradeType != UpgradeId::None) {
					DebugOut(">False");
					goto LABEL_87;
				}
				v11 = v9 + 1;
				DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case UnitCreateRequirementOpcodes::IsNotConstructing:
				DebugOut("IsNotConstructing (%d):", currentOpcode);
				if (parentUnit->id == UnitId::TerranSCV && parentUnit->mainOrderId == OrderId::ConstructingBuilding) {
					DebugOut(">False");
					goto LABEL_87;
				}
				v11 = v9 + 1;
				DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case UnitCreateRequirementOpcodes::DoesNotHaveAddonAttached:
				DebugOut("DoesNotHaveAddonAttached (%d):", currentOpcode);
				if (parentUnit->building.addon)
				{
					if (!v28) {
						*lastInternalErr = InternalErrors::UnknownError4;
					}
					DebugOut(">Return NotAllowed");
					return 0;
				}
				v11 = v9 + 1;
			LABEL_72_NEXT_OPCODE:
				//DebugOut("  NEXT_OPCODE");
				v29 = v11;
			LABEL_73_PERHAPS_FINAL_CHECK:
				//Advance opcode
				if (requirementsTable[++opcodeIndex] == -255)
				{
					//ByteOperations::SetLowWord(currentOpcode, requirementsTable[opcodeIndex]);
					currentOpcode = requirementsTable[opcodeIndex];
					v9 = v29;
					++opcodeIndex;
					continue;
				}
				if (!v29 && v27)
				{
					*lastInternalErr = InternalErrors::UnitBusyOrIncompatible;
					DebugOut(">Return Gray");
					return -1;
				}
				//ByteOperations::SetLowWord(currentOpcode, requirementsTable[opcodeIndex]);
				currentOpcode = requirementsTable[opcodeIndex];
				v27 = 1;
				if (currentOpcode != UnitCreateRequirementOpcodes::EndOfSublist)
					goto LABEL_18_ENTER_MAIN_BODY;
				if (v28)
				{
					*lastInternalErr = InternalErrors::UnknownError25;
					DebugOut(">Return Gray");
					return -1;
				}
				DebugOut(">Return Allowed");
				return 1;
			case UnitCreateRequirementOpcodes::HasHangarSpace:
				DebugOut("HasHangarSpace (%d):", currentOpcode);
				v18 = -1;
				//ByteOperations::SetLowWord(v19, unitIsCarrier(parentUnit));
				v19 = (parentUnit->isCarrier());
				if (v19)
				{
					v20 = parentUnit->carrier.inHangarCount + parentUnit->getHangarTrainCount();
				}
				else
				{
					if (!parentUnit->isReaver()) {
						goto LABEL_51;
					}
					v20 = parentUnit->getHangarTrainCount();
				}
				v18 = parentUnit->carrier.inHangarCount + v20;
			LABEL_51:
				if (v18 >= parentUnit->getMaxHangarSpace()) {
					DebugOut(">False");
					goto LABEL_86_ERROR_3_RETURN_0;
				}
				++v29;
				DebugOut(">True");
				goto LABEL_73_PERHAPS_FINAL_CHECK;
			case UnitCreateRequirementOpcodes::DoesNotHaveArmedMissile:
				DebugOut("DoesNotHaveArmedMissile (%d):", currentOpcode);
				if (parentUnit->building.silo.nuke) {
					DebugOut(">False");
					goto LABEL_86_ERROR_3_RETURN_0;
				}
				v11 = v9 + 1;
				DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case UnitCreateRequirementOpcodes::IsNotBurrowed:
				DebugOut("IsNotBurrowed (%d):", currentOpcode);
				if (parentUnit->status & UnitStatus::Burrowed)
				{
				LABEL_87:
					*lastInternalErr = InternalErrors::UnknownError5;
					DebugOut(">Return NotAllowed");
					return 0;
				}
				v11 = v9 + 1;
				DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case UnitCreateRequirementOpcodes::Grey:
				DebugOut("Grey (%d):", currentOpcode);
				*lastInternalErr = InternalErrors::UnknownError21;
				DebugOut(">Return Grey");
				return -1;
			case UnitCreateRequirementOpcodes::Blank:
				DebugOut("Blank (%d):", currentOpcode);
				goto LABEL_93_ERROR_23_RETURN_0;
			case UnitCreateRequirementOpcodes::MustBeBroodWar:
				DebugOut("MustBeBroodWar (%d):", currentOpcode);
				if (!IS_BROOD_WAR)
				{
					*lastInternalErr = InternalErrors::UnknownError26;
					DebugOut(">Return NotAllowed");
					return 0;
				}
				v11 = v9 + 1;
				DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case UnitCreateRequirementOpcodes::IsResearched:
				DebugOut("IsResearched (%d):", currentOpcode);
				v10 = requirementsTable[opcodeIndex++];
				if (scbw::hasTechResearched(parentUnit->playerId, v10) && !(*CHEAT_STATE & CheatFlags::MedievalMan))
				{
					*lastInternalErr = InternalErrors::UnknownError9;
					DebugOut(">Return Gray");
					return -1;
				}
				++v29;
				DebugOut(">True");
				goto LABEL_73_PERHAPS_FINAL_CHECK;
			case UnitCreateRequirementOpcodes::IsBurrowed:
				DebugOut("IsBurrowed (%d):", currentOpcode);
				if (!(parentUnit->status & UnitStatus::Burrowed))
				{
					*lastInternalErr = InternalErrors::UnknownError5;
					DebugOut(">Return Gray");
					return -1;
				}
				v11 = v9 + 1;
				DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			default:
				DebugOut("(Default) numberOfCompletedUnitsOfType (%d)", currentOpcode);
				if (*CHEAT_STATE & CheatFlags::ModifyThePhaseVariance)
					++v9;
				v25 = PLAYER::numberOfCompletedUnitsOfType(playerID, currentOpcode);
				DebugOut(">%s", v25 > 0 ? "True" : "False");
			LABEL_71_SUM_VALUES:
				v11 = v25 + v9;
				goto LABEL_72_NEXT_OPCODE;
			}
		}
	}

	void parseUnitsDatReqs() {
		const u32 Func_parseUnitDatReqs = 0x0046e100;
		__asm {
			PUSHAD
			CALL Func_parseUnitDatReqs
			POPAD
		}
	}
}