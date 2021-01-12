#include "tech_requirements.h"
#include "SCBW/structures/Player.h"
#include <SCBW/api.h>

namespace {
	s32 parseRequirementOpcodesLogic(CUnit* unit, u32 datReqOffset, u16 techID, u32 player, u16* datReqBase);
}

namespace hooks {

	s32 parseRequirementOpcodes(CUnit* unit, u32 datReqOffset, u16 techID, u32 player) {

		u16* datReqBase = requirements::tech;
		s32 result = parseRequirementOpcodesLogic(unit, datReqOffset, techID, player, datReqBase);
		return result;
	}
}

namespace {
	//parseRequirementOpcodes 0046D610 0000057A

	// "techID" can also be an upgrade ID
	// Return values seem to be:
	//  1: Enabled
	//  0: Blank
	// -1: Grey

	s32 parseRequirementOpcodesLogic(CUnit* unit, u32 datReqOffset, u16 techID, u32 player, u16* datReqBase) {
		// args:
		// eax          u32    dat req offset
		// esi          CUnit* unit
		// ebp+8  arg1  u16    tech/upg ID
		// ebp+c  arg2  u32    player
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

		while (datReqBase[datReqOffset] != 0xFFFF) {
			opcode == datReqBase[datReqOffset++];
			switch (opcode) {
			case 0xFF24: // Must be Brood War
				if (scbw::isBroodWarMode() == false) {
					*lastInternalErr = 26;
					return 0;
				}
				success++;
				break;

			case 0xFF02: // Current Unit Is...
				unitid = datReqBase[datReqOffset++];
				if (PLAYER::numberOfCompletedUnitsOfType(unitid, player) != 0) {
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

			case 0xFF03: // Must have...
				unitid = datReqBase[datReqOffset++];
				success += PLAYER::numberOfCompletedUnitsOfType(unitid, player) + PLAYER::numberOfUnitsOfType(unitid, player);
				break;

			case 0xFF18: // Is transport
				if (unit->status & UnitStatus::IsHallucination) break;
				if (unit->id == UnitId::overlord && UpgradesSc->currentLevel[unit->playerId][UpgradeId::VentralSacs] == 0) break;
				if (units_dat::SpaceProvided[unit->id] == 0) break;
				success++;
				break;

			case 0xFF0F: // Must be researched
				if (techID == TechId::Burrowing && unit->id == UnitId::lurker) {
					success++;
					break;
				}
				if (scbw::hasTechResearched(player, techID) == false && scbw::isCheatEnabled(CheatFlags::MedievalMan) == false) {
					break;
				}
				success++;
				break;

			case 0xFF1C: // Is Hero and Enabled
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::Hero) == 0) break;
				success++;
				break;

			case 0xFF07: // Is not training or morphing
				if (unit->unitIsTrainingOrMorphing() || unit->building.techType != TechId::None) { // unitIsTrainingOrMorphing 00401500 0000006A
					*lastInternalErr = 5;
					return 0;
				}
				// fall through
			case 0xFF0A: // Is not upgrading
				if (unit->building.upgradeType != UpgradeId::None) {
					*lastInternalErr = 5;
					return 0;
				}
				success++;
				break;

			case 0xFF08: // Is not constructing add-on
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

			case 0xFF09: // Is not researching
				if (unit->building.techType != TechId::None) {
					*lastInternalErr = 5;
					return 0;
				}
				success++;
				break;

			case 0xFF05: // Is not lifted off
				if ((unit->status & UnitStatus::GroundedBuilding) == 0) {
					*lastInternalErr = 7;
					return 0;
				}
				success++;
				break;

			case 0xFF06: // Is lifted off
				if ((unit->status & UnitStatus::GroundedBuilding) != 0) {
					*lastInternalErr = 6;
					return 0;
				}
				success++;
				break;

			case 0xFF0D: // Does not have exit
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

			case 0xFF11: // Is not burrowed
				if ((unit->status & UnitStatus::Burrowed) != 0 && unit->pAI == NULL) {
					*lastInternalErr = 5;
					return 0;
				}
				success++;
				break;

			case 0xFF26: // Is burrowed
				if ((unit->status & UnitStatus::Burrowed) == 0){
				*lastInternalErr = 5;
				return 0;
			}
			success++;
			break;

			case 0xFF12: // Can attack -- duplicate of "Is lifted off"???
				if ((unit->status & UnitStatus::GroundedBuilding) != 0){
				*lastInternalErr = 19;
				return 0;
			}
			success++;
			break;

			case 0xFF13: // Can set rally point -- duplicate of "Is lifted off"???
				if ((unit->status & UnitStatus::GroundedBuilding) == 0){
				*lastInternalErr = 19;
				return 0;
			}
			success++;
			break;

			case 0xFF14: // Can move
				if ((unit->status & UnitStatus::GroundedBuilding) != 0){
				*lastInternalErr = 19;
				return 0;
			}
			if ((unit->id == UnitId::lurker || (unit->status & UnitStatus::Burrowed) == 0) && units_dat::RightClickAction[unit->id] == RightClickActions::NoCommand_AutoAttack) {
				*lastInternalErr = 19;
				return 0;
			}
			if (unit->getRightClickActionOrder() == RightClickActions::NoMove_NormalAttack) {
				// getRightClickActionOrder 004E5EA0 0000003A
				// GPTP\hooks\orders\base_orders\attack_orders.cpp(1908)
				// GPTP\hooks\right_click_CMDACT.cpp(509)
				*lastInternalErr = 19;
				return 0;
			}
			success++;
			break;

			case 0xFF15: // Has weapon
				if (unit->hasWeapon() == false) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case 0xFF1A: // Is subunit
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::Subunit) == 0) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case 0xFF17: // Is flying building
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::FlyingBuilding) == 0) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case 0xFF16: // Is worker
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::Worker) == 0) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case 0xFF19: // Is powerup
				if ((units_dat::BaseProperty[unit->id] & UnitProperty::NeutralAccessories) == 0) {
					*lastInternalErr = 19;
					return 0;
				}
				success++;
				break;

			case 0xFF1B: // Has spider mines
				if (unit->vulture.spiderMineCount == 0) {
					*lastInternalErr = 8;
					return 0;
				}
				success++;
				break;

			case 0xFF1D: // Can hold position
				if ((unit->id != UnitId::lurker || (unit->status & UnitStatus::Burrowed) == 0) && units_dat::RightClickAction[unit->id] == RightClickActions::NoCommand_AutoAttack && ((unit->status & UnitStatus::GroundedBuilding) == 0 || unit->isFactory() == false)) {
					*lastInternalErr = 8;
					return 0;
				}
				success++;
				break;

			case 0xFF1E: // Allow on hallucinations
				allowHalluc = true;
				success++;
				break;

			case 0xFF1F: // Upgrade Lv-1 Require...
				switch (scbw::getUpgradeLevel(player, techID)) {
				default:
					findopcode = 0xFF1F; // Upgrade Lv-1 Require...
					break;
				case 2:
					findopcode = 0xFF21; // Upgrade Lv-3 Require...
					break;
				case 1:
					findopcode = 0xFF20; // Upgrade Lv-2 Require...
				}
				while (opcode != findopcode) {
					opcode = datReqBase[datReqOffset++];
				}
				success++;
				break;

			case 0xFF22: // Grey
				*lastInternalErr = 21;
				return -1;

			case 0xFF23: // Blank
				*lastInternalErr = 23;
				return 0;

			default:
				// Any invalid opcodes will be read here as unit IDs
				unitid = datReqBase[datReqOffset++];
				success += PLAYER::numberOfCompletedUnitsOfType(player, unitid);
			}

			if (datReqBase[datReqOffset] == 0xFF01) { // Or
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