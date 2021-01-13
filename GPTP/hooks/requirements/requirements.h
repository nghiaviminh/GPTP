#include <SCBW/api.h>
#include "hooks/requirements/unit_requirements.h"
#include "hooks/requirements/order_requirements.h"
#include "hooks/requirements/upgrade_requirements.h"
#include "hooks/requirements/tech_requirements.h"
#include "hooks/requirements/research_requirements.h"

namespace customRequirements {

	u16* getOrderRequirementOverrides();
	u16* getOrderRequirementOpcodes();

	u16* getResearchRequirementOverrides();
	u16* getResearchRequirementOpcodes();

	u16* getTechRequirementOverrides();
	u16* getTechRequirementOpcodes();

	u16* getUnitRequirementOverrides();
	u16* getUnitRequirementOpcodes();

	u16* getUpgradeRequirementOverrides();
	u16* getUpgradeRequirementOpcodes();

	void parseRequirementOverrides();
}