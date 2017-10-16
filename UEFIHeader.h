#pragma once
#include <cstdint>

typedef char CHAR8;
typedef wchar_t CHAR16;
typedef uint8_t BOOLEAN;
typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;

typedef UINT16 EFI_QUESTION_ID;
typedef UINT16 EFI_IMAGE_ID;
typedef UINT16 EFI_STRING_ID;
typedef UINT16 EFI_FORM_ID;
typedef UINT16 EFI_VARSTORE_ID;
typedef UINT16 EFI_ANIMATION_ID;
typedef UINT16 EFI_DEFAULT_ID;

typedef struct {
	UINT32 Data1;
	UINT16 Data2;
	UINT16 Data3;
	UINT8 Data4[8];
} EFI_GUID;

typedef struct {
	UINT32 Length : 24;
	UINT32 Type : 8;
	//UINT8 Data[0];
} EFI_HII_PACKAGE_HEADER;

#define EFI_HII_PACKAGE_TYPE_ALL 0x00
#define EFI_HII_PACKAGE_TYPE_GUID 0x01
#define EFI_HII_PACKAGE_FORMS 0x02
#define EFI_HII_PACKAGE_STRINGS 0x04
#define EFI_HII_PACKAGE_FONTS 0x05
#define EFI_HII_PACKAGE_IMAGES 0x06
#define EFI_HII_PACKAGE_SIMPLE_FONTS 0x07
#define EFI_HII_PACKAGE_DEVICE_PATH 0x08
#define EFI_HII_PACKAGE_KEYBOARD_LAYOUT 0x09
#define EFI_HII_PACKAGE_ANIMATIONS 0x0A
#define EFI_HII_PACKAGE_END 0xDF
#define EFI_HII_PACKAGE_TYPE_SYSTEM_BEGIN 0xE0
#define EFI_HII_PACKAGE_TYPE_SYSTEM_END 0xFF

typedef struct _EFI_HII_STRING_PACKAGE_HDR {
	EFI_HII_PACKAGE_HEADER Header;
	UINT32 HdrSize;
	UINT32 StringInfoOffset;
	CHAR16 LanguageWindow[16];
	EFI_STRING_ID LanguageName;
	CHAR8 Language[1];
} EFI_HII_STRING_PACKAGE_HDR;

typedef struct {
	UINT8 BlockType;
	//UINT8 BlockBody[0];
} EFI_HII_STRING_BLOCK;

#define EFI_HII_SIBT_END 0x00 // The end of the string information.
#define EFI_HII_SIBT_STRING_SCSU 0x10 // Single string using default font information.
#define EFI_HII_SIBT_STRING_SCSU_FONT 0x11 // Single string with font information.
#define EFI_HII_SIBT_STRINGS_SCSU 0x12 // Multiple strings using default font information.
#define EFI_HII_SIBT_STRINGS_SCSU_FONT 0x13 // Multiple strings with font information.
#define EFI_HII_SIBT_STRING_UCS2 0x14 // Single UCS - 2 string using default font information.
#define EFI_HII_SIBT_STRING_UCS2_FONT 0x15 // Single UCS - 2 string with font information
#define EFI_HII_SIBT_STRINGS_UCS2 0x16 // Multiple UCS - 2 strings using default font information.
#define EFI_HII_SIBT_STRINGS_UCS2_FONT 0x17 // Multiple UCS - 2 strings with font information.
#define EFI_HII_SIBT_DUPLICATE 0x20 // Create a duplicate of an existing string.
#define EFI_HII_SIBT_SKIP2 0x21 // Skip a certain number of string identifiers.
#define EFI_HII_SIBT_SKIP1 0x22 // Skip a certain number of string identifiers.
#define EFI_HII_SIBT_EXT1 0x30 // For future expansion(one byte length field)
#define EFI_HII_SIBT_EXT2 0x31 // For future expansion(two byte length field)
#define EFI_HII_SIBT_EXT4 0x32 // For future expansion(four byte length field)
#define EFI_HII_SIBT_FONT 0x40 // Font information.

#pragma pack(push, 1)
typedef struct _EFI_HII_SIBT_STRING_UCS2_BLOCK {
	EFI_HII_STRING_BLOCK Header;
	CHAR16 StringText[1];
} EFI_HII_SIBT_STRING_UCS2_BLOCK;
#pragma pack(pop)

typedef struct _EFI_HII_FORM_PACKAGE_HDR {
	EFI_HII_PACKAGE_HEADER Header;
	//EFI_IFR_OP_HEADER OpCodeHeader;
	//More op-codes follow
} EFI_HII_FORM_PACKAGE_HDR;

typedef struct _EFI_IFR_OP_HEADER {
	UINT8 OpCode;
	UINT8 Length : 7;
	UINT8 Scope : 1;
} EFI_IFR_OP_HEADER;

typedef struct {
	UINT8 Hour;
	UINT8 Minute;
	UINT8 Second;
} EFI_HII_TIME;

typedef struct {
	UINT16 Year;
	UINT8 Month;
	UINT8 Day;
} EFI_HII_DATE;

typedef struct {
	EFI_QUESTION_ID QuestionId;
	EFI_FORM_ID FormId;
	EFI_GUID FormSetGuid;
	EFI_STRING_ID DevicePath;
} EFI_HII_REF;

typedef union {
	UINT8 u8; // EFI_IFR_TYPE_NUM_SIZE_8
	UINT16 u16; // EFI_IFR_TYPE_NUM_SIZE_16
	UINT32 u32; // EFI_IFR_TYPE_NUM_SIZE_32
	UINT64 u64; // EFI_IFR_TYPE_NUM_SIZE_64
	BOOLEAN b; // EFI_IFR_TYPE_BOOLEAN
	EFI_HII_TIME time; // EFI_IFR_TYPE_TIME
	EFI_HII_DATE date; // EFI_IFR_TYPE_DATE
	EFI_STRING_ID string; // EFI_IFR_TYPE_STRING, EFI_IFR_TYPE_ACTION
	EFI_HII_REF ref; // EFI_IFR_TYPE_REF
	// UINT8 buffer[]; // EFI_IFR_TYPE_BUFFER
} EFI_IFR_TYPE_VALUE;

typedef struct _EFI_IFR_STATEMENT_HEADER {
	EFI_STRING_ID Prompt;
	EFI_STRING_ID Help;
} EFI_IFR_STATEMENT_HEADER;

#pragma pack(push, 1)
typedef struct _EFI_IFR_QUESTION_HEADER {
	EFI_IFR_STATEMENT_HEADER Header;
	EFI_QUESTION_ID QuestionId;
	EFI_VARSTORE_ID VarStoreId;
	union {
		EFI_STRING_ID VarName;
		UINT16 VarOffset;
	} VarStoreInfo;
	UINT8 Flags;
} EFI_IFR_QUESTION_HEADER;
#pragma pack(pop)

#define EFI_IFR_FLAG_READ_ONLY 0x01
#define EFI_IFR_FLAG_CALLBACK 0x04
#define EFI_IFR_FLAG_RESET_REQUIRED 0x10
#define EFI_IFR_FLAG_RECONNECT_REQUIRED 0x40
#define EFI_IFR_FLAG_OPTIONS_ONLY 0x80

typedef struct _EFI_IFR_ACTION {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	EFI_STRING_ID QuestionConfig;
} EFI_IFR_ACTION;

typedef struct _EFI_IFR_ACTION_1 {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
}_EFI_IFR_ACTION_1;

typedef struct _EFI_IFR_ANIMATION {
	EFI_IFR_OP_HEADER Header;
	EFI_ANIMATION_ID Id;
} EFI_IFR_ANIMATION;

typedef struct _EFI_IFR_ADD {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_ADD;

typedef struct _EFI_IFR_AND {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_AND;

typedef struct _EFI_IFR_BITWISE_AND {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_BITWISE_AND;

typedef struct _EFI_IFR_BITWISE_NOT {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_BITWISE_NOT;

typedef struct _EFI_IFR_BITWISE_OR {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_BITWISE_OR;

typedef struct _EFI_IFR_CATENATE {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_CATENATE;

typedef struct _EFI_IFR_CHECKBOX {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	UINT8 Flags;
} EFI_IFR_CHECKBOX;

#define EFI_IFR_CHECKBOX_DEFAULT 0x01
#define EFI_IFR_CHECKBOX_DEFAULT_MFG 0x02

typedef struct _EFI_IFR_CONDITIONAL {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_CONDITIONAL;

typedef struct _EFI_IFR_DATE {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	UINT8 Flags;
} EFI_IFR_DATE;

#define EFI_QF_DATE_YEAR_SUPPRESS 0x01
#define EFI_QF_DATE_MONTH_SUPPRESS 0x02
#define EFI_QF_DATE_DAY_SUPPRESS 0x04
#define EFI_QF_DATE_STORAGE 0x30

#define QF_DATE_STORAGE_NORMAL 0x00
#define QF_DATE_STORAGE_TIME 0x10
#define QF_DATE_STORAGE_WAKEUP 0x20

#pragma pack(push, 1)
typedef struct _EFI_IFR_DEFAULT {
	EFI_IFR_OP_HEADER Header;
	UINT16 DefaultId;
	UINT8 Type;
	EFI_IFR_TYPE_VALUE Value;
} EFI_IFR_DEFAULT;
#pragma pack(pop)

typedef struct _EFI_IFR_DEFAULT_2 {
	EFI_IFR_OP_HEADER Header;
	UINT16 DefaultId;
	UINT8 Type;
} EFI_IFR_DEFAULT_2;

typedef struct _EFI_IFR_DEFAULTSTORE {
	EFI_IFR_OP_HEADER Header;
	EFI_STRING_ID DefaultName;
	UINT16 DefaultId;
} EFI_IFR_DEFAULTSTORE;

typedef struct _EFI_IFR_DISABLE_IF {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_DISABLE_IF;

typedef struct _EFI_IFR_DIVIDE {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_DIVIDE;

typedef struct _EFI_IFR_DUP {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_DUP;

typedef struct _EFI_IFR_END {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_END;

typedef struct _EFI_IFR_EQUAL {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_EQUAL;

typedef struct _EFI_IFR_EQ_ID_ID {
	EFI_IFR_OP_HEADER Header;
	EFI_QUESTION_ID QuestionId1;
	EFI_QUESTION_ID QuestionId2;
} EFI_IFR_EQ_ID_ID;

typedef struct _EFI_IFR_EQ_ID_VAL_LIST {
	EFI_IFR_OP_HEADER Header;
	EFI_QUESTION_ID QuestionId;
	UINT16 ListLength;
	UINT16 ValueList[1];
} EFI_IFR_EQ_ID_VAL_LIST;

typedef struct _EFI_IFR_EQ_ID_VAL {
	EFI_IFR_OP_HEADER Header;
	EFI_QUESTION_ID QuestionId;
	UINT16 Value;
} EFI_IFR_EQ_ID_VAL;

typedef struct _EFI_IFR_FALSE {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_FALSE;

typedef struct _EFI_IFR_FIND {
	EFI_IFR_OP_HEADER Header;
	UINT8 Format;
} EFI_IFR_FIND;

#define EFI_IFR_FF_CASE_SENSITIVE 0x00
#define EFI_IFR_FF_CASE_INSENSITIVE 0x01

typedef struct _EFI_IFR_FORM {
	EFI_IFR_OP_HEADER Header;
	EFI_FORM_ID FormId;
	EFI_STRING_ID FormTitle;
} EFI_IFR_FORM;

typedef struct _EFI_IFR_FORM_MAP_METHOD {
	EFI_STRING_ID MethodTitle;
	EFI_GUID MethodIdentifier;
} EFI_IFR_FORM_MAP_METHOD;
typedef struct _EFI_IFR_FORM_MAP {
	EFI_IFR_OP_HEADER Header;
	EFI_FORM_ID FormId;
	//EFI_IFR_FORM_MAP_METHOD Methods[];
} EFI_IFR_FORM_MAP;

#pragma pack(push, 1)
typedef struct _EFI_IFR_FORM_SET {
	EFI_IFR_OP_HEADER Header;
	EFI_GUID Guid;
	EFI_STRING_ID FormSetTitle;
	EFI_STRING_ID Help;
	UINT8 Flags;
	EFI_GUID ClassGuid[1];
} EFI_IFR_FORM_SET;
#pragma pack(pop)

typedef struct _EFI_IFR_GET {
	EFI_IFR_OP_HEADER Header;
	EFI_VARSTORE_ID VarStoreId;
	union {
		EFI_STRING_ID VarName;
		UINT16 VarOffset;
	} VarStoreInfo;
	UINT8 VarStoreType;
} EFI_IFR_GET;

typedef struct _EFI_IFR_GRAY_OUT_IF {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_GRAY_OUT_IF;

typedef struct _EFI_IFR_GREATER_EQUAL {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_GREATER_EQUAL;

typedef struct _EFI_IFR_GREATER_THAN {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_GREATER_THAN;

typedef struct _EFI_IFR_GUID {
	EFI_IFR_OP_HEADER Header;
	EFI_GUID Guid;
	//Optional Data Follows
} EFI_IFR_GUID;

typedef struct _EFI_IFR_IMAGE {
	EFI_IFR_OP_HEADER Header; // why was it missing in docs?
	EFI_IMAGE_ID Id;
} EFI_IFR_IMAGE;

typedef struct _EFI_IFR_INCONSISTENT_IF {
	EFI_IFR_OP_HEADER Header;
	EFI_STRING_ID Error;
} EFI_IFR_INCONSISTENT_IF;

typedef struct _EFI_IFR_LENGTH {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_LENGTH;

typedef struct _EFI_IFR_LESS_EQUAL {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_LESS_EQUAL;

typedef struct _EFI_IFR_LESS_THAN {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_LESS_THAN;

typedef struct _EFI_IFR_LOCKED {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_LOCKED;

typedef struct _EFI_IFR_MAP {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_MAP;

typedef struct _EFI_IFR_MATCH {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_MATCH;

typedef struct _EFI_IFR_MID {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_MID;

typedef struct _EFI_IFR_MODAL_TAG {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_MODAL_TAG;

typedef struct _EFI_IFR_MODULO {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_MODULO;

typedef struct _EFI_IFR_MULTIPLY {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_MULTPLY;

typedef struct _EFI_IFR_NOT {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_NOT;

typedef struct _EFI_IFR_NOT_EQUAL {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_NOT_EQUAL;

typedef struct _EFI_IFR_NO_SUBMIT_IF {
	EFI_IFR_OP_HEADER Header;
	EFI_STRING_ID Error;
} EFI_IFR_NO_SUBMIT_IF;

#pragma pack(push, 1)
typedef struct _EFI_IFR_NUMERIC {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	UINT8 Flags;

	union {
		struct {
			UINT8 MinValue;
			UINT8 MaxValue;
			UINT8 Step;
		} u8;
		struct {
			UINT16 MinValue;
			UINT16 MaxValue;
			UINT16 Step;
		} u16;
		struct {
			UINT32 MinValue;
			UINT32 MaxValue;
			UINT32 Step;
		} u32;
		struct {
			UINT64 MinValue;
			UINT64 MaxValue;
			UINT64 Step;
		} u64;
	} data;
} EFI_IFR_NUMERIC;
#pragma pack(pop)

#define EFI_IFR_NUMERIC_SIZE 0x03
#define EFI_IFR_NUMERIC_SIZE_1 0x00
#define EFI_IFR_NUMERIC_SIZE_2 0x01
#define EFI_IFR_NUMERIC_SIZE_4 0x02
#define EFI_IFR_NUMERIC_SIZE_8 0x03
#define EFI_IFR_DISPLAY 0x30
#define EFI_IFR_DISPLAY_INT_DEC 0x00
#define EFI_IFR_DISPLAY_UINT_DEC 0x10
#define EFI_IFR_DISPLAY_UINT_HEX 0x20

typedef struct _EFI_IFR_ONE {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_ONE;

typedef struct _EFI_IFR_ONES {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_ONES;

#pragma pack(push, 1)
typedef struct _EFI_IFR_ONE_OF {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	UINT8 Flags;
	union {
		struct {
			UINT8 MinValue;
			UINT8 MaxValue;
			UINT8 Step;
		} u8;
		struct {
			UINT16 MinValue;
			UINT16 MaxValue;
			UINT16 Step;
		} u16;
		struct {
			UINT32 MinValue;
			UINT32 MaxValue;
			UINT32 Step;
		} u32;
		struct {
			UINT64 MinValue;
			UINT64 MaxValue;
			UINT64 Step;
		} u64;
	} data;
} EFI_IFR_ONE_OF;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _EFI_IFR_ONE_OF_OPTION {
	EFI_IFR_OP_HEADER Header;
	EFI_STRING_ID Option;
	UINT8 Flags;
	UINT8 Type;
	EFI_IFR_TYPE_VALUE Value;
} EFI_IFR_ONE_OF_OPTION;
#pragma pack(pop)

#define EFI_IFR_TYPE_NUM_SIZE_8 0x00
#define EFI_IFR_TYPE_NUM_SIZE_16 0x01
#define EFI_IFR_TYPE_NUM_SIZE_32 0x02
#define EFI_IFR_TYPE_NUM_SIZE_64 0x03
#define EFI_IFR_TYPE_BOOLEAN 0x04
#define EFI_IFR_TYPE_TIME 0x05
#define EFI_IFR_TYPE_DATE 0x06
#define EFI_IFR_TYPE_STRING 0x07
#define EFI_IFR_TYPE_OTHER 0x08
#define EFI_IFR_TYPE_UNDEFINED 0x09
#define EFI_IFR_TYPE_ACTION 0x0A
#define EFI_IFR_TYPE_BUFFER 0x0B
#define EFI_IFR_TYPE_REF 0x0C

#define EFI_IFR_OPTION_DEFAULT 0x10
#define EFI_IFR_OPTION_DEFAULT_MFG 0x20

typedef struct _EFI_IFR_OR {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_OR;

typedef struct _EFI_IFR_ORDERED_LIST {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	UINT8 MaxContainers;
	UINT8 Flags;
} EFI_IFR_ORDERED_LIST;

#define EFI_IFR_UNIQUE_SET 0x01
#define EFI_IFR_NO_EMPTY_SET 0x02

typedef struct _EFI_IFR_PASSWORD {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	UINT16 MinSize;
	UINT16 MaxSize;
} EFI_IFR_PASSWORD;

typedef struct _EFI_IFR_QUESTION_REF1 {
	EFI_IFR_OP_HEADER Header;
	EFI_QUESTION_ID QuestionId;
} EFI_IFR_QUESTION_REF1;

typedef struct _EFI_IFR_QUESTION_REF2 {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_QUESTION_REF2;

typedef struct _EFI_IFR_QUESTION_REF3 {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_QUESTION_REF3;

typedef struct _EFI_IFR_QUESTION_REF3_2 {
	EFI_IFR_OP_HEADER Header;
	EFI_STRING_ID DevicePath;
} EFI_IFR_QUESTION_REF3_2;

typedef struct _EFI_IFR_QUESTION_REF3_3 {
	EFI_IFR_OP_HEADER Header;
	EFI_STRING_ID DevicePath;
	EFI_GUID Guid;
} EFI_IFR_QUESTION_REF3_3;

typedef struct _EFI_IFR_READ {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_READ;

#pragma pack(push, 1)
typedef struct _EFI_IFR_REF {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	EFI_FORM_ID FormId;
} EFI_IFR_REF;
#pragma pack(pop)

typedef struct _EFI_IFR_REF2 {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	EFI_FORM_ID FormId;
	EFI_QUESTION_ID QuestionId;
} EFI_IFR_REF2;

typedef struct _EFI_IFR_REF3 {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	EFI_FORM_ID FormId;
	EFI_QUESTION_ID QuestionId;
	EFI_GUID FormSetId;
} EFI_IFR_REF3;

typedef struct _EFI_IFR_REF4 {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	EFI_FORM_ID FormId;
	EFI_QUESTION_ID QuestionId;
	EFI_GUID FormSetId;
	EFI_STRING_ID DevicePath;
} EFI_IFR_REF4;

typedef struct _EFI_IFR_REF5 {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
} EFI_IFR_REF5;

typedef struct _EFI_IFR_REFRESH {
	EFI_IFR_OP_HEADER Header;
	UINT8 RefreshInterval;
} EFI_IFR_REFRESH;

typedef struct _EFI_IFR_REFRESH_ID {
	EFI_IFR_OP_HEADER Header;
	EFI_GUID RefreshEventGroupId;
} EFI_IFR_REFRESH_ID;

typedef struct _EFI_IFR_RESET_BUTTON {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_STATEMENT_HEADER Statement;
	EFI_DEFAULT_ID DefaultId;
} EFI_IFR_RESET_BUTTON;

typedef struct _EFI_IFR_RULE {
	EFI_IFR_OP_HEADER Header;
	UINT8 RuleId;
} EFI_IFR_RULE;

typedef struct _EFI_IFR_RULE_REF {
	EFI_IFR_OP_HEADER Header;
	UINT8 RuleId;
} EFI_IFR_RULE_REF;

typedef struct _EFI_IFR_SECURITY {
	EFI_IFR_OP_HEADER Header;
	EFI_GUID Permissions;
} EFI_IFR_SECURITY;

typedef struct _EFI_IFR_SET {
	EFI_IFR_OP_HEADER Header;
	EFI_VARSTORE_ID VarStoreId;
	union {
		EFI_STRING_ID VarName;
		UINT16 VarOffset;
	} VarStoreInfo;
	UINT8 VarStoreType;
} EFI_IFR_SET;

typedef struct _EFI_IFR_SHIFT_LEFT {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_SHIFT_LEFT;

typedef struct _EFI_IFR_SHIFT_RIGHT {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_SHIFT_RIGHT;

typedef struct _EFI_IFR_SPAN {
	EFI_IFR_OP_HEADER Header;
	UINT8 Flags;
} EFI_IFR_SPAN;

#define EFI_IFR_FLAGS_FIRST_MATCHING 0x00
#define EFI_IFR_FLAGS_FIRST_NON_MATCHING 0x01

typedef struct _EFI_IFR_STRING {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	UINT8 MinSize;
	UINT8 MaxSize;
	UINT8 Flags;
} EFI_IFR_STRING;

typedef struct _EFI_IFR_STRING_REF1 {
	EFI_IFR_OP_HEADER Header;
	EFI_STRING_ID StringId;
} EFI_IFR_STRING_REF1;

typedef struct _EFI_IFR_STRING_REF2 {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_STRING_REF2;

typedef struct _EFI_IFR_SUBTITLE {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_STATEMENT_HEADER Statement;
	UINT8 Flags;
} EFI_IFR_SUBTITLE;

#define EFI_IFR_FLAGS_HORIZONTAL 0x01

typedef struct _EFI_IFR_SUBTRACT {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_SUBTRACT;

typedef struct _EFI_IFR_SUPPRESS_IF {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_SUPPRESS_IF;

typedef struct _EFI_IFR_TEXT {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_STATEMENT_HEADER Statement;
	EFI_STRING_ID TextTwo;
} EFI_IFR_TEXT;

typedef struct _EFI_IFR_THIS {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_THIS;

typedef struct _EFI_IFR_TIME {
	EFI_IFR_OP_HEADER Header;
	EFI_IFR_QUESTION_HEADER Question;
	UINT8 Flags;
} EFI_IFR_TIME;

typedef struct _EFI_IFR_TOKEN {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_TOKEN;

typedef struct _EFI_IFR_TO_BOOLEAN {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_TO_BOOLEAN;

typedef struct _EFI_IFR_TO_LOWER {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_TO_LOWER;

typedef struct _EFI_IFR_TO_STRING {
	EFI_IFR_OP_HEADER Header;
	UINT8 Format;
} EFI_IFR_TO_STRING;

typedef struct _EFI_IFR_TO_UINT {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_TO_UINT;

typedef struct _EFI_IFR_TO_UPPER {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_TO_UPPER;

typedef struct _EFI_IFR_TRUE {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_TRUE;

typedef struct _EFI_IFR_UINT8 {
	EFI_IFR_OP_HEADER Header;
	UINT8 Value;
} EFI_IFR_UINT8;

typedef struct _EFI_IFR_UINT16 {
	EFI_IFR_OP_HEADER Header;
	UINT16 Value;
} EFI_IFR_UINT16;

typedef struct _EFI_IFR_UINT32 {
	EFI_IFR_OP_HEADER Header;
	UINT32 Value;
} EFI_IFR_UINT32;

typedef struct _EFI_IFR_UINT64 {
	EFI_IFR_OP_HEADER Header;
	UINT64 Value;
} EFI_IFR_UINT64;

typedef struct _EFI_IFR_UNDEFINED {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_UNDEFINED;

typedef struct _EFI_IFR_VALUE {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_VALUE;

#pragma pack(push, 1)
typedef struct {
	EFI_IFR_OP_HEADER Header;
	EFI_GUID Guid;
	EFI_VARSTORE_ID VarStoreId;
	UINT16 Size;
	UINT8 Name[1];
} EFI_IFR_VARSTORE;
#pragma pack(pop)

typedef struct _EFI_IFR_VARSTORE_NAME_VALUE {
	EFI_IFR_OP_HEADER Header;
	EFI_VARSTORE_ID VarStoreId;
	EFI_GUID Guid;
} EFI_IFR_VARSTORE_NAME_VALUE;

#pragma pack(push, 1)
typedef struct _EFI_IFR_VARSTORE_EFI {
	EFI_IFR_OP_HEADER Header;
	EFI_VARSTORE_ID VarStoreId;
	EFI_GUID Guid;
	UINT32 Attributes;
	UINT16 Size;
	UINT8 Name[1];
} EFI_IFR_VARSTORE_EFI;
#pragma pack(pop)

typedef struct _EFI_IFR_VARSTORE_DEVICE {
	EFI_IFR_OP_HEADER Header;
	EFI_STRING_ID DevicePath;
} EFI_IFR_VARSTORE_DEVICE;

typedef struct _EFI_IFR_VERSION {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_VERSION;

typedef struct _EFI_IFR_WRITE {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_WRITE;

typedef struct _EFI_IFR_ZERO {
	EFI_IFR_OP_HEADER Header;
} EFI_IFR_ZERO;

typedef struct _EFI_IFR_WARNING_IF {
	EFI_IFR_OP_HEADER Header;
	EFI_STRING_ID Warning;
	UINT8 TimeOut;
} EFI_IFR_WARNING_IF;

typedef struct _EFI_IFR_MATCH2 {
	EFI_IFR_OP_HEADER Header;
	EFI_GUID SyntaxType;
} EFI_IFR_MATCH2;

#define EFI_IFR_FORM_OP 0x01 // Form
#define EFI_IFR_SUBTITLE_OP 0x02 // Subtitle statement
#define EFI_IFR_TEXT_OP 0x03 // Static text / image statement
#define EFI_IFR_IMAGE_OP 0x04 // Static image.
#define EFI_IFR_ONE_OF_OP 0x05 // One - of question
#define EFI_IFR_CHECKBOX_OP 0x06 // Boolean question
#define EFI_IFR_NUMERIC_OP 0x07 // Numeric question
#define EFI_IFR_PASSWORD_OP 0x08 // Password string question
#define EFI_IFR_ONE_OF_OPTION_OP 0x09 // Option
#define EFI_IFR_SUPPRESS_IF_OP 0x0A // Suppress if conditional
#define EFI_IFR_LOCKED_OP 0x0B // Marks statement / question as locked
#define EFI_IFR_ACTION_OP 0x0C // Button question
#define EFI_IFR_RESET_BUTTON_OP 0x0D // Reset button statement
#define EFI_IFR_FORM_SET_OP 0x0E // Form set
#define EFI_IFR_REF_OP 0x0F // Cross - reference statement
#define EFI_IFR_NO_SUBMIT_IF_OP 0x10 // Error checking conditional
#define EFI_IFR_INCONSISTENT_IF_OP 0x11 // Error checking conditional
#define EFI_IFR_EQ_ID_VAL_OP 0x12 // Return true if question value equals UINT16
#define EFI_IFR_EQ_ID_ID_OP 0x13 // Return true if question value equals another question value
#define EFI_IFR_EQ_ID_VAL_LIST_OP 0x14 // Return true if question value is found in list of UINT16s
#define EFI_IFR_AND_OP 0x15 // Push true if both sub - expressions returns true.
#define EFI_IFR_OR_OP 0x16 // Push true if either sub - expressions returns true.
#define EFI_IFR_NOT_OP 0x17 // Push false if sub - expression returns true, otherwise return true.
#define EFI_IFR_RULE_OP 0x18 // Create rule in current form.
#define EFI_IFR_GRAY_OUT_IF_OP 0x19 // Nested statements, questions or options will not be selectable if expression returns true.
#define EFI_IFR_DATE_OP 0x1A // Date question.
#define EFI_IFR_TIME_OP 0x1B // Time question.
#define EFI_IFR_STRING_OP 0x1C // String question
#define EFI_IFR_REFRESH_OP 0x1D // Interval for refreshing a question
#define EFI_IFR_DISABLE_IF_OP 0x1E // Nested statements, questions or options will not be processed if expression returns true.
#define EFI_IFR_ANIMATION_OP 0x1F // Animation associated with question statement, form or form set.
#define EFI_IFR_TO_LOWER_OP 0x20 // Convert a string on the expression stack to lower case.
#define EFI_IFR_TO_UPPER_OP 0x21 // Convert a string on the expression stack to upper case.
#define EFI_IFR_MAP_OP 0x22 // Convert one value to another by selecting a match from a list.
#define EFI_IFR_ORDERED_LIST_OP 0x23 // Set question
#define EFI_IFR_VARSTORE_OP 0x24 // Define a buffer - style variable storage.
#define EFI_IFR_VARSTORE_NAME_VALUE_OP 0x25 // Define a name / value style variable storage.
#define EFI_IFR_VARSTORE_EFI_OP 0x26 // Define a UEFI variable style variable storage.
#define EFI_IFR_VARSTORE_DEVICE_OP 0x27 // Specify the device path to use for variable storage.
#define EFI_IFR_VERSION_OP 0x28 // Push the revision level of the UEFI Specification to which this Forms Processor is compliant.
#define EFI_IFR_END_OP 0x29 // Marks end of scope.
#define EFI_IFR_MATCH_OP 0x2A // Push TRUE if string matches a pattern.
#define EFI_IFR_MATCH2_OP 0x64 // Push TRUE if string matches a Regular Expression pattern.
#define EFI_IFR_GET_OP 0x2B // Return a stored value.
#define EFI_IFR_SET_OP 0x2C // Change a stored value.
#define EFI_IFR_READ_OP 0x2D // Provides a value for the current question or default.
#define EFI_IFR_WRITE_OP 0x2E // Change a value for the current question.
#define EFI_IFR_EQUAL_OP 0x2F // Push TRUE if two expressions are equal.
#define EFI_IFR_NOT_EQUAL_OP 0x30 // Push TRUE if two expressions are not equal.
#define EFI_IFR_GREATER_THAN_OP 0x31 // Push TRUE if one expression is greater than another expression.
#define EFI_IFR_GREATER_EQUAL_OP 0x32 // Push TRUE if one expression is greater than or equal to another expression.
#define EFI_IFR_LESS_THAN_OP 0x33 // Push TRUE if one expression is less than another expression.
#define EFI_IFR_LESS_EQUAL_OP 0x34 // Push TRUE if one expression is less than or equal to another expression.
#define EFI_IFR_BITWISE_AND_OP 0x35 // Bitwise - AND two unsigned integers and push the result.
#define EFI_IFR_BITWISE_OR_OP 0x36 // Bitwise - OR two unsigned integers and push the result.
#define EFI_IFR_BITWISE_NOT_OP 0x37 // Bitwise - NOT an unsigned integer and push the result.
#define EFI_IFR_SHIFT_LEFT_OP 0x38 // Shift an unsigned integer left by a number of bits and push the result.
#define EFI_IFR_SHIFT_RIGHT_OP 0x39 // Shift an unsigned integer right by a number of bits and push the result.
#define EFI_IFR_ADD_OP 0x3A // Add two unsigned integers and push the result.
#define EFI_IFR_SUBTRACT_OP 0x3B // Subtract two unsigned integers and push the result.
#define EFI_IFR_MULTIPLY_OP 0x3C // Multiply two unsigned integers and push the result.
#define EFI_IFR_DIVIDE_OP 0x3D // Divide one unsigned integer by another and push the result.
#define EFI_IFR_MODULO_OP 0x3E // Divide one unsigned integer by another and push the remainder.
#define EFI_IFR_RULE_REF_OP 0x3F // Evaluate a rule
#define EFI_IFR_QUESTION_REF1_OP 0x40 // Push a question’s value
#define EFI_IFR_QUESTION_REF2_OP 0x41 // Push a question’s value
#define EFI_IFR_UINT8_OP 0x42 // Push an 8 - bit unsigned integer
#define EFI_IFR_UINT16_OP 0x43 // Push a 16 - bit unsigned integer.
#define EFI_IFR_UINT32_OP 0x44 // Push a 32 - bit unsigned integer
#define EFI_IFR_UINT64_OP 0x45 // Push a 64 - bit unsigned integer.
#define EFI_IFR_TRUE_OP 0x46 // Push a boolean TRUE.
#define EFI_IFR_FALSE_OP 0x47 // Push a boolean FALSE
#define EFI_IFR_TO_UINT_OP 0x48 // Convert expression to an unsigned integer
#define EFI_IFR_TO_STRING_OP 0x49 // Convert expression to a string
#define EFI_IFR_TO_BOOLEAN_OP 0x4A // Convert expression to a boolean.
#define EFI_IFR_MID_OP 0x4B // Extract portion of string or buffer
#define EFI_IFR_FIND_OP 0x4C // Find a string in a string.
#define EFI_IFR_TOKEN_OP 0x4D // Extract a delimited byte or character string from buffer or string.
#define EFI_IFR_STRING_REF1_OP 0x4E // Push a string
#define EFI_IFR_STRING_REF2_OP 0x4F // Push a string
#define EFI_IFR_CONDITIONAL_OP 0x50 // Duplicate one of two expressions depending on result of the first expression.
#define EFI_IFR_QUESTION_REF3_OP 0x51 // Push a question’s value from a different form.
#define EFI_IFR_ZERO_OP 0x52 // Push a zero
#define EFI_IFR_ONE_OP 0x53 // Push a one
#define EFI_IFR_ONES_OP 0x54 // Push a 0xFFFFFFFFFFFFFFFF.
#define EFI_IFR_UNDEFINED_OP 0x55 // Push Undefined
#define EFI_IFR_LENGTH_OP 0x56 // Push length of buffer or string.
#define EFI_IFR_DUP_OP 0x57 // Duplicate top of expression stack
#define EFI_IFR_THIS_OP 0x58 // Push the current question’s value
#define EFI_IFR_SPAN_OP 0x59 // Return first matching / non - matching character in a string
#define EFI_IFR_VALUE_OP 0x5A // Provide a value for a question
#define EFI_IFR_DEFAULT_OP 0x5B // Provide a default value for a question.
#define EFI_IFR_DEFAULTSTORE_OP 0x5C // Define a Default Type Declaration
#define EFI_IFR_FORM_MAP_OP 0x5D // Create a standards - map form.
#define EFI_IFR_CATENATE_OP 0x5E // Push concatenated buffers or strings.
#define EFI_IFR_GUID_OP 0x5F // An extensible GUIDed op - code
#define EFI_IFR_SECURITY_OP 0x60 // Returns whether current user profile contains specified setup access privileges.
#define EFI_IFR_MODAL_TAG_OP 0x61 // Specify current form is modal
#define EFI_IFR_REFRESH_ID_OP 0x62 // Establish an event group for refreshing a forms - based element.
#define EFI_IFR_WARNING_IF_OP 0x63 // Warning conditional
