#include <SCBW/api.h>
#include "hooks/requirements/unit_requirements.h"
#include "hooks/requirements/order_requirements.h"
#include "hooks/requirements/upgrade_requirements.h"
#include "hooks/requirements/tech_requirements.h"
#include "hooks/requirements/research_requirements.h"



namespace customRequirements {
	u16* getUnitRequirementOverrides();
	u16* getUnitRequirementOpcodes();
	void parseUnitRequirementOverrides();
}