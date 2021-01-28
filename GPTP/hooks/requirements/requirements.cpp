#include "requirements.h"
#include "requirements_parser.h"
#include "SCBW/structures/Player.h"
#include <SCBW/api.h>
#include <DebugUtils.h>

namespace {
	s32 parseRequirementOpcodesLogic(CUnit* unit, u32 datReqOffset, u16 id, u32 player, u16* datReqBase);
	s8 UnitCreateAllowedLogic(u16 unitId, CUnit* parentUnit, int playerID, u16* unitsDat, u16* requirementsTable);

	void parseUnitsDatReqs();
}

namespace hooks {
	s32 parseRequirementOpcodes(CUnit* unit, u32 datReqOffset, u16 id, u32 player, u16* datReqBase) { //0x0046D610

		if (datReqBase == requirements::orders && customRequirements::getOrderRequirementOverrides()[id] != 0) {
			datReqOffset = customRequirements::getOrderRequirementOverrides()[id];
			datReqBase = customRequirements::getOrderRequirementOpcodes();
		} else if (datReqBase == requirements::research && customRequirements::getResearchRequirementOverrides()[id] != 0) {
			datReqOffset = customRequirements::getResearchRequirementOverrides()[id];
			datReqBase = customRequirements::getResearchRequirementOpcodes();
		} else if (datReqBase == requirements::tech && customRequirements::getTechRequirementOverrides()[id] != 0) {
			datReqOffset = customRequirements::getTechRequirementOverrides()[id];
			datReqBase = customRequirements::getTechRequirementOpcodes();
		} else if (datReqBase == requirements::upgrade && customRequirements::getUpgradeRequirementOverrides()[id] != 0) {
			datReqOffset = customRequirements::getUpgradeRequirementOverrides()[id];
			datReqBase = customRequirements::getUpgradeRequirementOpcodes();
		}

		s32 result = parseRequirementOpcodesLogic(unit, datReqOffset, id, player, datReqBase);
		return result;
	}

	s8 UnitCreateAllowed(u16 unitId, CUnit* parentUnit, int playerID) { //0046E1C0
		u16* unitsDat = units_dat::unitsDat43;
		u16* requirementsTable = requirements::units;

		if (customRequirements::getUnitRequirementOverrides()[unitId] != 0) {
			unitsDat = customRequirements::getUnitRequirementOverrides();
			requirementsTable = customRequirements::getUnitRequirementOpcodes();;
		}

		return UnitCreateAllowedLogic(unitId, parentUnit, playerID, unitsDat, requirementsTable);
	}
}

namespace {
	s32 parseRequirementOpcodesLogic(CUnit* unit, u32 datReqOffset, u16 id, u32 playerId, u16* datReqBase) {
		// args:
		// eax          u32    dat req offset
		// esi          CUnit* unit
		// ebp+8  arg1  u16    tech/upg ID
		// ebp+c  arg2  u32    playerId
		// ebp+10 arg3  u16*   dat req script ptr

		bool invalidUnit = false; // janky 'Current Unit Is...' logic variables
		bool greyButton = true; // greys out the button when reqs aren't met
		bool unitNotFound = true; // allows "invalid unit" to be set

		bool allowHalluc = false;

		// 'Or' logic tracking
		u32 success = 0; // Has integer values added to it, but is essentially treated like a boolean

		u16 opcode;
		u16 unitId;
		u16 findopcode; // for upgrade level opcode

		while (datReqBase[datReqOffset] != RequirementOpcodes::EndOfSublist) {
			opcode = datReqBase[datReqOffset++];
			switch (opcode) {
			case RequirementOpcodes::MustBeBroodWar: // Must be Brood War
				if (scbw::isBroodWarMode() == false) {
					*lastInternalErr = InternalErrors::UnknownError26;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::CurrentUnitIs: // Current Unit Is...
				unitId = datReqBase[datReqOffset++];
				if (PLAYER::numberOfCompletedUnitsOfType(unitId, playerId) != 0) {
					greyButton = false;
				}
				if (unit->id != unitId) {
					if (unitNotFound == false) break;
					invalidUnit = true;
					break;
				}
				invalidUnit = false;
				unitNotFound = false;
				success++;
				break;

			case RequirementOpcodes::MustHave:
				unitId = datReqBase[datReqOffset++];
				success += PLAYER::numberOfCompletedUnitsOfType(unitId, playerId) + PLAYER::numberOfUnitsOfType(unitId, playerId);
				break;

			case RequirementOpcodes::IsTransport:
				if (unit->status & UnitStatus::IsHallucination) break;
				if (unit->id == UnitId::overlord && UpgradesSc->currentLevel[unit->playerId][UpgradeId::VentralSacs] == 0) break;
				if (units_dat::SpaceProvided[unit->id] == 0) break;
				success++;
				break;

			case RequirementOpcodes::MustBeResearched:
				if (id == TechId::Burrowing && unit->id == UnitId::lurker) {
					success++;
					break;
				}
				if (scbw::hasTechResearched(playerId, id) == false && scbw::isCheatEnabled(CheatFlags::MedievalMan) == false) {
					break;
				}
				success++;
				break;

			case RequirementOpcodes::IsHeroAndEnabled:
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::Hero) == 0) break;
				success++;
				break;

			case RequirementOpcodes::IsNotTrainingOrMorphing:
				if (unit->unitIsTrainingOrMorphing() || unit->building.techType != TechId::None) { // unitIsTrainingOrMorphing 00401500 0000006A
					*lastInternalErr = InternalErrors::UnknownError5;
					return 0;
				}
				// fall through
			case RequirementOpcodes::IsNotUpgrading:
				if (unit->building.upgradeType != UpgradeId::None) {
					*lastInternalErr = InternalErrors::UnknownError5;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsNotConstructingAddon:
				if (unit->secondaryOrderId == OrderId::BuildAddon && (unit->status & UnitStatus::GroundedBuilding) != 0 && unit->currentBuildUnit != NULL && (unit->currentBuildUnit->status & UnitStatus::Completed) == 0) {
					*lastInternalErr = InternalErrors::UnknownError5;
					return 0;
				}
				if (unit->mainOrderId == OrderId::BuildProtoss1 && (unit->status & UnitStatus::GroundedBuilding) != 0 && unit->orderTarget.unit != NULL && (unit->orderTarget.unit->status & UnitStatus::Completed) == 0) {
					*lastInternalErr = InternalErrors::UnknownError5;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsNotResearching:
				if (unit->building.techType != TechId::None) {
					*lastInternalErr = InternalErrors::UnknownError5;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsNotLiftedOff: // Is not lifted off
				if ((unit->status & UnitStatus::GroundedBuilding) == 0) {
					*lastInternalErr = InternalErrors::UnknownError7;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsLiftedOff: // Is lifted off
				if ((unit->status & UnitStatus::GroundedBuilding) != 0) {
					*lastInternalErr = InternalErrors::UnknownError6;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::DoesNotHaveExit: // Does not have exit
				if (unit->building.nydusExit != NULL) {
					// or unit->building.ghostNukeMissile
					// or unit->building.nuke
					// or unit->building.pylonAura
					// or unit-> Worker - Harvest target unit
					*lastInternalErr = InternalErrors::UnitBusyOrIncompatible;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsNotBurrowed: // Is not burrowed
				if ((unit->status & UnitStatus::Burrowed) != 0 && unit->pAI == NULL) {
					*lastInternalErr = InternalErrors::UnknownError5;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsBurrowed: // Is burrowed
				if ((unit->status & UnitStatus::Burrowed) == 0) {
					*lastInternalErr = InternalErrors::UnknownError5;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::CanAttack: // Can attack -- duplicate of "Is lifted off"???
				if ((unit->status & UnitStatus::GroundedBuilding) != 0) {
					*lastInternalErr = InternalErrors::UnknownError19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::CanSetRallyPoint: // Can set rally point -- duplicate of "Is lifted off"???
				if ((unit->status & UnitStatus::GroundedBuilding) == 0) {
					*lastInternalErr = InternalErrors::UnknownError19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::CanMove:
				if ((unit->status & UnitStatus::GroundedBuilding) != 0) {
					*lastInternalErr = InternalErrors::UnknownError19;
					return 0;
				}
				if ((unit->id == UnitId::lurker || (unit->status & UnitStatus::Burrowed) == 0) && units_dat::RightClickAction[unit->id] == RightClickActions::NoCommand_AutoAttack) {
					*lastInternalErr = InternalErrors::UnknownError19;
					return 0;
				}
				if (unit->getRightClickActionOrder() == RightClickActions::NoMove_NormalAttack) {
					*lastInternalErr = InternalErrors::UnknownError19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::HasWeapon:
				if (unit->hasWeapon() == false) {
					*lastInternalErr = InternalErrors::UnknownError19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsSubunit:
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::Subunit) == 0) {
					*lastInternalErr = InternalErrors::UnknownError19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsFlyingBuilding:
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::FlyingBuilding) == 0) {
					*lastInternalErr = InternalErrors::UnknownError19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsWorker:
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::Worker) == 0) {
					*lastInternalErr = InternalErrors::UnknownError19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsPowerup:
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::NeutralAccessories) == 0) {
					*lastInternalErr = InternalErrors::UnknownError19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::HasMines:
				if (unit->vulture.spiderMineCount == 0) {
					*lastInternalErr = InternalErrors::UnitBusyOrIncompatible;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::CanHoldPosition:
				if ((unit->id != UnitId::lurker || (unit->status & UnitStatus::Burrowed) == 0)
					&& units_dat::RightClickAction[unit->id] == RightClickActions::NoCommand_AutoAttack
					&& ((unit->status & UnitStatus::GroundedBuilding) == 0 || unit->isFactory() == false)) {
					*lastInternalErr = InternalErrors::UnitBusyOrIncompatible;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::AllowOnHallucinations:
				allowHalluc = true;
				success++;
				break;

			case RequirementOpcodes::UpgradeLv1Requires:
				switch (scbw::getUpgradeLevel(playerId, id)) {
				default:
					findopcode = RequirementOpcodes::UpgradeLv1Requires;
					break;
				case 2:
					findopcode = RequirementOpcodes::UpgradeLv3Requires;
					break;
				case 1:
					findopcode = RequirementOpcodes::UpgradeLv2Requires;
				}
				while (opcode != findopcode) {
					opcode = datReqBase[datReqOffset++];
				}
				success++;
				break;

			case RequirementOpcodes::Grey:
				*lastInternalErr = InternalErrors::UnknownError21;
				return -1;

			case RequirementOpcodes::Blank:
				*lastInternalErr = InternalErrors::MissingRequirement;
				return 0;

			default:
				// Any invalid opcodes will be read here as unit IDs
				unitId = opcode;
				success += PLAYER::numberOfCompletedUnitsOfType(playerId, unitId);
			}

			if (datReqBase[datReqOffset] == RequirementOpcodes::Or) { // Or
				datReqOffset++;
				continue; // continue loop without clearing success or applying failure
			}

			// no button on failure
			if (invalidUnit == true) {
				*lastInternalErr = InternalErrors::UnknownError25;
				return 0;
			}

			// grey button on failure
			if (success == 0 && greyButton == true) {
				*lastInternalErr = InternalErrors::UnitBusyOrIncompatible;
				return -1;
			}

			greyButton = true;
			success = 0;
		}

		// Hides button for hallucinations unless explicitly allowed
		if ((unit->status & UnitStatus::IsHallucination) != 0 && allowHalluc == false) {
			*lastInternalErr = InternalErrors::UnknownError22;
			return 0;
		}

		return 1;
	}

	s8 UnitCreateAllowedLogic(u16 unitId, CUnit* parentUnit, int playerID, u16* unitsDat, u16* requirementsTable) {
		//DebugOut("UnitCreateAllowedLogic: untitID %d, creatingUnit %d, playerID %d", unitId, parentUnit->id, playerID);
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
			//DebugOut(">Return NotAllowed");
			return 0;
		}
		if (playerID != parentUnit->playerId) {
			*lastInternalErr = InternalErrors::UnknownError1;
			//DebugOut(">Return NotAllowed");
			return 0;
		}
		if (!(parentUnit->status & UnitStatus::Completed)) {
			*lastInternalErr = InternalErrors::UnknownError20;
			//DebugOut(">Return NotAllowed");
			return 0;
		}
		if (parentUnit->isFrozen()) {
			*lastInternalErr = InternalErrors::UnitFrozen;
			//DebugOut(">Return NotAllowed");
			return 0;
		}
		if (!UnitAvailability->available[parentUnit->playerId][parentUnit->id]) {
			*lastInternalErr = InternalErrors::UnknownError2;
			//DebugOut(">Return NotAllowed");
			return 0;
		}
		if (parentUnit->isQueueSlotActive(4)) {
		LABEL_86_ERROR_3_RETURN_0:
			*lastInternalErr = InternalErrors::UnknownError3;
			//DebugOut(">Return NotAllowed");
			return 0;
		}
		if (parentUnit->status & UnitStatus::IsHallucination) {
			*lastInternalErr = InternalErrors::UnknownError22;
			//DebugOut(">Return Disallowed");
			return 0;
		}

		if (unitsDat[unitId] == RequirementOpcodes::EndOfSublist) {
			//DebugOut("EndOfSublist (%d):", currentOpcode);
			parseUnitsDatReqs();
		}

		////DebugOut("opcode index = %d", opcodeIndex);
		if (unitsDat[unitId] == 0) {
		LABEL_93_ERROR_23_RETURN_0:
			*lastInternalErr = 23;
			//DebugOut(">Return NotAllowed");
			return 0;
		}
		v27 = 1;
		v26 = 1;
		//LOWORD(v8) = requirementsTable[v7];
		//ByteOperations::SetLowWord(v8, requirements::requirementsTable[v7]);

		v28 = 0;
		if (currentOpcode == RequirementOpcodes::EndOfSublist) {
			//DebugOut(">Return Allowed");
			return 1;
		}
	LABEL_18_ENTER_MAIN_BODY:
		////DebugOut("  MAIN_BODY");
		v9 = 0;
		v29 = 0;
		while (2)
		{
			////DebugOut("v8 = %d", currentOpcode);
			switch (currentOpcode) {
			case RequirementOpcodes::CurrentUnitIs:
			{
				u16 nextOpcodeUnitId = requirementsTable[opcodeIndex + 1];
				//DebugOut("CurrentUnitIs (%d) unitId = %d:", currentOpcode, nextOpcodeUnitId);
				////DebugOut("  current unit is = %d", nextOpcodeUnitId);
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
					//DebugOut(">True");
					goto LABEL_72_NEXT_OPCODE;
				}
				if (v26) {
					v28 = 1;
				}
				//DebugOut(">False");
				goto LABEL_73_PERHAPS_FINAL_CHECK;
			}
			case RequirementOpcodes::MustHave:
				//DebugOut("MustHave (%d):", currentOpcode);
				v24 = requirementsTable[opcodeIndex++];
				v25 = PLAYER::numberOfCompletedUnitsOfType(playerID, currentOpcode) + PLAYER::numberOfUnitsOfType(playerID, currentOpcode);
				//DebugOut(">Checking next opcode");
				goto LABEL_71_SUM_VALUES;
			case RequirementOpcodes::MustHaveAddon:
			{
				//DebugOut("MustHaveAddon (%d):", currentOpcode);
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
							//DebugOut(">Return Gray");
							return -1;
						}
					}
					v11 = v9 + 1;
				}
				//DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			}
			case RequirementOpcodes::IsNotLiftedOff:
				//DebugOut("IsNotLiftedOff (%d):", currentOpcode);
				if (!(parentUnit->status & UnitStatus::GroundedBuilding))
				{
					*lastInternalErr = InternalErrors::UnknownError7;
					//DebugOut(">Return NotAllowed");
					return 0;
				}
				v11 = v9 + 1;
				//DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case RequirementOpcodes::IsNotTrainingOrMorphing:
				//DebugOut("IsNotTrainingOrMorphing (%d):", currentOpcode);
				if (parentUnit->unitIsTrainingOrMorphing() || parentUnit->building.upgradeType != TechId::None || parentUnit->building.techType != UpgradeId::None) {
					//DebugOut(">False");
					goto LABEL_87;
				}
				++v28;
				//DebugOut(">True");
				goto LABEL_73_PERHAPS_FINAL_CHECK;
			case RequirementOpcodes::IsNotConstructingAddon:
				//DebugOut("IsNotConstructingAddon (%d):", currentOpcode);
				if (parentUnit->isConstructingAddon() || parentUnit->isAttemptingProtossBuild()) {
					//DebugOut(">False");
					goto LABEL_87;
				}
				v11 = v15 + 1;
				//DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case RequirementOpcodes::IsNotResearching:
				//DebugOut("IsNotResearching (%d):", currentOpcode);
				if (parentUnit->building.techType != TechId::None) {
					//DebugOut(">False");
					goto LABEL_87;
				}
				v11 = v9 + 1;
				//DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case RequirementOpcodes::IsNotUpgrading:
				//DebugOut("IsNotUpgrading (%d):", currentOpcode);
				if (parentUnit->building.upgradeType != UpgradeId::None) {
					//DebugOut(">False");
					goto LABEL_87;
				}
				v11 = v9 + 1;
				//DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case RequirementOpcodes::IsNotConstructing:
				//DebugOut("IsNotConstructing (%d):", currentOpcode);
				if (parentUnit->id == UnitId::TerranSCV && parentUnit->mainOrderId == OrderId::ConstructingBuilding) {
					//DebugOut(">False");
					goto LABEL_87;
				}
				v11 = v9 + 1;
				//DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case RequirementOpcodes::DoesNotHaveAddonAttached:
				//DebugOut("DoesNotHaveAddonAttached (%d):", currentOpcode);
				if (parentUnit->building.addon)
				{
					if (!v28) {
						*lastInternalErr = InternalErrors::UnknownError4;
					}
					//DebugOut(">Return NotAllowed");
					return 0;
				}
				v11 = v9 + 1;
			LABEL_72_NEXT_OPCODE:
				////DebugOut("  NEXT_OPCODE");
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
					//DebugOut(">Return Gray");
					return -1;
				}
				//ByteOperations::SetLowWord(currentOpcode, requirementsTable[opcodeIndex]);
				currentOpcode = requirementsTable[opcodeIndex];
				v27 = 1;
				if (currentOpcode != RequirementOpcodes::EndOfSublist)
					goto LABEL_18_ENTER_MAIN_BODY;
				if (v28)
				{
					*lastInternalErr = InternalErrors::UnknownError25;
					//DebugOut(">Return Gray");
					return -1;
				}
				//DebugOut(">Return Allowed");
				return 1;
			case RequirementOpcodes::HasHangarSpace:
				//DebugOut("HasHangarSpace (%d):", currentOpcode);
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
					//DebugOut(">False");
					goto LABEL_86_ERROR_3_RETURN_0;
				}
				++v29;
				//DebugOut(">True");
				goto LABEL_73_PERHAPS_FINAL_CHECK;
			case RequirementOpcodes::DoesNotHaveArmedMissile:
				//DebugOut("DoesNotHaveArmedMissile (%d):", currentOpcode);
				if (parentUnit->building.silo.nuke) {
					//DebugOut(">False");
					goto LABEL_86_ERROR_3_RETURN_0;
				}
				v11 = v9 + 1;
				//DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case RequirementOpcodes::IsNotBurrowed:
				//DebugOut("IsNotBurrowed (%d):", currentOpcode);
				if (parentUnit->status & UnitStatus::Burrowed)
				{
				LABEL_87:
					*lastInternalErr = InternalErrors::UnknownError5;
					//DebugOut(">Return NotAllowed");
					return 0;
				}
				v11 = v9 + 1;
				//DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case RequirementOpcodes::Grey:
				//DebugOut("Grey (%d):", currentOpcode);
				*lastInternalErr = InternalErrors::UnknownError21;
				//DebugOut(">Return Grey");
				return -1;
			case RequirementOpcodes::Blank:
				//DebugOut("Blank (%d):", currentOpcode);
				goto LABEL_93_ERROR_23_RETURN_0;
			case RequirementOpcodes::MustBeBroodWar:
				//DebugOut("MustBeBroodWar (%d):", currentOpcode);
				if (!IS_BROOD_WAR)
				{
					*lastInternalErr = InternalErrors::UnknownError26;
					//DebugOut(">Return NotAllowed");
					return 0;
				}
				v11 = v9 + 1;
				//DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			case RequirementOpcodes::IsResearched:
				//DebugOut("IsResearched (%d):", currentOpcode);
				v10 = requirementsTable[opcodeIndex++];
				if (scbw::hasTechResearched(parentUnit->playerId, v10) && !(*CHEAT_STATE & CheatFlags::MedievalMan))
				{
					*lastInternalErr = InternalErrors::UnknownError9;
					//DebugOut(">Return Gray");
					return -1;
				}
				++v29;
				//DebugOut(">True");
				goto LABEL_73_PERHAPS_FINAL_CHECK;
			case RequirementOpcodes::IsBurrowed:
				//DebugOut("IsBurrowed (%d):", currentOpcode);
				if (!(parentUnit->status & UnitStatus::Burrowed))
				{
					*lastInternalErr = InternalErrors::UnknownError5;
					//DebugOut(">Return Gray");
					return -1;
				}
				v11 = v9 + 1;
				//DebugOut(">True");
				goto LABEL_72_NEXT_OPCODE;
			default:
				//DebugOut("(Default) numberOfCompletedUnitsOfType (%d)", currentOpcode);
				if (*CHEAT_STATE & CheatFlags::ModifyThePhaseVariance)
					++v9;
				v25 = PLAYER::numberOfCompletedUnitsOfType(playerID, currentOpcode);
				//DebugOut(">%s", v25 > 0 ? "True" : "False");
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