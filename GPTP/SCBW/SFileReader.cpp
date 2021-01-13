#include "SFileReader.h"
#include <SCBW/api.h>
#include "types.h"
#include <Windows.h>
#include <DebugUtils.h>
#include <stdio.h>
#include <stdlib.h>
#include "hooks/requirements/requirements.h"

// TODO move all these out later
namespace {
	typedef void* HANDLE;
	typedef void* (__stdcall* SMemAlloc_type)(int size, char* filename, int line, int value);
	typedef u32(__stdcall* SMemFree_type)(void* ptr, char* filename, int line, int idk);
	typedef u32(__stdcall* SFileGetFileSize_type)(HANDLE hFile, LPDWORD lpFileSizeHigh);
	typedef u32(__stdcall* SFileCloseFile_type)(HANDLE hFile);
	typedef u32(__stdcall* SFileOpenFileEx_type)(HANDLE hArchive, char* filename, int scope, HANDLE* hFile);
	typedef u32(__stdcall* SFileReadFile_type)(HANDLE hFile, void* buffer, int toRead, int* read, int overlapped);

	SMemAlloc_type SMemAlloc = (SMemAlloc_type)0x0041006A;
	SMemFree_type SMemFree = (SMemFree_type)0x00410070;
	SFileGetFileSize_type SFileGetFileSize = (SFileGetFileSize_type)0x00410142;
	SFileCloseFile_type SFileCloseFile = (SFileCloseFile_type)0x004100B8;
	SFileOpenFileEx_type SFileOpenFileEx = (SFileOpenFileEx_type)0x004100C4;
	SFileReadFile_type SFileReadFile = (SFileReadFile_type)0x00410148;
}

// TODO break out into requirements.h and implementation. Requirements reading should not live in the generic SFileReader
bool parseRequirementsFile(char* path, u16 requirementOverridesIndex[UNIT_TYPE_COUNT], u16 requirementOpcodes[UNIT_TYPE_COUNT*10]) {
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


