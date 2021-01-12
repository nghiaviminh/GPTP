#pragma once
#include <SCBW/api.h>

namespace hooks {

	s8 UnitCreateAllowed(u16 unitId, CUnit* unit, int playerID);//0046E1C0	

	void injectUnitCreateAllowedWrapper();

}