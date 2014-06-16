// Header gaurd
#ifndef EFI_H
#define EFI_H


// Header files
#include <string>
#include <vector>
#include <cstdint>

using namespace std;


// Gobal definitions
#define EFI_IFR_FORM_OP '\x01'
#define EFI_IFR_SUBTITLE_OP '\x02'
#define EFI_IFR_TEXT_OP '\x03'
#define EFI_IFR_GRAPHIC_OP '\x04'
#define EFI_IFR_ONE_OF_OP '\x05'
#define EFI_IFR_CHECKBOX_OP '\x06'
#define EFI_IFR_NUMERIC_OP '\x07'
#define EFI_IFR_PASSWORD_OP '\x08'
#define EFI_IFR_ONE_OF_OPTION_OP '\x09'
#define EFI_IFR_SUPPRESS_IF_OP '\x0A'
#define EFI_IFR_END_FORM_OP '\x0B'
#define EFI_IFR_HIDDEN_OP '\x0C'
#define EFI_IFR_END_FORM_SET_OP '\x0D'
#define EFI_IFR_FORM_SET_OP '\x0E'
#define EFI_IFR_REF_OP '\x0F'
#define EFI_IFR_END_ONE_OF_OP '\x10'
#define EFI_IFR_END_OP EFI_IFR_END_ONE_OF_OP
#define EFI_IFR_INCONSISTENT_IF_OP '\x11'
#define EFI_IFR_EQ_ID_VAL_OP '\x12'
#define EFI_IFR_EQ_ID_ID_OP '\x13'
#define EFI_IFR_EQ_ID_LIST_OP '\x14'
#define EFI_IFR_AND_OP '\x15'
#define EFI_IFR_OR_OP '\x16'
#define EFI_IFR_NOT_OP '\x17'
#define EFI_IFR_END_IF_OP '\x18'
#define EFI_IFR_GRAYOUT_IF_OP '\x19'
#define EFI_IFR_DATE_OP '\x1A'
#define EFI_IFR_TIME_OP '\x1B'
#define EFI_IFR_STRING_OP '\x1C'
#define EFI_IFR_LABEL_OP '\x1D'
#define EFI_IFR_SAVE_DEFAULTS_OP '\x1E'
#define EFI_IFR_RESTORE_DEFAULTS_OP '\x1F'
#define EFI_IFR_BANNER_OP '\x20'
#define EFI_IFR_INVENTORY_OP '\x21'
#define EFI_IFR_EQ_VAR_VAL_OP '\x22'
#define EFI_IFR_ORDERED_LIST_OP '\x23'
#define EFI_IFR_VARSTORE_OP '\x24'
#define EFI_IFR_VARSTORE_SELECT_OP '\x25'
#define EFI_IFR_VARSTORE_SELECT_PAIR_OP '\x26'
#define EFI_IFR_TRUE_OP '\x27'
#define EFI_IFR_FALSE_OP '\x28'
#define EFI_IFR_GT_OP '\x29'
#define EFI_IFR_GE_OP '\x2A'
#define EFI_IFR_OEM_DEFINED_OP '\x2B'
#define EFI_IFR_LAST_OPCODE EFI_IFR_OEM_DEFINED_OP
#define EFI_IFR_OEM_OP '\xFE'
#define EFI_IFR_NV_ACCESS_COMMAND '\xFF'


// Classes/structs
struct EFI_HII_PACK_HEADER {
	uint32_t offset;
	uint32_t length;
	uint16_t type;
};

struct EFI_IFR_STRING_PACK {
	EFI_HII_PACK_HEADER header;
	uint32_t numberOfStrings;
	uint32_t attributes;
	string language;
	uint32_t structureOffset;
};

struct EFI_IFR_FORM_SET_PACK {
	EFI_HII_PACK_HEADER header;
	uint16_t titleString;
	uint32_t usingStringPackage;
	string title;
};

struct EFI_IFR_OP_HEADER {
	uint32_t offset;
	uint8_t opcode;
	uint8_t length;
};

struct EFI_IFR_FORM_SET {
	EFI_IFR_OP_HEADER header;
	string guid;
	uint16_t titleString;
	uint16_t helpString;
	uint64_t callbackHandle;
	uint16_t mainClass;
	uint16_t subClass;
	uint16_t nvDataSize;
};

struct EFI_IFR_FORM {
	EFI_IFR_OP_HEADER header;
	uint16_t formId;
	uint16_t titleString;
};

struct EFI_IFR_LABEL {
	EFI_IFR_OP_HEADER header;
	uint16_t labelId;
};

struct EFI_IFR_SUBTITLE {
	EFI_IFR_OP_HEADER header;
	uint16_t subtitleString;
};

struct EFI_IFR_TEXT {
	EFI_IFR_OP_HEADER header;
	uint16_t helpString;
	uint16_t primaryTextString;
	uint16_t secondaryTextString;
	uint8_t flags;
	uint16_t key;
};

struct EFI_IFR_REF {
	EFI_IFR_OP_HEADER header;
	uint16_t formId;
	uint16_t promptString;
	uint16_t helpString;
	uint8_t flags;
	uint16_t key;
};

struct EFI_IFR_END_FORM {
	EFI_IFR_OP_HEADER header;
};

struct EFI_IFR_END_FORM_SET {
	EFI_IFR_OP_HEADER header;
};

struct EFI_IFR_ONE_OF {
	EFI_IFR_OP_HEADER header;
	uint16_t questionId;
	uint8_t width;
	uint16_t promptString;
	uint16_t helpString;
};

struct EFI_IFR_CHECKBOX {
	EFI_IFR_OP_HEADER header;
	uint16_t questionId;
	uint8_t width;
	uint16_t promptString;
	uint16_t helpString;
	uint8_t flags;
	uint16_t key;
};

struct  EFI_IFR_ORDERED_LIST {
	EFI_IFR_OP_HEADER header;
	uint16_t questionId;
	uint8_t maximumEntries;
	uint16_t promptString;
	uint16_t helpString;
};

struct EFI_IFR_CHECK_BOX {
	EFI_IFR_OP_HEADER header;
	uint16_t option;
	uint16_t value;
	uint8_t flags;
	uint16_t key;
};

struct EFI_IFR_ONE_OF_OPTION {
	EFI_IFR_OP_HEADER header;
	uint16_t optionString;
	uint16_t value;
	uint8_t flags;
	uint16_t key;
};

struct EFI_IFR_NUMERIC {
	EFI_IFR_OP_HEADER header;
	uint16_t questionId;
	uint8_t width;
	uint16_t promptString;
	uint16_t helpString;
	uint8_t flags;
	uint16_t key;
	uint16_t minimum;
	uint16_t maximum;
	uint16_t step;
	uint16_t defaultValue;
};

struct EFI_IFR_TIME {
	EFI_IFR_NUMERIC hour;
	EFI_IFR_NUMERIC minute;
	EFI_IFR_NUMERIC second;
};

struct EFI_IFR_DATE {
	EFI_IFR_NUMERIC year;
	EFI_IFR_NUMERIC month;
	EFI_IFR_NUMERIC day;
};

struct EFI_IFR_PASSWORD {
	EFI_IFR_OP_HEADER header;
	uint16_t questionId;
	uint8_t width;
	uint16_t promptString;
	uint16_t helpString;
	uint8_t flags;
	uint16_t key;
	uint8_t minimumSize;
	uint8_t maximumSize;
	uint16_t encoding;
};

struct EFI_IFR_STRING {
	EFI_IFR_OP_HEADER header;
	uint16_t questionId;
	uint8_t width;
	uint16_t promptString;
	uint16_t helpString;
	uint8_t flags;
	uint16_t key;
	uint8_t minimumSize;
	uint8_t maximumSize;
};

struct EFI_IFR_END_ONE_OF {
	EFI_IFR_OP_HEADER header;
};

struct EFI_IFR_HIDDEN {
	EFI_IFR_OP_HEADER header;
	uint16_t value;
	uint16_t key;
};

struct EFI_IFR_SUPPRESS {
	EFI_IFR_OP_HEADER header;
	uint8_t flags;
};

struct EFI_IFR_GRAYOUT {
	EFI_IFR_OP_HEADER header;
	uint8_t flags;
};

struct EFI_IFR_INCONSISTENT {
	EFI_IFR_OP_HEADER header;
	uint16_t popup;
	uint8_t flags;
};

struct EFI_IFR_EQ_ID_VAL {
	EFI_IFR_OP_HEADER header;
	uint16_t questionId;
	uint8_t width;
	uint16_t value;
};

struct EFI_IFR_EQ_ID_LIST {
	EFI_IFR_OP_HEADER header;
	uint16_t questionId;
	uint8_t width;
	uint16_t listLength;
	vector<uint16_t> valueList;
};

struct EFI_IFR_EQ_ID_ID {
	EFI_IFR_OP_HEADER header;
	uint16_t primaryQuestionId;
	uint8_t width;
	uint16_t secondaryQuestionId;
};

struct EFI_IFR_EQ_VAR_VAL {
	EFI_IFR_OP_HEADER header;
	uint16_t variableId;
	uint16_t value;
};

struct EFI_IFR_AND {
	EFI_IFR_OP_HEADER header;
};

struct EFI_IFR_OR {
	EFI_IFR_OP_HEADER header;
};

struct EFI_IFR_NOT {
	EFI_IFR_OP_HEADER header;
};

struct EFI_IFR_END_IF {
	EFI_IFR_OP_HEADER header;
};

struct EFI_IFR_SAVE_DEFAULTS {
	EFI_IFR_OP_HEADER header;
	uint16_t formId;
	uint16_t promptString;
	uint16_t helpString;
	uint8_t flags;
	uint16_t key;
};

struct EFI_IFR_RESTORE_DEFAULTS {
	EFI_IFR_OP_HEADER header;
	uint16_t formId;
	uint16_t promptString;
	uint16_t helpString;
	uint8_t flags;
	uint16_t key;
};

struct EFI_IFR_INVENTORY {
	EFI_IFR_OP_HEADER header;
	uint16_t helpString;
	uint16_t primaryTextString;
	uint16_t secondaryTextString;
};

struct EFI_IFR_VARSTORE {
	EFI_IFR_OP_HEADER header;
	string guid;
	uint16_t varId;
	uint16_t size;
};

struct EFI_IFR_VARSTORE_SELECT {
	EFI_IFR_OP_HEADER header;
	uint16_t varId;
};

struct EFI_IFR_VARSTORE_SELECT_PAIR {
	EFI_IFR_OP_HEADER header;
	uint16_t primaryVarId;
	uint16_t secondaryVarId;
};

struct EFI_IFR_TRUE {
	EFI_IFR_OP_HEADER header;
};

struct EFI_IFR_FALSE {
	EFI_IFR_OP_HEADER header;
};

struct EFI_IFR_GT {
	EFI_IFR_OP_HEADER header;
};

struct EFI_IFR_GE {
	EFI_IFR_OP_HEADER header;
};

struct EFI_IFR_BANNER {
	EFI_IFR_OP_HEADER header;
	uint16_t titleString;
	uint16_t lineNumber;
	uint8_t alignment;
};

struct EFI_IFR_NV_DATA {
	EFI_IFR_OP_HEADER header;
	uint16_t questionId;
	uint8_t storageWidth;
	vector<char> data;
};


// Function prototypes
/*
Name: getEFIStringPackages
Purpose: Gets EFI string packages
*/
void getEFIStringPackages(vector<EFI_IFR_STRING_PACK> &stringPackages, const string &buffer);

/*
Name: displayEFIStringPackages
Purpose: Displays EFI string packages
*/
void displayEFIStringPackages(const vector<EFI_IFR_STRING_PACK> &stringPackages);

/*
Name: getEFIStrings
Purpose: Gets strings from string package
*/
void getEFIStrings(vector<EFI_IFR_STRING_PACK> &stringPackages, vector<string> &strings, const string &buffer);

/*
Name: displayEFIStrings
Purpose: Displays EFI strings
*/
void displayEFIStrings(const vector<string> &strings);

/*
Name: getEFIFormSets
Purpose: Gets EFI form sets
*/
void getEFIFormSets(vector<EFI_IFR_FORM_SET_PACK> &formSets, const string &buffer, const vector<EFI_IFR_STRING_PACK> &stringPackages, const vector<string> &strings);

/*
Name: displayEFIFormSets
Purpose: Displays EFI form sets
*/
void displayEFIFormSets(const vector<EFI_IFR_FORM_SET_PACK> &formSets);

/*
Name: generateEFIIFRDump
Purpose: Generates EFI IFR dump
*/
void generateEFIIFRDump(const string &outputFile, const vector<EFI_IFR_STRING_PACK> &stringPackages, const vector<EFI_IFR_FORM_SET_PACK> &formSets, const string &buffer, const vector<string> &strings);

/*
Name: endEFICondition
Purpose: Returns if opcode ends a conditional statement
*/
bool endEFICondition(const string &buffer, uint32_t j);


#endif
