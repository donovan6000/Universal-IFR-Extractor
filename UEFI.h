#pragma once

// Header files
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

// Gobal definitions
enum { FORM_SET, FORM, CONDITION, OPTION, OTHER };

// Classes/structs
struct UEFI_HII_PACK_HEADER {
	uint32_t offset;
	uint32_t length;
	uint8_t type;
};

struct UEFI_IFR_STRING_PACK {
	UEFI_HII_PACK_HEADER header;
	uint32_t headerSize;
	uint32_t stringOffset;
	vector<uint16_t> languageWindow;
	string language;
	uint32_t structureOffset;
};

struct UEFI_IFR_FORM_SET_PACK {
	UEFI_HII_PACK_HEADER header;
	uint16_t titleString;
	uint32_t usingStringPackage;
	string title;
};

// Function prototypes
/*
Name: getUEFIStringPackages
Purpose: Gets UEFI string packages
*/
void getUEFIStringPackages(vector<UEFI_IFR_STRING_PACK> &stringPackages, const string &buffer);

/*
Name: displayUEFIStringPackages
Purpose: Displays UEFI string packages
*/
void displayUEFIStringPackages(const vector<UEFI_IFR_STRING_PACK> &stringPackages);

/*
Name: getUEFIStrings
Purpose: Gets strings from string package
*/
void getUEFIStrings(vector<UEFI_IFR_STRING_PACK> &stringPackages, vector<string> &strings, const string &buffer);

/*
Name: displayUEFIStrings
Purpose: Displays UEFI strings
*/
void displayUEFIStrings(const vector<string> &strings);

/*
Name: getUEFIFormSets
Purpose: Gets UEFI form sets
*/
void getUEFIFormSets(vector<UEFI_IFR_FORM_SET_PACK> &formSets, const string &buffer, const vector<UEFI_IFR_STRING_PACK> &stringPackages, const vector<string> &strings);

/*
Name: displayUEFIFormSets
Purpose: Displays UEFI form sets
*/
void displayUEFIFormSets(const vector<UEFI_IFR_FORM_SET_PACK> &formSets);

/*
Name: generateUEFIIFRDump
Purpose: Generates UEFI IFR dump
*/
void generateUEFIIFRDump(const string &outputFile, const vector<UEFI_IFR_STRING_PACK> &stringPackages, const vector<UEFI_IFR_FORM_SET_PACK> &formSets, const string &buffer, const vector<string> &strings);
