// Header files
#include <iostream>
#include <fstream>
#include <iomanip>
#include "UEFI.h"
#include "UEFIHeader.h"

// Supporting function implementation
void getUEFIStringPackages(vector<UEFI_IFR_STRING_PACK> &stringPackages, const string &buffer) {

	// Initialize variables
	UEFI_IFR_STRING_PACK tempStringPackage;

	//int numPackages = 0;

	// Go through buffer
	for (unsigned int i = 0; i < buffer.size() - 48; i++)

		// Check if string pakage was found
		if ((buffer[i] != '\x00' || buffer[i + 1] != '\x00' || buffer[i + 2] != '\x00') && buffer[i + 3] == '\x04' && buffer[i + 4] == '\x34' && buffer[i + 44] == '\x01' && buffer[i + 45] == '\x00' && buffer[i + 48] == '\x2D' && i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 1] == '\x00' && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 2] == '\x00') {

			//numPackages++;

			//EFI_HII_STRING_PACKAGE_HDR* strs = (EFI_HII_STRING_PACKAGE_HDR*)(&buffer[i]);

			//EFI_HII_STRING_BLOCK* strings = (EFI_HII_STRING_BLOCK*)((char*)strs + strs->StringInfoOffset);

			//UINT32 pkgLen = strs->Header.Length;

			//for (UINT32 offset = 0; i < pkgLen; )
			//{
			//	EFI_HII_SIBT_STRING_UCS2_BLOCK* str;

			//	if ((strings + offset)->BlockType == EFI_HII_SIBT_STRING_UCS2)
			//	{
			//		int s2 = sizeof(EFI_HII_STRING_BLOCK);
			//		int s = sizeof(UINT8);
			//		int offs = offsetof(EFI_HII_SIBT_STRING_UCS2_BLOCK, StringText);
			//		str = (EFI_HII_SIBT_STRING_UCS2_BLOCK*)(strings + offset);
			//		int i = 0;

			//		offset += (wcslen(str->StringText) + 1) * sizeof(CHAR16) + 1;
			//	}
			//	else if ((strings + offset)->BlockType == EFI_HII_SIBT_END)
			//	{
			//		break;
			//	}
			//	else
			//	{

			//	}
			//}

			// Set temp string package
			tempStringPackage.header.offset = i;

			tempStringPackage.header.length = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16));

			tempStringPackage.header.type = static_cast<unsigned char>(buffer[i + 3]);
			tempStringPackage.headerSize = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 4]) + (static_cast<unsigned char>(buffer[i + 5]) << 8) + (static_cast<unsigned char>(buffer[i + 6]) << 16) + (static_cast<unsigned char>(buffer[i + 7]) << 24));

			tempStringPackage.stringOffset = i + static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 8]) + (static_cast<unsigned char>(buffer[i + 9]) << 8) + (static_cast<unsigned char>(buffer[i + 10]) << 16) + (static_cast<unsigned char>(buffer[i + 11]) << 24));

			for (int j = 0; j < 16; j++)
				tempStringPackage.languageWindow.push_back(static_cast<uint16_t>(static_cast<unsigned char>(buffer[i + 12 + j * 2]) + (static_cast<unsigned char>(buffer[i + 13 + j * 2]) << 8)));

			for (int j = i + 46; buffer[j] != '\x00'; j++)
				tempStringPackage.language.push_back(buffer[j]);

			tempStringPackage.structureOffset = 0;

			// Add string package to list
			stringPackages.push_back(tempStringPackage);

			// Clear temp string package
			tempStringPackage.languageWindow.clear();
			tempStringPackage.language.clear();
		}
}

void displayUEFIStringPackages(const vector<UEFI_IFR_STRING_PACK> &stringPackages) {

	// Display string packages info
	cout << "String Packages Information:" << endl;

	// Go through string packages
	for (unsigned int i = 0; i < stringPackages.size(); i++) {

		// Display string package info
		cout << "\tString Package " << i << " Information:" << endl;
		cout << "\t\tOffset: " << dec << stringPackages[i].header.offset << hex << uppercase << " (0x" << stringPackages[i].header.offset << ')' << endl;
		cout << "\t\tLength: " << dec << stringPackages[i].header.length << hex << uppercase << " (0x" << stringPackages[i].header.length << ')' << endl;
		cout << "\t\tLanguage: " << stringPackages[i].language << endl;
	}

	cout << endl;
}

void getUEFIStrings(vector<UEFI_IFR_STRING_PACK> &stringPackages, vector<string> &strings, const string &buffer) {

	// Initialize variables
	int index;
	string temp;

	// Go through string packages
	for (unsigned int i = 0; i < stringPackages.size(); i++) {

		// Check if language isn't english
		if (stringPackages[i].language != "en-US")

			// Continue
			continue;

		// Set structure offset
		stringPackages[i].structureOffset = strings.size();

		// Add language string to list
		strings.push_back(stringPackages[i].language);

		// Loop while string exists
		for (index = stringPackages[i].stringOffset; buffer[index] == '\x14'; index += 2) {

			// Fill string
			for (index++; buffer[index] != '\x00'; index += 2)
				temp.push_back(buffer[index]);

			// Add string to list
			strings.push_back(temp);

			// Clear temp
			temp.clear();

			// Skip % padding
			while (buffer[index + 1] == '\x25')
				index += 2;
		}
	}

	// Go through strings
	for (unsigned int i = 0; i < strings.size(); i++) {

		// Change carriage return characters to spaces
		for (unsigned int j = 0; j < strings[i].size(); j++)
			if (strings[i][j] == '\r')
				strings[i][j] = ' ';

		// Remove leading spaces
		while (strings[i].size() > 1 && strings[i][0] == ' ')
			strings[i].erase(0, 1);

		// Change double spaces
		for (unsigned int j = 0; strings[i].size() && j < strings[i].size() - 1; j++)
			if (strings[i][j] == ' ' && strings[i][j + 1] == ' ')
				strings[i].erase(j, 1);
	}
}

void displayUEFIStrings(const vector<string> &strings) {

	// Print strings
	cout << "Strings:" << endl;

	// Go through strings
	for (uint32_t i = 0; i < strings.size(); i++)

		// Print string
		cout << '\t' << dec << i << " (0x" << hex << uppercase << i << ") " << strings[i] << endl;

	cout << endl;
}

void getUEFIFormSets(vector<UEFI_IFR_FORM_SET_PACK> &formSets, const string &buffer, const vector<UEFI_IFR_STRING_PACK> &stringPackages, const vector<string> &strings) {

	// Initialize variables
	UEFI_IFR_FORM_SET_PACK tempFormSet;
	vector<uint32_t> stringCandidates;
	uint32_t chosenCandidate;

	// Go through string packages
	for (unsigned int i = 0; i < stringPackages.size(); i++) {

		// Check if language isn't english
		if (stringPackages[i].language != "en-US")

			// Continue
			continue;

		// Get string candidate
		stringCandidates.push_back(i);
	}

	// Go through buffer
	for (uint32_t i = 0; i < buffer.size() - 4; i++)

		// Check if form set was found
		if ((buffer[i] != '\x00' || buffer[i + 1] != '\x00' || buffer[i + 2] != '\x00') && buffer[i + 3] == '\x02' && buffer[i + 4] == '\x0E' && i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 1] == '\x02' && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 2] == '\x29') {

			// Get chosen candidate for form set's string package
			chosenCandidate = stringCandidates[0];
			for (unsigned int j = 1; j < stringCandidates.size(); j++)
				if (abs(static_cast<signed>(stringPackages[j].header.offset - i)) < abs(static_cast<signed>(stringPackages[chosenCandidate].header.offset - i)))
					chosenCandidate = j;

			// Set temp form set
			tempFormSet.header.offset = i;
			tempFormSet.header.length = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16));
			tempFormSet.header.type = static_cast<unsigned char>(buffer[i + 3]);
			tempFormSet.titleString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[i + 22]) + (static_cast<unsigned char>(buffer[i + 23]) << 8));
			tempFormSet.usingStringPackage = stringCandidates[chosenCandidate];
			tempFormSet.title = strings[tempFormSet.titleString + stringPackages[tempFormSet.usingStringPackage].structureOffset];

			// Add temp form set to list
			formSets.push_back(tempFormSet);
		}
}

void displayUEFIFormSets(const vector<UEFI_IFR_FORM_SET_PACK> &formSets) {

	// Print form sets information
	cout << "Form Sets Information:" << endl;

	// Go through form sets
	for (uint32_t i = 0; i < formSets.size(); i++) {

		// Print form set information
		cout << "\tForm Set " << i << " Information:" << endl;
		cout << "\t\tOffset: " << dec << formSets[i].header.offset << hex << uppercase << " (0x" << formSets[i].header.offset << ')' << endl;
		cout << "\t\tLength: " << dec << formSets[i].header.length << hex << uppercase << " (0x" << formSets[i].header.length << ')' << endl;
		cout << "\t\tTitle ID: " << dec << formSets[i].titleString << hex << uppercase << " (0x" << formSets[i].titleString << ')' << endl;
		cout << "\t\tTitle: " << formSets[i].title << endl;
	}

	cout << endl;
}

vector<uint8_t> conditionalStack;
uint8_t numberOfTabs = 0;

void IndentText(uint8_t reason)
{
	// Increment number of tabs
	numberOfTabs++;

	// Add to conditional stack
	conditionalStack.push_back(reason);
}

const std::string & GetTextById(EFI_STRING_ID stringId, const std::vector<std::string> &strings, const uint32_t strPackageOffset)
{
	return strings[stringId + strPackageOffset];
}

std::string GuidToString(EFI_GUID &guid)
{
	char guid_cstr[39];
	snprintf(guid_cstr, sizeof(guid_cstr), "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	return std::string(guid_cstr);
}

void generateUEFIIFRDump(const string &outputFile, const vector<UEFI_IFR_STRING_PACK> &stringPackages, const vector<UEFI_IFR_FORM_SET_PACK> &formSets, const string &buffer, const vector<string> &strings) {

	// Initialize variables
	ofstream fout(outputFile.c_str());

	// Display protocol
	for (uint8_t i = 0; i < 32; i++)
		fout << ' ';
	fout << "UEFI Protocol Detected" << endl;
	for (uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl << endl << endl;

	// Display string packages info
	for (uint8_t i = 0; i < 32; i++)
		fout << ' ';
	fout << "String Packages" << endl;
	for (uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl;
	fout << "Offset:\t\tLanguage:" << endl;
	for (uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl;
	for (uint8_t i = 0; i < stringPackages.size(); i++) {
		fout << "0x" << hex << uppercase << stringPackages[i].header.offset;
		fout << "\t\t" << stringPackages[i].language << " (0x" << hex << uppercase << static_cast<unsigned int>(i) << ')' << endl;
	}
	fout << endl << endl;

	// Display form sets info
	for (uint8_t i = 0; i < 35; i++)
		fout << ' ';
	fout << "Form Sets" << endl;
	for (uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl;
	fout << "Offset:\t\tTitle:" << endl;
	for (uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl;
	for (uint8_t i = 0; i < formSets.size(); i++) {
		fout << "0x" << hex << uppercase << formSets[i].header.offset;
		fout << "\t\t" << formSets[i].title << " (0x" << hex << uppercase << formSets[i].titleString << " from string package 0x" << formSets[i].usingStringPackage << ')' << endl;
	}
	fout << endl << endl;

	// Display IFR
	for (uint8_t i = 0; i < 25; i++)
		fout << ' ';
	fout << "Internal Forms Representation" << endl;
	for (uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl;
	fout << "Offset:\t\tInstruction:" << endl;
	for (uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl;

	// Go through form sets
	for (uint32_t i = 0; i < formSets.size(); i++) {

		uint32_t strPackageOffset = stringPackages[formSets[i].usingStringPackage].structureOffset;

		// Go through all the form set's operations
		for (uint32_t j = formSets[i].header.offset + 4; j < formSets[i].header.offset + formSets[i].header.length; j += static_cast<unsigned char>(buffer[j + 1]) & 0x7F) {

			// Display offset
			fout << "0x" << hex << uppercase << j << ' ';

			if (buffer[j] != EFI_IFR_END_OP) {
				// Display tabs
				for (uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';
			}

			// Check opcodes
			if (buffer[j] == EFI_IFR_FORM_OP) {

				// Create temp
				EFI_IFR_FORM *temp = (EFI_IFR_FORM*)&buffer[j];

				// Display temp
				fout << "Form: " << strings[temp->FormTitle + strPackageOffset] << ", FormId: 0x" << hex << uppercase << temp->FormId;
			}
			else if (buffer[j] == EFI_IFR_SUBTITLE_OP) {

				// Create temp
				EFI_IFR_SUBTITLE *temp = (EFI_IFR_SUBTITLE*)&buffer[j];

				// Display temp
				fout << "Subtitle: Statement.Prompt: " << strings[temp->Statement.Prompt + strPackageOffset];
				//fout << ", Statement.Help: " << strings[temp->Statement.Help + strPackageOffset];
				fout << ", Flags: 0x" << hex << uppercase << (uint16_t)temp->Flags;
			}
			else if (buffer[j] == EFI_IFR_TEXT_OP) {

				// Create temp
				EFI_IFR_TEXT *temp = (EFI_IFR_TEXT*)&buffer[j];

				// Display temp
				fout << "Text: Statement.Prompt: " << strings[temp->Statement.Prompt + strPackageOffset];
				//fout << ", Statement.Help: " << strings[temp->Statement.Help + strPackageOffset];
				fout << ", TextTwo: " << strings[temp->TextTwo + strPackageOffset];
			}
			else if (buffer[j] == EFI_IFR_IMAGE_OP) {

				// Create temp
				EFI_IFR_IMAGE *temp = (EFI_IFR_IMAGE*)&buffer[j];

				// Display temp
				fout << "Image: Id: 0x" << hex << uppercase << temp->Id;
			}
			else if (buffer[j] == EFI_IFR_ONE_OF_OP) {

				// Create temp
				EFI_IFR_ONE_OF *temp = (EFI_IFR_ONE_OF*)&buffer[j];

				// If VarStoreId refers to Buffer Storage (EFI_IFR_VARSTORE or EFI_IFR_VARSTORE_EFI), then VarStoreInfo contains a 16-bit Buffer Storage offset (VarOffset). If VarStoreId refers to Name/Value Storage (EFI_IFR_VARSTORE_NAME_VALUE), then VarStoreInfo contains the String ID of the name (VarName) for this name/value pair.

				// Display temp
				fout << "One Of: " << strings[temp->Question.Header.Prompt + strPackageOffset] << ", VarStoreInfo (VarOffset/VarName): 0x" << hex << uppercase << temp->Question.VarStoreInfo.VarOffset << ", VarStore: 0x" << temp->Question.VarStoreId << ", QuestionId: 0x" << temp->Question.QuestionId;

				if ((temp->Flags & EFI_IFR_NUMERIC_SIZE) == EFI_IFR_NUMERIC_SIZE_1) {
					fout << ", Size: 1";
					fout << ", Min: 0x" << (uint16_t)temp->data.u8.MinValue;
					fout << ", Max 0x" << (uint16_t)temp->data.u8.MaxValue;
					fout << ", Step: 0x" << (uint16_t)temp->data.u8.Step;
				}
				else if ((temp->Flags & EFI_IFR_NUMERIC_SIZE) == EFI_IFR_NUMERIC_SIZE_2) {
					fout << ", Size: 2";
					fout << ", Min: 0x" << temp->data.u16.MinValue;
					fout << ", Max 0x" << temp->data.u16.MaxValue;
					fout << ", Step: 0x" << temp->data.u16.Step;
				}
				else if ((temp->Flags & EFI_IFR_NUMERIC_SIZE) == EFI_IFR_NUMERIC_SIZE_4) {
					fout << ", Size: 4";
					fout << ", Min: 0x" << temp->data.u32.MinValue;
					fout << ", Max 0x" << temp->data.u32.MaxValue;
					fout << ", Step: 0x" << temp->data.u32.Step;
				}
				else if ((temp->Flags & EFI_IFR_NUMERIC_SIZE) == EFI_IFR_NUMERIC_SIZE_8) {
					fout << ", Size: 8";
					fout << ", Min: 0x" << temp->data.u64.MinValue;
					fout << ", Max 0x" << temp->data.u64.MaxValue;
					fout << ", Step: 0x" << temp->data.u64.Step;
				}
			}
			else if (buffer[j] == EFI_IFR_CHECKBOX_OP) {

				// Create temp
				EFI_IFR_CHECKBOX *temp = (EFI_IFR_CHECKBOX*)&buffer[j];

				// Display temp
				fout << "Checkbox: " << strings[temp->Question.Header.Prompt + strPackageOffset] << ", VarStoreInfo (VarOffset/VarName): 0x" << hex << uppercase << temp->Question.VarStoreInfo.VarOffset << ", VarStore: 0x" << temp->Question.VarStoreId << ", QuestionId: 0x" << temp->Question.QuestionId;
			}
			else if (buffer[j] == EFI_IFR_NUMERIC_OP) {

				// Create temp
				EFI_IFR_NUMERIC *temp = (EFI_IFR_NUMERIC*)&buffer[j];

				// Display temp
				fout << "Numeric: " << strings[temp->Question.Header.Prompt + strPackageOffset] << ", VarStoreInfo (VarOffset/VarName): 0x" << hex << uppercase << temp->Question.VarStoreInfo.VarOffset << ", VarStore: 0x" << temp->Question.VarStoreId << ", QuestionId: 0x" << temp->Question.QuestionId;

				if ((temp->Flags & EFI_IFR_NUMERIC_SIZE) == EFI_IFR_NUMERIC_SIZE_1) {
					fout << ", Size: 1";
					fout << ", Min: 0x" << (uint16_t)temp->data.u8.MinValue;
					fout << ", Max 0x" << (uint16_t)temp->data.u8.MaxValue;
					fout << ", Step: 0x" << (uint16_t)temp->data.u8.Step;
				}
				else if ((temp->Flags & EFI_IFR_NUMERIC_SIZE) == EFI_IFR_NUMERIC_SIZE_2) {
					fout << ", Size: 2";
					fout << ", Min: 0x" << temp->data.u16.MinValue;
					fout << ", Max 0x" << temp->data.u16.MaxValue;
					fout << ", Step: 0x" << temp->data.u16.Step;
				}
				else if ((temp->Flags & EFI_IFR_NUMERIC_SIZE) == EFI_IFR_NUMERIC_SIZE_4) {
					fout << ", Size: 4";
					fout << ", Min: 0x" << temp->data.u32.MinValue;
					fout << ", Max 0x" << temp->data.u32.MaxValue;
					fout << ", Step: 0x" << temp->data.u32.Step;
				}
				else if ((temp->Flags & EFI_IFR_NUMERIC_SIZE) == EFI_IFR_NUMERIC_SIZE_8) {
					fout << ", Size: 8";
					fout << ", Min: 0x" << temp->data.u64.MinValue;
					fout << ", Max 0x" << temp->data.u64.MaxValue;
					fout << ", Step: 0x" << temp->data.u64.Step;
				}
			}
			else if (buffer[j] == EFI_IFR_PASSWORD_OP) {

				// Create temp
				EFI_IFR_PASSWORD *temp = (EFI_IFR_PASSWORD*)&buffer[j];

				// Display temp
				fout << "Password: " << strings[temp->Question.Header.Prompt + strPackageOffset] << ", VarStoreInfo (VarOffset/VarName): 0x" << hex << uppercase << temp->Question.VarStoreInfo.VarOffset << ", VarStore: 0x" << temp->Question.VarStoreId << ", QuestionId: 0x" << temp->Question.QuestionId;
				fout << ", MinSize: 0x" << temp->MinSize;
				fout << ", MaxSize 0x" << temp->MaxSize;
			}
			else if (buffer[j] == EFI_IFR_ONE_OF_OPTION_OP) {

				// Create temp
				EFI_IFR_ONE_OF_OPTION *temp = (EFI_IFR_ONE_OF_OPTION*)&buffer[j];

				// Display temp
				fout << "One Of Option: " << strings[temp->Option + strPackageOffset] << ", Value ";

				if (temp->Type == EFI_IFR_TYPE_NUM_SIZE_8)
					fout << "(8 bit): 0x" << hex << uppercase << (uint16_t)temp->Value.u8;
				else if (temp->Type == EFI_IFR_TYPE_NUM_SIZE_16)
					fout << "(16 bit): 0x" << hex << uppercase << temp->Value.u16;
				else if (temp->Type == EFI_IFR_TYPE_NUM_SIZE_32)
					fout << "(32 bit): 0x" << hex << uppercase << temp->Value.u32;
				else if (temp->Type == EFI_IFR_TYPE_NUM_SIZE_64)
					fout << "(64 bit): 0x" << hex << uppercase << temp->Value.u64;
				else if (temp->Type == EFI_IFR_TYPE_BOOLEAN)
					fout << "(Bool): " << (temp->Value.b ? "true" : "false");
				else if (temp->Type == EFI_IFR_TYPE_TIME)
					fout << "(Time)";
				else if (temp->Type == EFI_IFR_TYPE_DATE)
					fout << "(Date)";
				else if (temp->Type == EFI_IFR_TYPE_STRING)
					fout << "(String): " << strings[temp->Value.string + strPackageOffset];
				else if (temp->Type == EFI_IFR_TYPE_OTHER)
					fout << "(Other)";
				else if (temp->Type == EFI_IFR_TYPE_UNDEFINED)
					fout << "(Undefined)";
				else if (temp->Type == EFI_IFR_TYPE_ACTION)
					fout << "(Action)";
				else if (temp->Type == EFI_IFR_TYPE_BUFFER)
					fout << "(Buffer)";
				else if (temp->Type == EFI_IFR_TYPE_REF)
					fout << "(Ref)";

				if (temp->Flags & EFI_IFR_OPTION_DEFAULT)
					fout << " (default)";
				else if (temp->Flags & EFI_IFR_OPTION_DEFAULT_MFG)
					fout << " (default MFG)";
			}
			else if (buffer[j] == EFI_IFR_SUPPRESS_IF_OP) {

				// Create temp
				EFI_IFR_SUPPRESS_IF *temp = (EFI_IFR_SUPPRESS_IF*)&buffer[j];

				// Display temp
				fout << "Suppress If";
			}
			else if (buffer[j] == EFI_IFR_LOCKED_OP) {

				// Create temp
				EFI_IFR_LOCKED *temp = (EFI_IFR_LOCKED*)&buffer[j];

				// Display temp
				fout << "Locked";
			}
			else if (buffer[j] == EFI_IFR_ACTION_OP) {

				// Create temp
				EFI_IFR_ACTION *temp = (EFI_IFR_ACTION*)&buffer[j];

				// Display temp
				fout << "Action: " << strings[temp->Question.Header.Prompt + strPackageOffset] << ", VarStoreInfo (VarOffset/VarName): 0x" << hex << uppercase << temp->Question.VarStoreInfo.VarOffset << ", VarStore: 0x" << temp->Question.VarStoreId << ", QuestionId: 0x" << temp->Question.QuestionId << ", QuestionConfig: " << strings[temp->QuestionConfig + strPackageOffset];
			}
			else if (buffer[j] == EFI_IFR_RESET_BUTTON_OP) {

				// Create temp
				EFI_IFR_RESET_BUTTON *temp = (EFI_IFR_RESET_BUTTON*)&buffer[j];

				// Display temp
				fout << "Reset Button: " << strings[temp->Statement.Prompt + strPackageOffset] << ", DefaultId: " << hex << uppercase << temp->DefaultId;
			}
			else if (buffer[j] == EFI_IFR_FORM_SET_OP) {

				// Create temp
				EFI_IFR_FORM_SET *temp = (EFI_IFR_FORM_SET*)&buffer[j];

				// Display temp
				fout << "Form Set: " << strings[temp->FormSetTitle + strPackageOffset] << " [" << GuidToString(temp->Guid) << "]";

				for (uint8_t idx = 0; idx < (temp->Flags & 0x3); idx++)
					fout << ", ClassGuid" << (uint16_t)idx << " [" << GuidToString(temp->ClassGuid[idx]) << "]";
			}
			else if (buffer[j] == EFI_IFR_REF_OP) {

				// Create temp
				EFI_IFR_REF *temp = (EFI_IFR_REF*)&buffer[j];

				// Display temp
				fout << "Ref: " << strings[temp->Question.Header.Prompt + strPackageOffset] << ", VarStoreInfo (VarOffset/VarName): 0x" << hex << uppercase << temp->Question.VarStoreInfo.VarOffset << ", VarStore: 0x" << temp->Question.VarStoreId << ", QuestionId: 0x" << temp->Question.QuestionId << ", FormId: 0x" << hex << uppercase << temp->FormId;
			}
			else if (buffer[j] == EFI_IFR_NO_SUBMIT_IF_OP) {

				// Create temp
				EFI_IFR_NO_SUBMIT_IF *temp = (EFI_IFR_NO_SUBMIT_IF*)&buffer[j];

				// Display temp
				fout << "No Submit If: " << strings[temp->Error + strPackageOffset];
			}
			else if (buffer[j] == EFI_IFR_INCONSISTENT_IF_OP) {

				// Create temp
				EFI_IFR_INCONSISTENT_IF *temp = (EFI_IFR_INCONSISTENT_IF*)&buffer[j];

				// Display temp
				fout << "Inconsistent If: " << strings[temp->Error + strPackageOffset];
			}
			else if (buffer[j] == EFI_IFR_EQ_ID_VAL_OP) {

				// Create temp
				EFI_IFR_EQ_ID_VAL *temp = (EFI_IFR_EQ_ID_VAL*)&buffer[j];

				// Display temp
				fout << "QuestionId 0x" << hex << uppercase << temp->QuestionId << " equals value 0x" << hex << uppercase << temp->Value;
			}
			else if (buffer[j] == EFI_IFR_EQ_ID_ID_OP) {

				// Create temp
				EFI_IFR_EQ_ID_ID *temp = (EFI_IFR_EQ_ID_ID*)&buffer[j];

				// Display temp
				fout << "QuestionId 0x" << hex << uppercase << temp->QuestionId1 << " equals QuestionId 0x" << hex << uppercase << temp->QuestionId2;
			}
			else if (buffer[j] == EFI_IFR_EQ_ID_VAL_LIST_OP) {

				// Create temp
				EFI_IFR_EQ_ID_VAL_LIST *temp = (EFI_IFR_EQ_ID_VAL_LIST*)&buffer[j];

				// Display temp
				fout << "QuestionId 0x" << hex << uppercase << temp->QuestionId << " equals value in list (";
				for (uint8_t k = 0; k < temp->ListLength; k++) {
					fout << "0x" << hex << uppercase << temp->ValueList[k];
					if (k != temp->ListLength - 1)
						fout << ", ";
					else
						fout << ')';
				}
			}
			else if (buffer[j] == EFI_IFR_AND_OP) {

				// Create temp
				EFI_IFR_AND *temp = (EFI_IFR_AND*)&buffer[j];

				// Display temp
				fout << "And";
			}
			else if (buffer[j] == EFI_IFR_OR_OP) {

				// Create temp
				EFI_IFR_OR *temp = (EFI_IFR_OR*)&buffer[j];

				// Display temp
				fout << "Or";
			}
			else if (buffer[j] == EFI_IFR_NOT_OP) {

				// Create temp
				EFI_IFR_NOT *temp = (EFI_IFR_NOT*)&buffer[j];

				// Display temp
				fout << "Not";
			}
			else if (buffer[j] == EFI_IFR_RULE_OP) {

				// Create temp
				EFI_IFR_RULE *temp = (EFI_IFR_RULE*)&buffer[j];

				// Display temp
				fout << "Rule: RuleId: 0x" << hex << uppercase << (uint16_t)temp->RuleId;
			}
			else if (buffer[j] == EFI_IFR_GRAY_OUT_IF_OP) {

				// Create temp
				EFI_IFR_GRAY_OUT_IF *temp = (EFI_IFR_GRAY_OUT_IF*)&buffer[j];

				// Display temp
				fout << "Gray Out If";
			}
			else if (buffer[j] == EFI_IFR_DATE_OP) {

				// Create temp
				UEFI_IFR_DATE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.question.statement.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.question.statement.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.question.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8));
				temp.question.varStoreId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 8]) + (static_cast<unsigned char>(buffer[j + 9]) << 8));
				temp.question.varOffset = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.question.flags = static_cast<unsigned char>(buffer[j + 12]);
				temp.flags = static_cast<unsigned char>(buffer[j + 13]);

				// Display temp
				fout << "Date: " << strings[temp.question.statement.promptString + strPackageOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset << ", VarStore: 0x" << temp.question.varStoreId << ", QuestionId: 0x" << temp.question.questionId;
			}
			else if (buffer[j] == EFI_IFR_TIME_OP) {

				// Create temp
				UEFI_IFR_TIME temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.question.statement.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.question.statement.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.question.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8));
				temp.question.varStoreId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 8]) + (static_cast<unsigned char>(buffer[j + 9]) << 8));
				temp.question.varOffset = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.question.flags = static_cast<unsigned char>(buffer[j + 12]);
				temp.flags = static_cast<unsigned char>(buffer[j + 13]);

				// Display temp
				fout << "Time: " << strings[temp.question.statement.promptString + strPackageOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset << ", VarStore: 0x" << temp.question.varStoreId << ", QuestionId: 0x" << temp.question.questionId;
			}
			else if (buffer[j] == EFI_IFR_STRING_OP) {

				// Create temp
				UEFI_IFR_STRING temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.question.statement.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.question.statement.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.question.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8));
				temp.question.varStoreId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 8]) + (static_cast<unsigned char>(buffer[j + 9]) << 8));
				temp.question.varOffset = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.question.flags = static_cast<unsigned char>(buffer[j + 12]);
				temp.minimumSize = static_cast<unsigned char>(buffer[j + 13]);
				temp.maximumSize = static_cast<unsigned char>(buffer[j + 14]);
				temp.flags = static_cast<unsigned char>(buffer[j + 15]);

				// Display temp
				fout << "String: " << strings[temp.question.statement.promptString + strPackageOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset << ", VarStore: 0x" << temp.question.varStoreId << ", QuestionId: 0x" << temp.question.questionId;
			}
			else if (buffer[j] == EFI_IFR_REFRESH_OP) {

				// Create temp
				UEFI_IFR_REFRESH temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.refreshInterval = static_cast<unsigned char>(buffer[j + 2]);

				// Display temp
				fout << "Refresh: Interval: 0x" << hex << uppercase << temp.refreshInterval;
			}
			else if (buffer[j] == EFI_IFR_DISABLE_IF_OP) {

				// Create temp
				EFI_IFR_DISABLE_IF *temp = (EFI_IFR_DISABLE_IF*)&buffer[j];

				// Display temp
				fout << "Disable If";
			}
			else if (buffer[j] == EFI_IFR_ANIMATION_OP) {

				// Create temp
				UEFI_IFR_ANIMATION temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.animationId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));

				// Display temp
				fout << "Animation";
			}
			else if (buffer[j] == EFI_IFR_TO_LOWER_OP) {

				// Create temp
				UEFI_IFR_TO_LOWER temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Convert to Lower";
			}
			else if (buffer[j] == EFI_IFR_TO_UPPER_OP) {

				// Create temp
				UEFI_IFR_TO_UPPER temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Convert to Upper";
			}
			else if (buffer[j] == EFI_IFR_MAP_OP) {

				// Create temp
				UEFI_IFR_MAP temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Map";
			}
			else if (buffer[j] == EFI_IFR_ORDERED_LIST_OP) {

				// Create temp
				UEFI_IFR_ORDERED_LIST temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.question.statement.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.question.statement.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.question.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8));
				temp.question.varStoreId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 8]) + (static_cast<unsigned char>(buffer[j + 9]) << 8));
				temp.question.varOffset = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.question.flags = static_cast<unsigned char>(buffer[j + 12]);
				temp.minimumContainers = static_cast<unsigned char>(buffer[j + 13]);
				temp.flags = static_cast<unsigned char>(buffer[j + 14]);

				// Display temp
				fout << "Ordered List: " << strings[temp.question.statement.promptString + strPackageOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset << ", VarStore: 0x" << temp.question.varStoreId << ", QuestionId: 0x" << temp.question.questionId;
			}
			else if (buffer[j] == EFI_IFR_VARSTORE_OP) {

				// Create temp
				EFI_IFR_VARSTORE *temp = (EFI_IFR_VARSTORE*)&buffer[j];

				// Display temp
				fout << "VarStore: VarStoreId: 0x" << hex << uppercase << temp->VarStoreId << " [" << GuidToString(temp->Guid) << "]" << ", Size: " << hex << uppercase << temp->Size << ", Name: " << temp->Name;
			}
			else if (buffer[j] == EFI_IFR_VARSTORE_NAME_VALUE_OP) {

				// Create temp
				EFI_IFR_VARSTORE_NAME_VALUE *temp = (EFI_IFR_VARSTORE_NAME_VALUE*)&buffer[j];

				// Display temp
				fout << "VarStoreNameValue: VarStoreId: 0x" << hex << uppercase << temp->VarStoreId << " [" << GuidToString(temp->Guid) << "]";
			}
			else if (buffer[j] == EFI_IFR_VARSTORE_EFI_OP) {

				// Create temp
				EFI_IFR_VARSTORE_EFI *temp = (EFI_IFR_VARSTORE_EFI*)&buffer[j];

				// Display temp
				fout << "VarStoreEFI: VarStoreId: 0x" << hex << uppercase << temp->VarStoreId << " [" << GuidToString(temp->Guid) << "]" << ", Attrubutes: " << hex << uppercase << temp->Attributes << ", Size: " << hex << uppercase << temp->Size << ", Name: " << temp->Name;
			}
			else if (buffer[j] == EFI_IFR_VARSTORE_DEVICE_OP) {

				// Create temp
				EFI_IFR_VARSTORE_DEVICE *temp = (EFI_IFR_VARSTORE_DEVICE*)&buffer[j];

				// Display temp
				fout << "VarStoreDevice: " << strings[temp->DevicePath + strPackageOffset];
			}
			else if (buffer[j] == EFI_IFR_VERSION_OP) {

				// Create temp
				UEFI_IFR_VERSION temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Version";
			}
			else if (buffer[j] == EFI_IFR_END_OP) {

				// Create temp
				UEFI_IFR_END temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.condition = conditionalStack.back();

				// Decrement number of tabs
				numberOfTabs--;

				// Display tabs
				for (uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Decrement conditional stack
				conditionalStack.pop_back();

				// Display temp
				fout << "End";
				if (temp.condition == FORM_SET)
					fout << " Form Set";
				else if (temp.condition == FORM)
					fout << " Form";
				else if (temp.condition == CONDITION)
					fout << " If";
				else if (temp.condition == OPTION)
					fout << " of Options";
			}
			else if (buffer[j] == EFI_IFR_MATCH_OP) {

				// Create temp
				UEFI_IFR_MATCH temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Match";
			}
			else if (buffer[j] == EFI_IFR_GET_OP) {

				// Create temp
				UEFI_IFR_GET temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.varStoreId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.varOffset = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.varStoreType = static_cast<unsigned char>(buffer[j + 6]);

				// Display temp
				fout << "Get: 0x" << hex << uppercase << temp.varOffset;
			}
			else if (buffer[j] == EFI_IFR_SET_OP) {

				// Create temp
				UEFI_IFR_SET temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.varStoreId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.varOffset = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.varStoreType = static_cast<unsigned char>(buffer[j + 6]);

				// Display temp
				fout << "Set: 0x" << hex << uppercase << temp.varOffset;
			}
			else if (buffer[j] == EFI_IFR_READ_OP) {

				// Create temp
				UEFI_IFR_READ temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Read";
			}
			else if (buffer[j] == EFI_IFR_WRITE_OP) {

				// Create temp
				UEFI_IFR_WRITE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Write";
			}
			else if (buffer[j] == EFI_IFR_EQUAL_OP) {

				// Create temp
				UEFI_IFR_EQUAL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Equal";
			}
			else if (buffer[j] == EFI_IFR_NOT_EQUAL_OP) {

				// Create temp
				UEFI_IFR_NOT_EQUAL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Not Equal";
			}
			else if (buffer[j] == EFI_IFR_GREATER_THAN_OP) {

				// Create temp
				UEFI_IFR_GREATER_THAN temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Greater Than";
			}
			else if (buffer[j] == EFI_IFR_GREATER_EQUAL_OP) {

				// Create temp
				UEFI_IFR_GREATER_EQUAL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Greater Than or Equal";
			}
			else if (buffer[j] == EFI_IFR_LESS_THAN_OP) {

				// Create temp
				UEFI_IFR_LESS_THAN temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Less Than";
			}
			else if (buffer[j] == EFI_IFR_LESS_EQUAL_OP) {

				// Create temp
				UEFI_IFR_LESS_EQUAL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Less Than or Equal";
			}
			else if (buffer[j] == EFI_IFR_BITWISE_AND_OP) {

				// Create temp
				UEFI_IFR_BITWISE_AND temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Bitwise And";
			}
			else if (buffer[j] == EFI_IFR_BITWISE_OR_OP) {

				// Create temp
				UEFI_IFR_BITWISE_OR temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Bitwise Or";
			}
			else if (buffer[j] == EFI_IFR_BITWISE_NOT_OP) {

				// Create temp
				UEFI_IFR_BITWISE_NOT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Bitwise Not";
			}
			else if (buffer[j] == EFI_IFR_SHIFT_LEFT_OP) {

				// Create temp
				UEFI_IFR_SHIFT_LEFT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Shift Left";
			}
			else if (buffer[j] == EFI_IFR_SHIFT_RIGHT_OP) {

				// Create temp
				UEFI_IFR_SHIFT_RIGHT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Shift Right";
			}
			else if (buffer[j] == EFI_IFR_ADD_OP) {

				// Create temp
				UEFI_IFR_ADD temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Add";
			}
			else if (buffer[j] == EFI_IFR_SUBTRACT_OP) {

				// Create temp
				UEFI_IFR_SUBTRACT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Subtract";
			}
			else if (buffer[j] == EFI_IFR_MULTIPLY_OP) {

				// Create temp
				UEFI_IFR_MULTIPLY temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Multiply";
			}
			else if (buffer[j] == EFI_IFR_DIVIDE_OP) {

				// Create temp
				UEFI_IFR_DIVIDE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Divide";
			}
			else if (buffer[j] == EFI_IFR_MODULO_OP) {

				// Create temp
				UEFI_IFR_MODULO temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display temp
				fout << "Modulo";
			}
			else if (buffer[j] == EFI_IFR_RULE_REF_OP) {

				// Create temp
				UEFI_IFR_RULE_REF temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.ruleId = static_cast<unsigned char>(buffer[j + 2]);

				// Display temp
				fout << "Rule Ref";
			}
			else if (buffer[j] == EFI_IFR_QUESTION_REF1_OP) {

				// Create temp
				UEFI_IFR_QUESTION_REF1 temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));

				// Display temp
				fout << "Question Ref1: 0x" << hex << uppercase << temp.questionId;
			}
			else if (buffer[j] == EFI_IFR_QUESTION_REF2_OP) {

				// Create temp
				EFI_IFR_QUESTION_REF2 *temp = (EFI_IFR_QUESTION_REF2*)&buffer[j];

				// Display temp
				fout << "Question Ref2";
			}
			else if (buffer[j] == EFI_IFR_UINT8_OP) {

				// Create temp
				EFI_IFR_UINT8 *temp = (EFI_IFR_UINT8*)&buffer[j];

				// Display temp
				fout << "8 Bit Unsigned Int: 0x" << hex << uppercase << (uint16_t)temp->Value;
			}
			else if (buffer[j] == EFI_IFR_UINT16_OP) {

				// Create temp
				EFI_IFR_UINT16 *temp = (EFI_IFR_UINT16*)&buffer[j];

				// Display temp
				fout << "16 Bit Unsigned Int: 0x" << hex << uppercase << temp->Value;
			}
			else if (buffer[j] == EFI_IFR_UINT32_OP) {

				// Create temp
				EFI_IFR_UINT32 *temp = (EFI_IFR_UINT32*)&buffer[j];

				// Display temp
				fout << "32 Bit Unsigned Int: 0x" << hex << uppercase << temp->Value;
			}
			else if (buffer[j] == EFI_IFR_UINT64_OP) {

				// Create temp
				EFI_IFR_UINT64 *temp = (EFI_IFR_UINT64*)&buffer[j];

				// Display temp
				fout << "64 Bit Unsigned Int: 0x" << hex << uppercase << temp->Value;
			}
			else if (buffer[j] == EFI_IFR_TRUE_OP) {

				// Create temp
				EFI_IFR_TRUE *temp = (EFI_IFR_TRUE*)&buffer[j];

				// Display temp
				fout << "True";
			}
			else if (buffer[j] == EFI_IFR_FALSE_OP) {

				// Create temp
				EFI_IFR_FALSE *temp = (EFI_IFR_FALSE*)&buffer[j];

				// Display temp
				fout << "False";
			}
			else if (buffer[j] == EFI_IFR_TO_UINT_OP) {

				// Create temp
				EFI_IFR_TO_UINT *temp = (EFI_IFR_TO_UINT*)&buffer[j];

				// Display temp
				fout << "Convert to Unsigned Int";
			}
			else if (buffer[j] == EFI_IFR_TO_STRING_OP) {

				// Create temp
				EFI_IFR_TO_STRING *temp = (EFI_IFR_TO_STRING*)&buffer[j];

				// Display temp
				fout << "Convert to String: Format: 0x" << hex << uppercase << (uint16_t)temp->Format;
			}
			else if (buffer[j] == EFI_IFR_TO_BOOLEAN_OP) {

				// Create temp
				EFI_IFR_TO_BOOLEAN *temp = (EFI_IFR_TO_BOOLEAN*)&buffer[j];

				// Display temp
				fout << "Boolean";
			}
			else if (buffer[j] == EFI_IFR_MID_OP) {

				// Create temp
				EFI_IFR_MID *temp = (EFI_IFR_MID*)&buffer[j];

				// Display temp
				fout << "Mid";
			}
			else if (buffer[j] == EFI_IFR_FIND_OP) {

				// Create temp
				EFI_IFR_FIND *temp = (EFI_IFR_FIND*)&buffer[j];

				// Display temp
				fout << "Find: Format: 0x" << hex << uppercase << (uint16_t)temp->Format;
			}
			else if (buffer[j] == EFI_IFR_TOKEN_OP) {

				// Create temp
				EFI_IFR_TOKEN *temp = (EFI_IFR_TOKEN*)&buffer[j];

				// Display temp
				fout << "Token";
			}
			else if (buffer[j] == EFI_IFR_STRING_REF1_OP) {

				// Create temp
				EFI_IFR_STRING_REF1 *temp = (EFI_IFR_STRING_REF1*)&buffer[j];

				// Display temp
				fout << "String Ref: " << strings[temp->StringId + strPackageOffset] << " (0x" << hex << uppercase << temp->StringId << ")";
			}
			else if (buffer[j] == EFI_IFR_STRING_REF2_OP) {

				// Create temp
				EFI_IFR_STRING_REF2 *temp = (EFI_IFR_STRING_REF2*)&buffer[j];

				// Display temp
				fout << "String Ref2";
			}
			else if (buffer[j] == EFI_IFR_CONDITIONAL_OP) {

				// Create temp
				EFI_IFR_CONDITIONAL *temp = (EFI_IFR_CONDITIONAL*)&buffer[j];

				// Display temp
				fout << "Conditional";
			}
			else if (buffer[j] == EFI_IFR_QUESTION_REF3_OP) {

				// Create temp
				EFI_IFR_QUESTION_REF3 *temp = (EFI_IFR_QUESTION_REF3*)&buffer[j];

				// Display temp
				fout << "Question Ref3";
			}
			else if (buffer[j] == EFI_IFR_ZERO_OP) {

				// Create temp
				EFI_IFR_ZERO *temp = (EFI_IFR_ZERO*)&buffer[j];

				// Display temp
				fout << "Zero";
			}
			else if (buffer[j] == EFI_IFR_ONE_OP) {

				// Create temp
				EFI_IFR_ONE *temp = (EFI_IFR_ONE*)&buffer[j];

				// Display temp
				fout << "One";
			}
			else if (buffer[j] == EFI_IFR_ONES_OP) {

				// Create temp
				EFI_IFR_ONES *temp = (EFI_IFR_ONES*)&buffer[j];

				// Display temp
				fout << "Ones";
			}
			else if (buffer[j] == EFI_IFR_UNDEFINED_OP) {

				// Create temp
				EFI_IFR_UNDEFINED *temp = (EFI_IFR_UNDEFINED*)&buffer[j];

				// Display temp
				fout << "Undefined";
			}
			else if (buffer[j] == EFI_IFR_LENGTH_OP) {

				// Create temp
				EFI_IFR_LENGTH *temp = (EFI_IFR_LENGTH*)&buffer[j];

				// Display temp
				fout << "Length";
			}
			else if (buffer[j] == EFI_IFR_DUP_OP) {

				// Create temp
				EFI_IFR_DUP *temp = (EFI_IFR_DUP*)&buffer[j];

				// Display temp
				fout << "Dup";
			}
			else if (buffer[j] == EFI_IFR_THIS_OP) {

				// Create temp
				EFI_IFR_THIS *temp = (EFI_IFR_THIS*)&buffer[j];

				// Display temp
				fout << "This";
			}
			else if (buffer[j] == EFI_IFR_SPAN_OP) {

				// Create temp
				EFI_IFR_SPAN *temp = (EFI_IFR_SPAN*)&buffer[j];

				// Display temp
				fout << "Span: Flags: 0x" << hex << uppercase << (uint16_t)temp->Flags;
			}
			else if (buffer[j] == EFI_IFR_VALUE_OP) {

				// Create temp
				EFI_IFR_VALUE *temp = (EFI_IFR_VALUE*)&buffer[j];

				// Display temp
				fout << "Value";
			}
			else if (buffer[j] == EFI_IFR_DEFAULT_OP) {

				// Create temp
				EFI_IFR_DEFAULT *temp = (EFI_IFR_DEFAULT*)&buffer[j];

				// Display temp
				fout << "Default: DefaultId: 0x" << hex << uppercase << temp->DefaultId << ", Value ";

				if (temp->Type == EFI_IFR_TYPE_NUM_SIZE_8)
					fout << "(8 bit): 0x" << hex << uppercase << (uint16_t)temp->Value.u8;
				else if (temp->Type == EFI_IFR_TYPE_NUM_SIZE_16)
					fout << "(16 bit): 0x" << hex << uppercase << temp->Value.u16;
				else if (temp->Type == EFI_IFR_TYPE_NUM_SIZE_32)
					fout << "(32 bit): 0x" << hex << uppercase << temp->Value.u32;
				else if (temp->Type == EFI_IFR_TYPE_NUM_SIZE_64)
					fout << "(64 bit): 0x" << hex << uppercase << temp->Value.u64;
				else if (temp->Type == EFI_IFR_TYPE_BOOLEAN)
					fout << "(Bool): " << (temp->Value.b ? "true" : "false");
				else if (temp->Type == EFI_IFR_TYPE_TIME)
					fout << "(Time)";
				else if (temp->Type == EFI_IFR_TYPE_DATE)
					fout << "(Date)";
				else if (temp->Type == EFI_IFR_TYPE_STRING)
					fout << "(String): " << strings[temp->Value.string + strPackageOffset];
				else if (temp->Type == EFI_IFR_TYPE_OTHER)
					fout << "(Other)";
				else if (temp->Type == EFI_IFR_TYPE_UNDEFINED)
					fout << "(Undefined)";
				else if (temp->Type == EFI_IFR_TYPE_ACTION)
					fout << "(Action)";
				else if (temp->Type == EFI_IFR_TYPE_BUFFER)
					fout << "(Buffer)";
				else if (temp->Type == EFI_IFR_TYPE_REF)
					fout << "(Ref)";
			}
			else if (buffer[j] == EFI_IFR_DEFAULTSTORE_OP) {

				// Create temp
				EFI_IFR_DEFAULTSTORE *temp = (EFI_IFR_DEFAULTSTORE*)&buffer[j];

				// Display temp
				fout << "Default Store: " << strings[temp->DefaultName + strPackageOffset] << ", Id: 0x" << hex << uppercase << temp->DefaultId;
			}
			else if (buffer[j] == EFI_IFR_FORM_MAP_OP) {

				// Create temp
				EFI_IFR_FORM_MAP *temp = (EFI_IFR_FORM_MAP*)&buffer[j];

				// Display temp
				fout << "Form Map: 0x" << hex << uppercase << temp->FormId;
			}
			else if (buffer[j] == EFI_IFR_CATENATE_OP) {

				// Create temp
				EFI_IFR_CATENATE *temp = (EFI_IFR_CATENATE*)&buffer[j];

				// Display temp
				fout << "Catenate";
			}
			else if (buffer[j] == EFI_IFR_GUID_OP) {

				// Create temp
				EFI_IFR_GUID *temp = (EFI_IFR_GUID*)&buffer[j];

				// Display temp
				fout << "Guid: [" << GuidToString(temp->Guid) << "]";
				// TODO: optional data?
			}
			else if (buffer[j] == EFI_IFR_SECURITY_OP) {

				// Create temp
				EFI_IFR_SECURITY *temp = (EFI_IFR_SECURITY*)&buffer[j];

				// Display temp
				fout << "Security: Permissions: [" << GuidToString(temp->Permissions) << "]";
			}
			else if (buffer[j] == EFI_IFR_MODAL_TAG_OP) {

				// Create temp
				EFI_IFR_MODAL_TAG *temp = (EFI_IFR_MODAL_TAG*)&buffer[j];

				// Display temp
				fout << "Modal Tag";
			}
			else if (buffer[j] == EFI_IFR_REFRESH_ID_OP) {

				// Create temp
				EFI_IFR_REFRESH_ID *temp = (EFI_IFR_REFRESH_ID*)&buffer[j];

				// Display temp
				fout << "Refresh Id: [" << GuidToString(temp->RefreshEventGroupId) << "]";
			}
			else if (buffer[j] == EFI_IFR_WARNING_IF_OP) {

				// Create temp
				EFI_IFR_WARNING_IF *temp = (EFI_IFR_WARNING_IF*)&buffer[j];

				// Display temp
				fout << "Warning If: " << strings[temp->Warning + strPackageOffset] << ", TimeOut: 0x" << hex << uppercase << temp->TimeOut;
			}
			else {

				// Create temp
				EFI_IFR_OP_HEADER *temp = (EFI_IFR_OP_HEADER*)&buffer[j];

				// Display temp
				fout << "Unknown EFI_IFR OpCode: 0x" << hex << uppercase << (uint16_t)temp->OpCode << ", Length: " << temp->Length << ", Scope: " << temp->Scope;
			}

			EFI_IFR_OP_HEADER *opHdr = (EFI_IFR_OP_HEADER*)&buffer[j];
			if (opHdr->Scope) {
				if (opHdr->OpCode == EFI_IFR_FORM_OP)
					IndentText(FORM);
				else if (opHdr->OpCode == EFI_IFR_ONE_OF_OP)
					IndentText(OPTION);
				else if (opHdr->OpCode == EFI_IFR_SUPPRESS_IF_OP || opHdr->OpCode == EFI_IFR_NO_SUBMIT_IF_OP || opHdr->OpCode == EFI_IFR_INCONSISTENT_IF_OP || opHdr->OpCode == EFI_IFR_GRAY_OUT_IF_OP || opHdr->OpCode == EFI_IFR_DISABLE_IF_OP || opHdr->OpCode == EFI_IFR_WARNING_IF_OP)
					IndentText(CONDITION);
				else if (opHdr->OpCode == EFI_IFR_FORM_SET_OP)
					IndentText(FORM_SET);
				else
					IndentText(OTHER);
			}

			// Display code
			fout << " {";
			for (int k = 0; k < static_cast<unsigned char>(buffer[j + 1] & 0x7F); k++) {
				fout << hex << uppercase << setw(2) << setfill('0') << static_cast<unsigned int>(static_cast<unsigned char>(buffer[j + k]));
				if (k != static_cast<unsigned char>(buffer[j + 1] & 0x7F) - 1)
					fout << ' ';
			}
			fout << '}' << endl;
		}
	}

	// Close file
	fout.close();
}
