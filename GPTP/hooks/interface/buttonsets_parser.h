#include <SCBW/api.h>

namespace customButtonSets {

	BUTTON_SET* getButtonSetOverrides();
	BUTTON* getButtons();
	BUTTON* getFirstButton();

	bool * getOverriddenButtons();
	int* getOverriddenButtonStartIndex();

	void parseButtonSetOverrides();
}