// Header gaurd
#ifndef UEFI_H
#define UEFI_H


// Header files
#include <string>
#include <vector>
#include <cstdint>

using namespace std;


// Gobal definitions
enum { FORM_SET, FORM, CONDITION, OPTION, OTHER };

#define UEFI_IFR_FORM_OP '\x01'
#define UEFI_IFR_SUBTITLE_OP '\x02'
#define UEFI_IFR_TEXT_OP '\x03'
#define UEFI_IFR_IMAGE_OP '\x04'
#define UEFI_IFR_ONE_OF_OP '\x05'
#define UEFI_IFR_CHECKBOX_OP '\x06'
#define UEFI_IFR_NUMERIC_OP '\x07'
#define UEFI_IFR_PASSWORD_OP '\x08'
#define UEFI_IFR_ONE_OF_OPTION_OP '\x09'
#define UEFI_IFR_SUPPRESS_IF_OP '\x0A'
#define UEFI_IFR_LOCKED_OP '\x0B'
#define UEFI_IFR_ACTION_OP '\x0C'
#define UEFI_IFR_RESET_BUTTON_OP '\x0D'
#define UEFI_IFR_FORM_SET_OP '\x0E'
#define UEFI_IFR_REF_OP '\x0F'
#define UEFI_IFR_NO_SUBMIT_IF_OP '\x10'
#define UEFI_IFR_INCONSISTENT_IF_OP '\x11'
#define UEFI_IFR_EQ_ID_VAL_OP '\x12'
#define UEFI_IFR_EQ_ID_ID_OP '\x13'
#define UEFI_IFR_EQ_ID_LIST_OP '\x14'
#define UEFI_IFR_AND_OP '\x15'
#define UEFI_IFR_OR_OP '\x16'
#define UEFI_IFR_NOT_OP '\x17'
#define UEFI_IFR_RULE_OP '\x18'
#define UEFI_IFR_GRAY_OUT_IF_OP '\x19'
#define UEFI_IFR_DATE_OP '\x1A'
#define UEFI_IFR_TIME_OP '\x1B'
#define UEFI_IFR_STRING_OP '\x1C'
#define UEFI_IFR_REFRESH_OP '\x1D'
#define UEFI_IFR_DISABLE_IF_OP '\x1E'
#define UEFI_IFR_ANIMATION_OP '\x1F'
#define UEFI_IFR_TO_LOWER_OP '\x20'
#define UEFI_IFR_TO_UPPER_OP '\x21'
#define UEFI_IFR_MAP_OP '\x22'
#define UEFI_IFR_ORDERED_LIST_OP '\x23'
#define UEFI_IFR_VARSTORE_OP '\x24'
#define UEFI_IFR_VARSTORE_NAME_VALUE_OP '\x25'
#define UEFI_IFR_VARSTORE_EFI_OP '\x26'
#define UEFI_IFR_VARSTORE_DEVICE_OP '\x27'
#define UEFI_IFR_VERSION_OP '\x28'
#define UEFI_IFR_END_OP '\x29'
#define UEFI_IFR_MATCH_OP '\x2A'
#define UEFI_IFR_GET_OP '\x2B'
#define UEFI_IFR_SET_OP '\x2C'
#define UEFI_IFR_READ_OP '\x2D'
#define UEFI_IFR_WRITE_OP '\x2E'
#define UEFI_IFR_EQUAL_OP '\x2F'
#define UEFI_IFR_NOT_EQUAL_OP '\x30'
#define UEFI_IFR_GREATER_THAN_OP '\x31'
#define UEFI_IFR_GREATER_EQUAL_OP '\x32'
#define UEFI_IFR_LESS_THAN_OP '\x33'
#define UEFI_IFR_LESS_EQUAL_OP '\x34'
#define UEFI_IFR_BITWISE_AND_OP '\x35'
#define UEFI_IFR_BITWISE_OR_OP '\x36'
#define UEFI_IFR_BITWISE_NOT_OP '\x37'
#define UEFI_IFR_SHIFT_LEFT_OP '\x38'
#define UEFI_IFR_SHIFT_RIGHT_OP '\x39'
#define UEFI_IFR_ADD_OP '\x3A'
#define UEFI_IFR_SUBTRACT_OP '\x3B'
#define UEFI_IFR_MULTIPLY_OP '\x3C'
#define UEFI_IFR_DIVIDE_OP '\x3D'
#define UEFI_IFR_MODULO_OP '\x3E'
#define UEFI_IFR_RULE_REF_OP '\x3F'
#define UEFI_IFR_QUESTION_REF1_OP '\x40'
#define UEFI_IFR_QUESTION_REF2_OP '\x41'
#define UEFI_IFR_UINT8_OP '\x42'
#define UEFI_IFR_UINT16_OP '\x43'
#define UEFI_IFR_UINT32_OP '\x44'
#define UEFI_IFR_UINT64_OP '\x45'
#define UEFI_IFR_TRUE_OP '\x46'
#define UEFI_IFR_FALSE_OP '\x47'
#define UEFI_IFR_TO_UINT_OP '\x48'
#define UEFI_IFR_TO_STRING_OP '\x49'
#define UEFI_IFR_TO_BOOLEAN_OP '\x4A'
#define UEFI_IFR_MID_OP '\x4B'
#define UEFI_IFR_FIND_OP '\x4C'
#define UEFI_IFR_TOKEN_OP '\x4D'
#define UEFI_IFR_STRING_REF1_OP '\x4E'
#define UEFI_IFR_STRING_REF2_OP '\x4F'
#define UEFI_IFR_CONDITIONAL_OP '\x50'
#define UEFI_IFR_QUESTION_REF3_OP '\x51'
#define UEFI_IFR_ZERO_OP '\x52'
#define UEFI_IFR_ONE_OP '\x53'
#define UEFI_IFR_ONES_OP '\x54'
#define UEFI_IFR_UNDEFINED_OP '\x55'
#define UEFI_IFR_LENGTH_OP '\x56'
#define UEFI_IFR_DUP_OP '\x57'
#define UEFI_IFR_THIS_OP '\x58'
#define UEFI_IFR_SPAN_OP '\x59'
#define UEFI_IFR_VALUE_OP '\x5A'
#define UEFI_IFR_DEFAULT_OP '\x5B'
#define UEFI_IFR_DEFAULTSTORE_OP '\x5C'
#define UEFI_IFR_FORM_MAP_OP '\x5D'
#define UEFI_IFR_CATENATE_OP '\x5E'
#define UEFI_IFR_GUID_OP '\x5F'
#define UEFI_IFR_SECURITY_OP '\x60'


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

struct UEFI_IFR_OP_HEADER {
	uint32_t offset;
	uint8_t opcode;
	uint8_t length;
	uint8_t scope;
};

struct UEFI_IFR_STATEMENT_HEADER {
	uint16_t promptString;
	uint16_t helpString;
};

struct UEFI_IFR_QUESTION_HEADER {
	UEFI_IFR_STATEMENT_HEADER statement;
	uint16_t questionId;
	uint16_t varStoreId;
	uint16_t varOffset;
	uint8_t flags;
};

union MINMAXSTEP_DATA {
	uint64_t minimumValue;
	uint64_t maximumValue;
	uint64_t step;
};

struct UEFI_IFR_FORM_SET {
	UEFI_IFR_OP_HEADER header;
	string guid;
	uint16_t titleString;
	uint16_t helpString;
	uint8_t flags;
	string classGuid;
};

struct UEFI_IFR_FORM {
	UEFI_IFR_OP_HEADER header;
	uint16_t formId;
	uint16_t titleString;
};

struct UEFI_IFR_VARSTORE {
	UEFI_IFR_OP_HEADER header;
	string guid;
	uint16_t varStoreId;
	uint16_t size;
	string name;
};

struct UEFI_IFR_VARSTORE_EFI {
	UEFI_IFR_OP_HEADER header;
	uint16_t varStoreId;
	string guid;
	uint32_t attributes;
};

struct UEFI_IFR_VARSTORE_NAME_VALUE {
	UEFI_IFR_OP_HEADER header;
	uint16_t varStoreId;
	string guid;
};

struct UEFI_IFR_END {
	UEFI_IFR_OP_HEADER header;
	uint8_t condition;
};

struct UEFI_IFR_IMAGE {
	UEFI_IFR_OP_HEADER header;
	uint16_t imageId;
};

struct UEFI_IFR_LOCKED {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_RULE {
	UEFI_IFR_OP_HEADER header;
	uint8_t ruleId;
};

struct UEFI_IFR_DEFAULT {
	UEFI_IFR_OP_HEADER header;
	uint16_t defaultId;
	uint8_t type;
	uint64_t value;
};

struct UEFI_IFR_DEFAULTSTORE {
	UEFI_IFR_OP_HEADER header;
	uint16_t defaultNameString;
	uint16_t defaultId;
};

struct UEFI_IFR_VALUE {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_SUBTITLE {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_STATEMENT_HEADER statement;
	uint8_t flags;
};

struct UEFI_IFR_CHECKBOX {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_QUESTION_HEADER question;
	uint8_t flags;
};

struct UEFI_IFR_TEXT {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_STATEMENT_HEADER statement;
	uint16_t secondaryTextString;
};

struct UEFI_IFR_REF {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_QUESTION_HEADER question;
	uint16_t formId;
};

struct UEFI_IFR_RESET_BUTTON {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_STATEMENT_HEADER statement;
	uint16_t defaultId;
};

struct UEFI_IFR_ACTION {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_QUESTION_HEADER question;
	uint16_t questionConfigString;
};

struct UEFI_IFR_DATE {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_QUESTION_HEADER question;
	uint8_t flags;
};

struct UEFI_IFR_NUMERIC {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_QUESTION_HEADER question;
	uint8_t flags;
	MINMAXSTEP_DATA data;
};

struct UEFI_IFR_ONE_OF {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_QUESTION_HEADER question;
	uint8_t flags;
	MINMAXSTEP_DATA data;
};

struct UEFI_IFR_STRING {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_QUESTION_HEADER question;
	uint8_t minimumSize;
	uint8_t maximumSize;
	uint8_t flags;
};

struct UEFI_IFR_PASSWORD {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_QUESTION_HEADER question;
	uint16_t minimumSize;
	uint16_t maximumSize;
};

struct UEFI_IFR_ORDERED_LIST {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_QUESTION_HEADER question;
	uint8_t minimumContainers;
	uint8_t flags;
};

struct UEFI_IFR_TIME {
	UEFI_IFR_OP_HEADER header;
	UEFI_IFR_QUESTION_HEADER question;
	uint8_t flags;
};

struct UEFI_IFR_DISABLE_IF {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_SUPPRESS_IF {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_GRAY_OUT_IF {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_INCONSISTENT_IF {
	UEFI_IFR_OP_HEADER header;
	uint16_t errorString;
};

struct UEFI_IFR_NO_SUBMIT_IF {
	UEFI_IFR_OP_HEADER header;
	uint16_t errorString;
};

struct UEFI_IFR_REFRESH {
	UEFI_IFR_OP_HEADER header;
	uint8_t refreshInterval;
};

struct UEFI_IFR_VARSTORE_DEVICE {
	UEFI_IFR_OP_HEADER header;
	uint16_t devicePathString;
};

struct UEFI_IFR_ONE_OF_OPTION {
	UEFI_IFR_OP_HEADER header;
	uint16_t optionString;
	uint8_t flags;
	uint8_t type;
	uint64_t value;
};

struct UEFI_IFR_GUID {
	UEFI_IFR_OP_HEADER header;
	string guid;
};

struct UEFI_IFR_DUP {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_EQ_ID_ID {
	UEFI_IFR_OP_HEADER header;
	uint16_t primaryQuestionId;
	uint16_t secondaryQuestionId;
};

struct UEFI_IFR_EQ_ID_VAL {
	UEFI_IFR_OP_HEADER header;
	uint16_t questionId;
	uint16_t value;
};

struct UEFI_IFR_EQ_ID_LIST {
	UEFI_IFR_OP_HEADER header;
	uint16_t questionId;
	uint16_t listLength;
	vector<uint16_t> valueList;
};

struct UEFI_IFR_UINT8 {
	UEFI_IFR_OP_HEADER header;
	uint8_t value;
};

struct UEFI_IFR_UINT16 {
	UEFI_IFR_OP_HEADER header;
	uint16_t value;
};

struct UEFI_IFR_UINT32 {
	UEFI_IFR_OP_HEADER header;
	uint32_t value;
};

struct UEFI_IFR_UINT64 {
	UEFI_IFR_OP_HEADER header;
	uint64_t value;
};

struct UEFI_IFR_QUESTION_REF1 {
	UEFI_IFR_OP_HEADER header;
	uint16_t questionId;
};

struct UEFI_IFR_QUESTION_REF2 {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_QUESTION_REF3 {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_RULE_REF {
	UEFI_IFR_OP_HEADER header;
	uint8_t ruleId;
};

struct UEFI_IFR_STRING_REF1 {
	UEFI_IFR_OP_HEADER header;
	uint16_t stringId;
};

struct UEFI_IFR_STRING_REF2 {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_THIS {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_TRUE {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_FALSE {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_ONE {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_ONES {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_ZERO {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_UNDEFINED {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_VERSION {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_LENGTH {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_NOT {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_BITWISE_NOT {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_TO_BOOLEAN {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_TO_STRING {
	UEFI_IFR_OP_HEADER header;
	uint8_t format;
};

struct UEFI_IFR_TO_UINT {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_TO_UPPER {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_TO_LOWER {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_ADD {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_AND {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_BITWISE_AND {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_BITWISE_OR {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_CATENATE {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_DIVIDE {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_EQUAL {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_GREATER_EQUAL {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_GREATER_THAN {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_LESS_EQUAL {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_LESS_THAN {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_MATCH {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_MULTIPLY {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_MODULO {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_NOT_EQUAL {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_OR {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_SHIFT_LEFT {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_SHIFT_RIGHT {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_SUBTRACT {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_CONDITIONAL {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_FIND {
	UEFI_IFR_OP_HEADER header;
	uint8_t format;
};

struct UEFI_IFR_MID {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_TOKEN {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_SPAN {
	UEFI_IFR_OP_HEADER header;
	uint8_t flags;
};

struct UEFI_IFR_SECURITY {
	UEFI_IFR_OP_HEADER header;
	string permission;
};

struct UEFI_IFR_FORM_MAP {
	UEFI_IFR_OP_HEADER header;
	uint16_t formId;
};

struct UEFI_IFR_SET {
	UEFI_IFR_OP_HEADER header;
	uint16_t varStoreId;
	uint16_t varOffset;
	uint8_t varStoreType;
};

struct UEFI_IFR_GET {
	UEFI_IFR_OP_HEADER header;
	uint16_t varStoreId;
	uint16_t varOffset;
	uint8_t varStoreType;
};

struct UEFI_IFR_READ {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_WRITE {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_MAP {
	UEFI_IFR_OP_HEADER header;
};

struct UEFI_IFR_ANIMATION {
	UEFI_IFR_OP_HEADER header;
	uint16_t animationId;
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


#endif
