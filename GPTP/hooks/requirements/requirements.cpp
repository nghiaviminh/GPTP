#include "requirements.h"
#include <SCBW/SFileReader.h>


namespace customRequirements {

	u16* getUnitRequirementOverrides()
	{
		static u16 unitRequirementOverridesIndex[UNIT_TYPE_COUNT] = { 0 };
		return unitRequirementOverridesIndex;
	}
	u16* getUnitRequirementOpcodes()
	{
		static u16 unitRequirementOpcodes[UNIT_TYPE_COUNT*10] = { 0 };
		return unitRequirementOpcodes;
	}

	void parseUnitRequirementOverrides() {
		static bool initialized = false;
		if (!initialized) {
			initialized = true;
			char* path = "rez\\unit_requirements.txt";
			parseRequirementsFile(path, customRequirements::getUnitRequirementOverrides(), customRequirements::getUnitRequirementOpcodes());
		}
	}
}
