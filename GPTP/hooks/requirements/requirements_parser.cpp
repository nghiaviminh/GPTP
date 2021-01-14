#include "requirements_parser.h"
#include <SCBW/SFileReader.h>
#include <DebugUtils.h>


namespace {
	bool parseRequirementsFile(char* path, u16 requirementOverridesIndex[UNIT_TYPE_COUNT], u16 requirementOpcodes[UNIT_TYPE_COUNT * 10]) {
		HANDLE fileHandle;
		if (!SFileOpenFileEx(0, path, 0, &fileHandle))
		{
			DebugOut("File not found: %s", path);
			return false;
		}
		int fileSize = SFileGetFileSize(fileHandle, 0);
		if (fileSize == -1 || !fileSize)
		{
			DebugOut("File is empty: %s", path);
			return false;
		}
		char* filePointer = (char*)SMemAlloc(fileSize, "logfilename", 0, 0);
		int bytesRead;
		if (!SFileReadFile(fileHandle, filePointer, fileSize, &bytesRead, 0) || bytesRead != fileSize)
		{
			DebugOut("File reading error %s (%d)", path, GetLastError());
			return false;
		}

		char* delimiter = "\r\n";
		int line = 1;  // To ensure no indexes point to line 0 which is undefined
		int opcodeSetCount = 0;
		int requirementSet = 0;
		//u16 requirementOverridesIndex[UNIT_TYPE_COUNT] = { 0 };
		//u16 requirementOpcodes[UNIT_TYPE_COUNT * 10] = { 0 };
		char* token = strtok(filePointer, delimiter);
		while (token != NULL) {
			// If line ends with ":", treat it as an opcode header and add the 
			// id to the requirementOverridesIndex lookup table.
			u16 value = atoi(token);
			if (strchr(token, ':') != NULL) {
				requirementOverridesIndex[value] = line + 1; //Because otherwise it gets stuck on having the unit
			}
			if (value != 0) {
				requirementOpcodes[line] = value;
			}
			line++;
			token = strtok(NULL, delimiter);
		}

		BOOL result = SFileCloseFile(fileHandle);
		// read filePointer
		// finally SMemFree when done with the buffer
		return result;
	}
}

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
