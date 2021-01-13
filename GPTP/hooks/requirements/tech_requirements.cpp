#include "tech_requirements.h"
#include "SCBW/structures/Player.h"
#include <SCBW/api.h>
#include <DebugUtils.h>


namespace {
	s32 parseRequirementOpcodesLogic(CUnit* unit, u32 datReqOffset, u16 id, u32 player, u16* datReqBase);
}

namespace hooks {
	s32 parseRequirementOpcodes(CUnit* unit, u32 datReqOffset, u16 id, u32 player, u16* datReqBase) {
		// Here we would do overrides to datReqOffset and datReqBase if needed
		s32 result = parseRequirementOpcodesLogic(unit, datReqOffset, id, player, datReqBase);
		return result;
	}
}

namespace {
	//parseRequirementOpcodes 0046D610 0000057A
	s32 parseRequirementOpcodesLogic(CUnit* unit, u32 datReqOffset, u16 id, u32 playerId, u16* datReqBase) {
		// args:
		// eax          u32    dat req offset
		// esi          CUnit* unit
		// ebp+8  arg1  u16    tech/upg ID
		// ebp+c  arg2  u32    playerId
		// ebp+10 arg3  u16*   dat req script ptr

		  // janky 'Current Unit Is...' logic variables
		bool invalidUnit = false;
		bool greyButton = true; // greys out the button when reqs aren't met
		bool unitNotFound = true; // allows "invalid unit" to be set

		bool allowHalluc = false;

		// 'Or' logic tracking
		u32 success = 0; // Has integer values added to it, but is essentially treated like a boolean

		u16 opcode;
		u16 unitid;
		u16 findopcode; // for upgrade level opcode

		if (datReqBase == requirements::upgrade) {
			//DebugOut("parseRequirementOpcodes upgrades for: upgradeId %d, unit %d, playerID %d (datReqOffset=%d, datReqBase=%p)", id, unit->id, playerId, datReqOffset, datReqBase);
		}
		else {
			//DebugOut("parseRequirementOpcodes research for: techId %d, unit %d, playerID %d (datReqOffset=%d, datReqBase=%p)", id, unit->id, playerId, datReqOffset, datReqBase);
		}


		while (datReqBase[datReqOffset] != RequirementOpcodes::EndOfSublist) {
			opcode = datReqBase[datReqOffset++];
			switch (opcode) {
			case RequirementOpcodes::MustBeBroodWar: // Must be Brood War
				if (scbw::isBroodWarMode() == false) {
					*lastInternalErr = 26;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::CurrentUnitIs: // Current Unit Is...
				unitid = datReqBase[datReqOffset++];
				if (PLAYER::numberOfCompletedUnitsOfType(unitid, playerId) != 0) {
					greyButton = false;
				}
				if (unit->id != unitid) {
					if (unitNotFound == false) break;
					invalidUnit = true;
					break;
				}
				invalidUnit = false;
				unitNotFound = false;
				success++;
				break;

			case RequirementOpcodes::MustHave:
				unitid = datReqBase[datReqOffset++];
				success += PLAYER::numberOfCompletedUnitsOfType(unitid, playerId) + PLAYER::numberOfUnitsOfType(unitid, playerId);
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
					*lastInternalErr = 5;
					return 0;
				}
				// fall through
			case RequirementOpcodes::IsNotUpgrading:
				if (unit->building.upgradeType != UpgradeId::None) {
					*lastInternalErr = 5;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsNotConstructingAddon:
				if (unit->secondaryOrderId == OrderId::BuildAddon && (unit->status & UnitStatus::GroundedBuilding) != 0 && unit->currentBuildUnit != NULL && (unit->currentBuildUnit->status & UnitStatus::Completed) == 0) {
					*lastInternalErr = 5;
					return 0;
				}
				if (unit->mainOrderId == OrderId::BuildProtoss1 && (unit->status & UnitStatus::GroundedBuilding) != 0 && unit->orderTarget.unit != NULL && (unit->orderTarget.unit->status & UnitStatus::Completed) == 0) {
					*lastInternalErr = 5;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsNotResearching:
				if (unit->building.techType != TechId::None) {
					*lastInternalErr = 5;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsNotLiftedOff: // Is not lifted off
				if ((unit->status & UnitStatus::GroundedBuilding) == 0) {
					*lastInternalErr = 7;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsLiftedOff: // Is lifted off
				if ((unit->status & UnitStatus::GroundedBuilding) != 0) {
					*lastInternalErr = 6;
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

					*lastInternalErr = 8;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsNotBurrowed: // Is not burrowed
				if ((unit->status & UnitStatus::Burrowed) != 0 && unit->pAI == NULL) {
					*lastInternalErr = 5;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsBurrowed: // Is burrowed
				if ((unit->status & UnitStatus::Burrowed) == 0) {
					*lastInternalErr = 5;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::CanAttack: // Can attack -- duplicate of "Is lifted off"???
				if ((unit->status & UnitStatus::GroundedBuilding) != 0) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::CanSetRallyPoint: // Can set rally point -- duplicate of "Is lifted off"???
				if ((unit->status & UnitStatus::GroundedBuilding) == 0) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::CanMove:
				if ((unit->status & UnitStatus::GroundedBuilding) != 0) {
					*lastInternalErr = 19;
					return 0;
				}
				if ((unit->id == UnitId::lurker || (unit->status & UnitStatus::Burrowed) == 0) && units_dat::RightClickAction[unit->id] == RightClickActions::NoCommand_AutoAttack) {
					*lastInternalErr = 19;
					return 0;
				}
				if (unit->getRightClickActionOrder() == RightClickActions::NoMove_NormalAttack) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::HasWeapon:
				if (unit->hasWeapon() == false) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsSubunit:
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::Subunit) == 0) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsFlyingBuilding:
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::FlyingBuilding) == 0) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsWorker:
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::Worker) == 0) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::IsPowerup:
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::NeutralAccessories) == 0) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::HasMines:
				if (unit->vulture.spiderMineCount == 0) {
					*lastInternalErr = 8;
					return 0;
				}
				success++;
				break;

			case RequirementOpcodes::CanHoldPosition:
				if ((unit->id != UnitId::lurker || (unit->status & UnitStatus::Burrowed) == 0)
					&& units_dat::RightClickAction[unit->id] == RightClickActions::NoCommand_AutoAttack
					&& ((unit->status & UnitStatus::GroundedBuilding) == 0 || unit->isFactory() == false)) {
					*lastInternalErr = 8;
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
				*lastInternalErr = 21;
				return -1;

			case RequirementOpcodes::Blank:
				*lastInternalErr = 23;
				return 0;

			default:
				// Any invalid opcodes will be read here as unit IDs
				unitid = datReqBase[datReqOffset++];
				success += PLAYER::numberOfCompletedUnitsOfType(playerId, unitid);
			}

			if (datReqBase[datReqOffset] == RequirementOpcodes::Or) { // Or
				datReqOffset++;
				continue; // continue loop without clearing success or applying failure
			}

			// no button on failure
			if (invalidUnit == true) {
				*lastInternalErr = 25;
				return 0;
			}

			// grey button on failure
			if (success == 0 && greyButton == true) {
				*lastInternalErr = 8;
				return -1;
			}

			greyButton = true;
			success = 0;
		}

		// Hides button for hallucinations unless explicitly allowed
		if ((unit->status & UnitStatus::IsHallucination) != 0 && allowHalluc == false) {
			*lastInternalErr = 22;
			return 0;
		}

		return 1;
	}

}