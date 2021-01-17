#include "buttonsets_parser.h"
#include <SCBW/SFileReader.h>
#include <DebugUtils.h>

char* strtok_r(char* str, const char* delim, char** save)
{
	char* res, * last;

	if (!save)
		return strtok(str, delim);
	if (!str && !(str = *save))
		return NULL;
	last = str + strlen(str);
	if ((*save = res = strtok(str, delim)))
	{
		*save += strlen(res);
		if (*save < last)
			(*save)++;
		else
			*save = NULL;
	}
	return res;
}



namespace {
	bool parseButtonSetOverridesFile(char* path, BUTTON_SET buttonSetOverrides[], BUTTON buttons[]) {
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
		char* lineToken = NULL;
		char* columnToken = NULL;
		char* lineDelimiter = "\r\n";
		char* columnDelimiter = ",";
		char* line;
		char* column;
		int lineCount = 0;
		int lastButtonSetId = 0;

		line = strtok_s(filePointer, lineDelimiter, &lineToken);
		while (line != NULL) {
			u16 value = atoi(line);
			if (strchr(line, ':') != NULL) {
				lastButtonSetId = value;
				buttonSetTable[lastButtonSetId].buttonsInSet = 0;
				buttonSetTable[lastButtonSetId].firstButton = nullptr;
				customButtonSets::getOverriddenButtons()[lastButtonSetId] = true;
			} else if (strchr(line, ',') != NULL) {
				BUTTON button;
				int c = 0;
				column = strtok_s(line, columnDelimiter, &columnToken);
				while (column != NULL) {
					switch (c) {
						case 0:	button.position = atoi(column); break;
						case 1:	button.iconID = atoi(column); break;
						case 2:	button.reqFunc = (REQ_FUNC*)strtol(column, NULL, 16); break;
						case 3:	button.actFunc = (ACT_FUNC*)strtol(column, NULL, 16); break;
						case 4:	button.reqVar = atoi(column); break;
						case 5:	button.actVar = atoi(column); break;
						case 6:	button.reqStringID = atoi(column); break;
						case 7:	button.actStringID = atoi(column); break;
					}
					column = strtok_s(NULL, columnDelimiter, &columnToken);
					c++;
				}
				
				if (buttonSetTable[lastButtonSetId].firstButton == nullptr) {
					customButtonSets::getOverriddenButtonStartIndex()[lastButtonSetId] = lineCount;
					buttonSetTable[lastButtonSetId].firstButton = &button;
					customButtonSets::getFirstButton()[lastButtonSetId] = button;
				}
				buttonSetTable[lastButtonSetId].buttonsInSet++;
				buttons[lineCount] = button;
			}
			lineCount++;
			line = strtok_s(NULL, lineDelimiter, &lineToken);
		}

		BOOL result = SFileCloseFile(fileHandle);
		SMemFree(filePointer, "", 0, 0);
		return result;
	}
}

namespace customButtonSets {
	const int BUTTON_SET_LENGTH = UNIT_TYPE_COUNT * 2;
	const int BUTTON_COUNT = BUTTON_SET_LENGTH * 24;

	BUTTON_SET* getButtonSetOverrides() {
		
		static BUTTON_SET* buttonSetOverrides = new BUTTON_SET[BUTTON_SET_LENGTH];
		return buttonSetOverrides;
	}

	BUTTON* getButtons() {
		static BUTTON *buttons = new BUTTON[BUTTON_COUNT];
		return buttons;
	}

	BUTTON* getFirstButton() {
		static BUTTON* buttons = new BUTTON[BUTTON_SET_LENGTH];
		return buttons;
	}

	bool * getOverriddenButtons() {
		static bool buttons[BUTTON_SET_LENGTH] = { false };
		return buttons;
	}

	int* getOverriddenButtonStartIndex() {
		static int buttons[BUTTON_SET_LENGTH] = { 0 };
		return buttons;
	}

	void parseButtonSetOverrides() {
		static bool initialized = false;
		if (!initialized) {
			initialized = true;
			parseButtonSetOverridesFile("datreq\\buttons.txt", getButtonSetOverrides(), getButtons());
		}
	}
}