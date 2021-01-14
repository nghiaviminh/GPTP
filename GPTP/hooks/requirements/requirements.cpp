#include "requirements.h"
#include <SCBW/SFileReader.h>

namespace customRequirements {
	const int ENTRY_OPCODE_LIMIT = 32;

	u16* getOrderRequirementOverrides() {
		static u16 orderRequirementOverridesIndex[ORDER_TYPE_COUNT] = { 0 };
		return orderRequirementOverridesIndex;
	}
	u16* getOrderRequirementOpcodes() {
		static u16 orderRequirementOpcodes[ORDER_TYPE_COUNT * ENTRY_OPCODE_LIMIT] = { 0 };
		return orderRequirementOpcodes;
	}

	u16* getResearchRequirementOverrides() {
		static u16 researchRequirementOverridesIndex[TECH_TYPE_COUNT] = { 0 };
		return researchRequirementOverridesIndex;
	}
	u16* getResearchRequirementOpcodes() {
		static u16 researchRequirementOpcodes[TECH_TYPE_COUNT * ENTRY_OPCODE_LIMIT] = { 0 };
		return researchRequirementOpcodes;
	}

	u16* getTechRequirementOverrides() {
		static u16 techRequirementOverridesIndex[TECH_TYPE_COUNT] = { 0 };
		return techRequirementOverridesIndex;
	}
	u16* getTechRequirementOpcodes() {
		static u16 techRequirementOpcodes[TECH_TYPE_COUNT * ENTRY_OPCODE_LIMIT] = { 0 };
		return techRequirementOpcodes;
	}

	u16* getUnitRequirementOverrides()
	{
		static u16 unitRequirementOverridesIndex[UNIT_TYPE_COUNT] = { 0 };
		return unitRequirementOverridesIndex;
	}
	u16* getUnitRequirementOpcodes()
	{
		static u16 unitRequirementOpcodes[UNIT_TYPE_COUNT * ENTRY_OPCODE_LIMIT] = { 0 };
		return unitRequirementOpcodes;
	}

	u16* getUpgradeRequirementOverrides() {
		static u16 upgradeRequirementOverridesIndex[UPGRADE_TYPE_COUNT] = { 0 };
		return upgradeRequirementOverridesIndex;
	}
	u16* getUpgradeRequirementOpcodes() {
		static u16 upgradeRequirementOpcodes[UPGRADE_TYPE_COUNT * ENTRY_OPCODE_LIMIT] = { 0 };
		return upgradeRequirementOpcodes;
	}

	void parseRequirementOverrides() {
		static bool initialized = false;
		if (!initialized) {
			initialized = true;
			parseRequirementsFile("rez\\orders.datreq", customRequirements::getOrderRequirementOverrides(), customRequirements::getOrderRequirementOpcodes());
			parseRequirementsFile("rez\\research.datreq", customRequirements::getResearchRequirementOverrides(), customRequirements::getResearchRequirementOpcodes());
			parseRequirementsFile("rez\\tech.datreq", customRequirements::getTechRequirementOverrides(), customRequirements::getTechRequirementOpcodes());
			parseRequirementsFile("rez\\units.datreq", customRequirements::getUnitRequirementOverrides(), customRequirements::getUnitRequirementOpcodes());
			parseRequirementsFile("rez\\upgrades.datreq", customRequirements::getUpgradeRequirementOverrides(), customRequirements::getUpgradeRequirementOpcodes());
		}
	}
}
