// Header files
#include <iostream>
#include <fstream>
#include <iomanip>
#include "EFI.h"


// Supporting function implementation
void getEFIStringPackages(vector<EFI_IFR_STRING_PACK> &stringPackages, const string &buffer) {

	// Initialize variables
	EFI_IFR_STRING_PACK tempStringPackage;

	// Go through buffer
	for(unsigned int i = 0; i < buffer.size() - 9; i++)
	
		// Check if string pakage was found
		if((buffer[i] != '\x00' || buffer[i + 1] != '\x00' || buffer[i + 2] != '\x00' || buffer[i + 3] != '\x00') && buffer[i + 4] == '\x02' && buffer[i + 5] == '\x00' && (buffer[i + 6] != '\x00' || buffer[i + 7] != '\x00' || buffer[i + 8] != '\x00' || buffer[i + 9] != '\x00') && i + static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24) + 4] >= 'a' && buffer[i + static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24) + 4] <= 'z' && i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) + 4] == '\x02' && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) + 5] == '\x00') {
	
			// Set temp string package
			tempStringPackage.header.offset = i;

			tempStringPackage.header.length = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24));

			tempStringPackage.header.type = static_cast<uint16_t>(static_cast<unsigned char>(buffer[i + 4]) + (static_cast<unsigned char>(buffer[i + 5]) << 8));

			for(uint32_t j = i + static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24)); buffer[j] != '\x00'; j += 2)
				tempStringPackage.language.push_back(buffer[j]);

			tempStringPackage.numberOfStrings = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 14]) + (static_cast<unsigned char>(buffer[i + 15]) << 8) + (static_cast<unsigned char>(buffer[i + 16]) << 16) + (static_cast<unsigned char>(buffer[i + 17]) << 24));

			tempStringPackage.attributes = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 18]) + (static_cast<unsigned char>(buffer[i + 19]) << 8) + (static_cast<unsigned char>(buffer[i + 20]) << 16) + (static_cast<unsigned char>(buffer[i + 21]) << 24));

			// Add string package to list
			stringPackages.push_back(tempStringPackage);

			// Clear temp string package
			tempStringPackage.language.clear();
		}
}

void displayEFIStringPackages(const vector<EFI_IFR_STRING_PACK> &stringPackages) {

	// Display string packages info
	cout << "String Packages Information:" << endl;

	// Go through string packages
	for(unsigned int i = 0; i < stringPackages.size(); i++) {

		// Display string package info
		cout << "\tString Package " << i << " Information:" << endl;
		cout << "\t\tOffset: " << dec << stringPackages[i].header.offset << hex << uppercase << " (0x" << stringPackages[i].header.offset << ')' << endl;
		cout << "\t\tLength: " << dec << stringPackages[i].header.length << hex << uppercase << " (0x" << stringPackages[i].header.length << ')' << endl;
		cout << "\t\tLanguage: " << stringPackages[i].language << endl;
		cout << "\t\tNumber of strings: " << dec << stringPackages[i].numberOfStrings << hex << uppercase << " (0x" << stringPackages[i].numberOfStrings << ')' << endl;
		cout << "\t\tAttributes: " << dec << stringPackages[i].attributes << hex << uppercase << " (0x" << stringPackages[i].attributes << ')' << endl;
	}

	cout << endl;
}

void getEFIStrings(vector<EFI_IFR_STRING_PACK> &stringPackages, vector<string> &strings, const string &buffer) {

	// Initialize variables
	int index;
	string temp;

	// Go through string packages
	for(unsigned int i = 0; i < stringPackages.size(); i++) {

		// Check if language isn't english
		if(stringPackages[i].language != "eng")

			// Continue
			continue;
		
		// Set structure offset
		stringPackages[i].structureOffset = strings.size();

		// Set index
		index = stringPackages[i].header.offset + 22;

		// Go through strings
		for(uint32_t j = 0; j < stringPackages[i].numberOfStrings; j++) {

			// Fill string
			for(uint32_t k = stringPackages[i].header.offset + static_cast<uint32_t>(static_cast<unsigned char>(buffer[index]) + (static_cast<unsigned char>(buffer[index + 1]) << 8) + (static_cast<unsigned char>(buffer[index + 2]) << 16) + (static_cast<unsigned char>(buffer[index + 3]) << 24)); buffer[k] != '\x00'; k += 2)
				temp.push_back(buffer[k]);

			// Add string to list
			strings.push_back(temp);

			// Clear temp
			temp.clear();

			// Increment index
			index += 4;
		}
	}

	// Go through strings
	for(unsigned int i = 0; i < strings.size(); i++) {

		// Change carriage return characters to spaces
		for(unsigned int j = 0; j < strings[i].size(); j++)
			if(strings[i][j] == '\r')
				strings[i][j] = ' ';

		// Remove leading spaces
		while(strings[i].size() > 1 && strings[i][0] == ' ')
			strings[i].erase(0, 1);

		// Change double spaces
		for(unsigned int j = 0; strings[i].size() && j < strings[i].size() - 1; j++)
			if(strings[i][j] == ' ' && strings[i][j + 1] == ' ')
				strings[i].erase(j, 1);
	}
}

void displayEFIStrings(const vector<string> &strings) {

	// Print strings
	cout << "Strings:" << endl;

	// Go through strings
	for(uint32_t i = 0; i < strings.size(); i++)

		// Print string
		cout << '\t' << dec << i << " (0x" << hex << uppercase << i << ") " << strings[i] << endl;

	cout << endl;
}

void getEFIFormSets(vector<EFI_IFR_FORM_SET_PACK> &formSets, const string &buffer, const vector<EFI_IFR_STRING_PACK> &stringPackages, const vector<string> &strings) {

	// Initialize variables
	EFI_IFR_FORM_SET_PACK tempFormSet;
	vector<uint32_t> stringCandidates;
	uint32_t chosenCandidate;
	
	// Go through string packages
	for(unsigned int i = 0; i < stringPackages.size(); i++) {

		// Check if language isn't english
		if(stringPackages[i].language != "eng")

			// Continue
			continue;
		
		// Get string candidate
		stringCandidates.push_back(i);
	}

	// Go through buffer
	for(uint32_t i = 0; i < buffer.size() - 5; i++)

		// Check if form set was found
		if((buffer[i] != '\x00' || buffer[i + 1] != '\x00' || buffer[i + 2] != '\x00' || buffer[i + 3] != '\x00') && buffer[i + 4] == '\x03' && buffer[i + 5] == '\x00' && i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) - 1] == '\x02' && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) - 2] == '\x0D') {
		
			// Get chosen candidate for form set's string package
			chosenCandidate = stringCandidates[0];
			for(unsigned int j = 1; j < stringCandidates.size(); j++)
				if(abs(static_cast<signed>(stringPackages[j].header.offset - i)) < abs(static_cast<signed>(stringPackages[chosenCandidate].header.offset - i)))
					chosenCandidate = j;

			// Set temp form set
			tempFormSet.header.offset = i;
			tempFormSet.header.length = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24));
			tempFormSet.header.type = static_cast<uint16_t>(static_cast<unsigned char>(buffer[i + 4]) + (static_cast<unsigned char>(buffer[i + 5]) << 8));
			tempFormSet.titleString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[i + 24]) + (static_cast<unsigned char>(buffer[i + 25]) << 8));
			tempFormSet.usingStringPackage = stringCandidates[chosenCandidate];
			tempFormSet.title = strings[tempFormSet.titleString + stringPackages[tempFormSet.usingStringPackage].structureOffset];

			// Add temp form set to list
			formSets.push_back(tempFormSet);
		}
}

void displayEFIFormSets(const vector<EFI_IFR_FORM_SET_PACK> &formSets) {

	// Print form sets information
	cout << "Form Sets Information:" << endl;

	// Go through form sets
	for(uint32_t i = 0; i < formSets.size(); i++) {

		// Print form set information
		cout << "\tForm Set " << i << " Information:" << endl;
		cout << "\t\tOffset: " << dec << formSets[i].header.offset << hex << uppercase << " (0x" << formSets[i].header.offset << ')' << endl;
		cout << "\t\tLength: " << dec << formSets[i].header.length << hex << uppercase << " (0x" << formSets[i].header.length << ')' << endl;
		cout << "\t\tTitle ID: " << dec << formSets[i].titleString << hex << uppercase << " (0x" << formSets[i].titleString << ')' << endl;
		cout << "\t\tTitle: " << formSets[i].title << endl;
	}

	cout << endl;
}

void generateEFIIFRDump(const string &outputFile, const vector<EFI_IFR_STRING_PACK> &stringPackages, const vector<EFI_IFR_FORM_SET_PACK> &formSets, const string &buffer, const vector<string> &strings) {

	// Initialize variables
	uint8_t numberOfTabs = 0;
	uint8_t numberOfIfs = 0;
	unsigned char highNibble, lowNibble;
	ofstream fout(outputFile.c_str());

	// Display protocol
	for(uint8_t i = 0; i < 32; i++)
		fout << ' ';
	fout << "EFI Protocol Detected" << endl;
	for(uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl << endl << endl;

	// Display string packages info
	for(uint8_t i = 0; i < 32; i++)
		fout << ' ';
	fout << "String Packages" << endl;
	for(uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl;
	fout << "Offset:\t\tLanguage:" << endl;
	for(uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl;
	for(uint8_t i = 0; i < stringPackages.size(); i++) {
		fout << "0x" << hex << uppercase << stringPackages[i].header.offset;
		fout << "\t\t" << stringPackages[i].language << " (0x" << hex << uppercase << static_cast<unsigned int>(i) << ')' << endl;
	}
	fout << endl << endl;

	// Display form sets info
	for(uint8_t i = 0; i < 35; i++)
		fout << ' ';
	fout << "Form Sets" << endl;
	for(uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl;
	fout << "Offset:\t\tTitle:" << endl;
	for(uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl;
	for(uint8_t i = 0; i < formSets.size(); i++) {
		fout << "0x" << hex << uppercase << formSets[i].header.offset;
		fout << "\t\t" << formSets[i].title << " (0x" << hex << uppercase << formSets[i].titleString << " from string package 0x" << formSets[i].usingStringPackage << ')' << endl;
	}
	fout << endl << endl;

	// Display internal forms representation
	for(uint8_t i = 0; i < 25; i++)
		fout << ' ';
	fout << "Internal Forms Representation" << endl;
	for(uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl;
	fout << "Offset:\t\tInstruction:" << endl;
	for(uint8_t i = 0; i < 80; i++)
		fout << '-';
	fout << endl;

	// Go through form sets
	for(uint32_t i = 0; i < formSets.size(); i++)

		// Go through all the form set's operations
		for(uint32_t j = formSets[i].header.offset + 6; j < formSets[i].header.offset + formSets[i].header.length; j += static_cast<unsigned char>(buffer[j + 1])) {

			// Check if opcode is EFI_IFR_FORM_OP 0x01
			if(buffer[j] == EFI_IFR_FORM_OP) {

				// Create temp
				EFI_IFR_FORM temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.formId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.titleString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Form: " << strings[temp.titleString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Form ID: 0x" << hex << uppercase << temp.formId;

				// Increment number of tabs
				numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_SUBTITLE_OP 0x02
			else if(buffer[j] == EFI_IFR_SUBTITLE_OP) {

				// Create temp
				EFI_IFR_SUBTITLE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.subtitleString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Subtitle: " << strings[temp.subtitleString + stringPackages[formSets[i].usingStringPackage].structureOffset];
			}

			// Otherwise check if opcode is EFI_IFR_TEXT_OP 0x03
			else if(buffer[j] == EFI_IFR_TEXT_OP) {

				// Create temp
				EFI_IFR_TEXT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.primaryTextString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.secondaryTextString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8));
				temp.flags = static_cast<unsigned char>(buffer[j + 8]);
				temp.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 9]) + (static_cast<unsigned char>(buffer[j + 10]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Text: " << strings[temp.primaryTextString + stringPackages[formSets[i].usingStringPackage].structureOffset];
			}

			// Otherwise chec if opcode is EFI_IFR_GRAPHIC_OP 0x04
			else if(buffer[j] == EFI_IFR_GRAPHIC_OP) {

				// Display offset
				fout << "0x" << hex << uppercase << j << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Graphic:";
			}

			// Otherwise check if opcode is EFI_IFR_ONE_OF_OP 0x05
			else if(buffer[j] == EFI_IFR_ONE_OF_OP) {

				// Create temp
				EFI_IFR_ONE_OF temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				temp.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Setting: " << strings[temp.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.questionId << '[' << dec << static_cast<unsigned int>(temp.width) << ']';

				// Increment number of tabs
				numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_CHECKBOX_OP 0x06
			else if(buffer[j] == EFI_IFR_CHECKBOX_OP) {

				// Create temp
				EFI_IFR_CHECKBOX temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				temp.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));
				temp.flags = static_cast<unsigned char>(buffer[j + 9]);
				temp.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Checkbox: " << strings[temp.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.questionId << '[' << dec << static_cast<unsigned int>(temp.width) << ']';
			}

			// Otherwise check if opcode is EFI_IFR_NUMERIC_OP 0x07
			else if(buffer[j] == EFI_IFR_NUMERIC_OP) {

				// Create temp
				EFI_IFR_NUMERIC temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				temp.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));
				temp.flags = static_cast<unsigned char>(buffer[j + 9]);
				temp.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.maximum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 12]) + (static_cast<unsigned char>(buffer[j + 13]) << 8));
				temp.minimum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 14]) + (static_cast<unsigned char>(buffer[j + 15]) << 8));
				temp.step = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 16]) + (static_cast<unsigned char>(buffer[j + 17]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Numeric: " << strings[temp.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << " (" << dec << temp.minimum << '-' << temp.maximum << "), Variable: 0x" << hex << uppercase << temp.questionId << '[' << dec << static_cast<unsigned int>(temp.width) << ']';
			}

			// Otherwise check if opcode is EFI_IFR_PASSWORD_OP 0x08
			else if(buffer[j] == EFI_IFR_PASSWORD_OP) {

				// Create temp
				EFI_IFR_PASSWORD temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				temp.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));
				temp.flags = static_cast<unsigned char>(buffer[j + 9]);
				temp.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.minimumSize = static_cast<unsigned char>(buffer[j + 12]);
				temp.maximumSize = static_cast<unsigned char>(buffer[j + 13]);
				temp.encoding = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 14]) + (static_cast<unsigned char>(buffer[j + 15]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Password: " << strings[temp.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.questionId << '[' << dec << static_cast<unsigned int>(temp.width) << ']';
			}

			// Otherwise check if opcode is EFI_IFR_ONE_OF_OPTION_OP 0x09
			else if(buffer[j] == EFI_IFR_ONE_OF_OPTION_OP) {

				// Create temp
				EFI_IFR_ONE_OF_OPTION temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.optionString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.value = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.flags = static_cast<unsigned char>(buffer[j + 6]);
				temp.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Option: " << strings[temp.optionString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Value: 0x" << hex << uppercase << temp.value;
			}

			// Otherwise check if opcode is EFI_IFR_SUPPRESS_IF_OP 0x0A
			else if(buffer[j] == EFI_IFR_SUPPRESS_IF_OP) {

				// Create temp
				EFI_IFR_SUPPRESS temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.flags = static_cast<unsigned char>(buffer[j + 2]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Suppress If:";

				// Increment number of ifs
				numberOfIfs++;
			}

			// Otherwise check if opcode is EFI_IFR_END_FORM_OP 0x0B
			else if(buffer[j] == EFI_IFR_END_FORM_OP) {

				// Create temp
				EFI_IFR_END_FORM temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);

				// Decrement number of tabs
				numberOfTabs--;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "End Form";
			}

			// Otherwise check if opcode is EFI_IFR_HIDDEN_OP 0x0C
			else if(buffer[j] == EFI_IFR_HIDDEN_OP) {

				// Create temp
				EFI_IFR_HIDDEN temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.value = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Hidden:";
			}

			// Otherwise check if opcode is EFI_IFR_END_FORM_SET_OP 0x0D
			else if(buffer[j] == EFI_IFR_END_FORM_SET_OP) {

				// Create temp
				EFI_IFR_END_FORM_SET temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);

				// Decrement number of tabs
				numberOfTabs--;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "End Form Set";
			}

			// Otherwise check if opcode is EFI_IFR_FORM_SET_OP 0x0E
			else if(buffer[j] == EFI_IFR_FORM_SET_OP) {

				// Create temp
				EFI_IFR_FORM_SET temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				for(uint32_t k = j + 2; k < j + 6; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.guid.insert(0, 1, lowNibble);
					temp.guid.insert(0, 1, highNibble);
				}
				temp.guid.push_back('-');
				for(uint32_t k = j + 6; k < j + 8; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.guid.insert(9, 1, lowNibble);
					temp.guid.insert(9, 1, highNibble);
				}
				temp.guid.push_back('-');
				for(uint32_t k = j + 8; k < j + 10; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.guid.insert(14, 1, lowNibble);
					temp.guid.insert(14, 1, highNibble);
				}
				temp.guid.push_back('-');
				for(uint32_t k = j + 10; k < j + 12; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.guid.push_back(highNibble);
					temp.guid.push_back(lowNibble);
				}
				temp.guid.push_back('-');
				for(uint32_t k = j + 12; k < j + 18; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.guid.push_back(highNibble);
					temp.guid.push_back(lowNibble);
				}
				temp.titleString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 18]) + (static_cast<unsigned char>(buffer[j + 19]) << 8));
				temp.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 20]) + (static_cast<unsigned char>(buffer[j + 21]) << 8));
				temp.callbackHandle = static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 22]) + (static_cast<unsigned char>(buffer[j + 23]) << 8) + (static_cast<unsigned char>(buffer[j + 24]) << 16) + (static_cast<unsigned char>(buffer[j + 25]) << 24) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 26])) << 32) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 27])) << 40) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 28])) << 48) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 29])) << 56));
				temp.mainClass = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 30]) + (static_cast<unsigned char>(buffer[j + 31]) << 8));
				temp.subClass = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 32]) + (static_cast<unsigned char>(buffer[j + 33]) << 8));
				temp.nvDataSize = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 34]) + (static_cast<unsigned char>(buffer[j + 25]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Form Set: " << strings[temp.titleString + stringPackages[formSets[i].usingStringPackage].structureOffset];

				// Increment number of tabs
				numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_REF_OP 0x0F
			else if(buffer[j] == EFI_IFR_REF_OP) {

				// Create temp
				EFI_IFR_REF temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.formId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8));
				temp.flags = static_cast<uint8_t>(static_cast<unsigned char>(buffer[j + 8]));
				temp.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 9]) + (static_cast<unsigned char>(buffer[j + 10]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Selectable: " << strings[temp.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Form ID: 0x" << hex << uppercase << temp.formId;
			}

			// Otherwise check if opcode is EFI_IFR_END_OP 0x10
			else if(buffer[j] == EFI_IFR_END_ONE_OF_OP) {

				// Create temp
				EFI_IFR_END_ONE_OF temp;

				// Fill end one of
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);

				// Decrement number of tabs
				numberOfTabs--;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "End of Options";
			}

			// Otherwise check if opcode is EFI_IFR_INCONSISTENT_IF_OP 0x11
			else if(buffer[j] == EFI_IFR_INCONSISTENT_IF_OP) {

				// Create temp
				EFI_IFR_INCONSISTENT temp;

				// Fill inconsistent
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.popup = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.flags = static_cast<uint8_t>(buffer[j + 4]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display inconsistent
				fout << "Inconsistent If:";

				// Increment number of ifs
				numberOfIfs++;
			}

			// Otherwise check if opcode is EFI_IFR_EQ_ID_VAL_OP 0x12
			else if(buffer[j] == EFI_IFR_EQ_ID_VAL_OP) {

				// Create temp
				EFI_IFR_EQ_ID_VAL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.value = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Variable 0x" << hex << uppercase << temp.questionId << '[' << dec << static_cast<unsigned int>(temp.width) << ']' << " equals 0x" << hex << uppercase << temp.value;

				// Check if there isn't a following conditional statement
				if(endEFICondition(buffer, j))

					// Increment number of tabs
					numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_EQ_ID_ID_OP 0x13
			else if(buffer[j] == EFI_IFR_EQ_ID_ID_OP) {

				// Create temp
				EFI_IFR_EQ_ID_ID temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.primaryQuestionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.secondaryQuestionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Variable 0x" << hex << uppercase << temp.primaryQuestionId << '[' << dec << static_cast<unsigned int>(temp.width) << ']' << " equals variable 0x" << hex << uppercase << temp.secondaryQuestionId;

				// Check if there isn't a following conditional statement
				if(endEFICondition(buffer, j))

					// Increment number of tabs
					numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_EQ_ID_LIST_OP 0x14
			else if(buffer[j] == EFI_IFR_EQ_ID_LIST_OP) {

				// Create temp
				EFI_IFR_EQ_ID_LIST temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.listLength = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));

				// Create list
				uint32_t index = j + 7;

				for(uint8_t k = 0; k < temp.listLength; k++, index += 2)
					temp.valueList.push_back(static_cast<uint16_t>(static_cast<unsigned char>(buffer[index]) + (static_cast<unsigned char>(buffer[index + 1]) << 8)));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Variable 0x" << hex << uppercase << temp.questionId << '[' << dec << static_cast<unsigned int>(temp.width) << ']' << " equals value in list (";
				for(uint8_t k = 0; k < temp.listLength; k++) {
					fout << "0x" << hex << uppercase << temp.valueList[k];
					if(k != temp.listLength - 1)
						fout << ", ";
					else
						fout << ')';
				}

				// Check if there isn't a following conditional statement
				if(endEFICondition(buffer, j))

					// Increment number of tabs
					numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_AND_OP 0x15
			else if(buffer[j] == EFI_IFR_AND_OP) {

				// Create temp
				EFI_IFR_AND temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "And";

				// Check if there isn't a following conditional statement
				if(endEFICondition(buffer, j))

					// Increment number of tabs
					numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_OR_OP 0x16
			else if(buffer[j] == EFI_IFR_OR_OP) {

				// Create temp
				EFI_IFR_OR temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Or";

				// Check if there isn't a following conditional statement
				if(endEFICondition(buffer, j))

					// Increment number of tabs
					numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_NOT_OP 0x17
			else if(buffer[j] == EFI_IFR_NOT_OP) {

				// Create temp
				EFI_IFR_NOT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Not";

				// Check if there isn't a following conditional statement
				if(endEFICondition(buffer, j))

					// Increment number of tabs
					numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_END_IF_OP 0x18
			else if(buffer[j] == EFI_IFR_END_IF_OP) {

				// Create temp
				EFI_IFR_END_IF temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);

				// Go through ifs
				for(uint8_t k = 0; k < numberOfIfs; k++) {

					// Decrement number of tabs
					numberOfTabs--;

					// Display offset
					fout << "0x" << hex << uppercase << temp.header.offset << ' ';

					// Display tabs
					for(uint8_t l = 0; l < numberOfTabs; l++)
						fout << '\t';

					// Display temp
					fout << "End If";
				}

				// Clear number of ifs
				numberOfIfs = 0;
			}

			// Otherwise check if opcode is EFI_IFR_GRAYOUT_IF_OP 0x19
			else if(buffer[j] == EFI_IFR_GRAYOUT_IF_OP) {

				// Create temp
				EFI_IFR_GRAYOUT temp;

				// Fill grayout
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.flags = static_cast<unsigned char>(buffer[j + 2]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Grayout If:";

				// Increment number of ifs
				numberOfIfs++;
			}

			// Otherwise check if opcode is EFI_IFR_DATE_OP 0x1A
			else if(buffer[j] == EFI_IFR_DATE_OP) {

				// Create temp
				EFI_IFR_DATE temp;

				// Fill year
				temp.year.header.offset = j;
				temp.year.header.opcode = buffer[j];
				temp.year.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.year.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.year.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.year.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				temp.year.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));
				temp.year.flags = static_cast<unsigned char>(buffer[j + 9]);
				temp.year.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.year.minimum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 12]) + (static_cast<unsigned char>(buffer[j + 13]) << 8));
				temp.year.maximum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 14]) + (static_cast<unsigned char>(buffer[j + 15]) << 8));
				temp.year.step = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 15]) + (static_cast<unsigned char>(buffer[j + 16]) << 8));
				temp.year.defaultValue = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 17]) + (static_cast<unsigned char>(buffer[j + 18]) << 8));

				// Increment j
				j += static_cast<unsigned char>(buffer[j + 1]);

				// Fill month
				temp.month.header.offset = j;
				temp.month.header.opcode = buffer[j];
				temp.month.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.month.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.month.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.month.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				temp.month.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));
				temp.month.flags = static_cast<unsigned char>(buffer[j + 9]);
				temp.month.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.month.minimum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 12]) + (static_cast<unsigned char>(buffer[j + 13]) << 8));
				temp.month.maximum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 14]) + (static_cast<unsigned char>(buffer[j + 15]) << 8));
				temp.month.step = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 15]) + (static_cast<unsigned char>(buffer[j + 16]) << 8));
				temp.month.defaultValue = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 17]) + (static_cast<unsigned char>(buffer[j + 18]) << 8));

				// Increment j
				j += static_cast<unsigned char>(buffer[j + 1]);

				// Fill day
				temp.day.header.offset = j;
				temp.day.header.opcode = buffer[j];
				temp.day.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.day.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.day.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.day.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				temp.day.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));
				temp.day.flags = static_cast<unsigned char>(buffer[j + 9]);
				temp.day.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.day.minimum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 12]) + (static_cast<unsigned char>(buffer[j + 13]) << 8));
				temp.day.maximum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 14]) + (static_cast<unsigned char>(buffer[j + 15]) << 8));
				temp.day.step = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 15]) + (static_cast<unsigned char>(buffer[j + 16]) << 8));
				temp.day.defaultValue = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 17]) + (static_cast<unsigned char>(buffer[j + 18]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.year.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Date: Day (" << dec << temp.day.minimum << '-' << temp.day.maximum << "), Month (" << temp.month.minimum << '-' << temp.month.maximum << "), Year (" << temp.year.minimum << '-' << temp.year.maximum << "), Variables: 0x" << hex << uppercase << temp.day.questionId << '[' << dec << static_cast<unsigned int>(temp.day.width) << ']' << ", 0x" << hex << uppercase << temp.month.questionId << '[' << dec << static_cast<unsigned int>(temp.month.width) << ']' << ", 0x" << hex << uppercase << temp.year.questionId << '[' << dec << static_cast<unsigned int>(temp.year.width) << ']';
			}

			// Otherwise check if opcode is EFI_IFR_TIME_OP 0x1B
			else if(buffer[j] == EFI_IFR_TIME_OP) {

				// Create temp
				EFI_IFR_TIME temp;

				// Fill hour
				temp.hour.header.offset = j;
				temp.hour.header.opcode = buffer[j];
				temp.hour.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.hour.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.hour.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.hour.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				temp.hour.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));
				temp.hour.flags = static_cast<unsigned char>(buffer[j + 9]);
				temp.hour.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.hour.minimum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 12]) + (static_cast<unsigned char>(buffer[j + 13]) << 8));
				temp.hour.maximum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 14]) + (static_cast<unsigned char>(buffer[j + 15]) << 8));
				temp.hour.step = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 15]) + (static_cast<unsigned char>(buffer[j + 16]) << 8));
				temp.hour.defaultValue = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 17]) + (static_cast<unsigned char>(buffer[j + 18]) << 8));

				// Increment j
				j += static_cast<unsigned char>(buffer[j + 1]);

				// Fill minute
				temp.minute.header.offset = j;
				temp.minute.header.opcode = buffer[j];
				temp.minute.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.minute.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.minute.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.minute.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				temp.minute.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));
				temp.minute.flags = static_cast<unsigned char>(buffer[j + 9]);
				temp.minute.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.minute.minimum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 12]) + (static_cast<unsigned char>(buffer[j + 13]) << 8));
				temp.minute.maximum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 14]) + (static_cast<unsigned char>(buffer[j + 15]) << 8));
				temp.minute.step = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 15]) + (static_cast<unsigned char>(buffer[j + 16]) << 8));
				temp.minute.defaultValue = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 17]) + (static_cast<unsigned char>(buffer[j + 18]) << 8));

				// Increment j
				j += static_cast<unsigned char>(buffer[j + 1]);

				// Fill second
				temp.second.header.offset = j;
				temp.second.header.opcode = buffer[j];
				temp.second.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.second.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.second.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.second.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				temp.second.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));
				temp.second.flags = static_cast<unsigned char>(buffer[j + 9]);
				temp.second.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.second.minimum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 12]) + (static_cast<unsigned char>(buffer[j + 13]) << 8));
				temp.second.maximum = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 14]) + (static_cast<unsigned char>(buffer[j + 15]) << 8));
				temp.second.step = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 15]) + (static_cast<unsigned char>(buffer[j + 16]) << 8));
				temp.second.defaultValue = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 17]) + (static_cast<unsigned char>(buffer[j + 18]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.hour.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Time: Hour (" << dec << temp.hour.minimum << '-' << temp.hour.maximum << "), Minute (" << temp.minute.minimum << '-' << temp.minute.maximum << "), Second (" << temp.second.minimum << '-' << temp.second.maximum << "), Variables: 0x" << hex << uppercase << temp.hour.questionId << '[' << dec << static_cast<unsigned int>(temp.hour.width) << ']' << ", 0x" << hex << uppercase << temp.minute.questionId << '[' << dec << static_cast<unsigned int>(temp.minute.width) << ']' << ", 0x" << hex << uppercase << temp.second.questionId << '[' << dec << static_cast<unsigned int>(temp.second.width) << ']';
			}

			// Otherwise check if opcode is EFI_IFR_STRING_OP 0x1C
			else if(buffer[j] == EFI_IFR_STRING_OP) {

				// Create temp
				EFI_IFR_STRING temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.width = static_cast<unsigned char>(buffer[j + 4]);
				temp.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				temp.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));
				temp.flags = static_cast<unsigned char>(buffer[j + 9]);
				temp.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 10]) + (static_cast<unsigned char>(buffer[j + 11]) << 8));
				temp.minimumSize = static_cast<unsigned char>(buffer[j + 12]);
				temp.maximumSize = static_cast<unsigned char>(buffer[j + 13]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "String: " << strings[temp.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.questionId << '[' << dec << static_cast<unsigned int>(temp.width) << ']';
			}

			// Otherwise check if opcode is EFI_IFR_LABEL_OP 0x1D
			else if(buffer[j] == EFI_IFR_LABEL_OP) {

				// Create temp
				EFI_IFR_LABEL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.labelId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Label: 0x" << hex << uppercase << temp.labelId;
			}

			// Otherwise check if opcode is EFI_IFR_SAVE_DEFAULTS_OP 0x1E
			else if(buffer[j] == EFI_IFR_SAVE_DEFAULTS_OP) {

				// Create temp
				EFI_IFR_SAVE_DEFAULTS temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.formId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8));
				temp.flags = static_cast<unsigned char>(buffer[j + 8]);
				temp.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 9]) + (static_cast<unsigned char>(buffer[j + 10]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Save Defaults: " << strings[temp.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Form ID: 0x" << hex << uppercase << temp.formId;
			}

			// Otherwise check if opcode is EFI_IFR_RESTORE_DEFAULTS_OP 0x1F
			else if(buffer[j] == EFI_IFR_RESTORE_DEFAULTS_OP) {

				// Create temp
				EFI_IFR_RESTORE_DEFAULTS temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.formId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8));
				temp.flags = static_cast<unsigned char>(buffer[j + 8]);
				temp.key = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 9]) + (static_cast<unsigned char>(buffer[j + 10]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Restore Defaults: " << strings[temp.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Form ID: 0x" << hex << uppercase << temp.formId;
			}

			// Otherwise check if opcode is EFI_IFR_BANNER_OP 0x20
			else if(buffer[j] == EFI_IFR_BANNER_OP) {

				// Create temp
				EFI_IFR_BANNER temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.titleString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.lineNumber = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.alignment = static_cast<unsigned char>(buffer[j + 6]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Banner: " << strings[temp.titleString + stringPackages[formSets[i].usingStringPackage].structureOffset];
			}

			// Otherwise check if opcode is EFI_IFR_INVENTORY_OP 0x21
			else if(buffer[j] == EFI_IFR_INVENTORY_OP) {

				// Create temp
				EFI_IFR_INVENTORY temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.primaryTextString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.secondaryTextString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Inventory: " << strings[temp.primaryTextString + stringPackages[formSets[i].usingStringPackage].structureOffset];
			}

			// Otherwise check if opcode is EFI_IFR_EQ_VAR_VAL_OP 0x22
			else if(buffer[j] == EFI_IFR_EQ_VAR_VAL_OP) {

				// Create temp
				EFI_IFR_EQ_VAR_VAL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.variableId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.value = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Variable 0x" << hex << uppercase << temp.variableId << " equals 0x" << temp.value;

				// Check if there isn't a following conditional statement
				if(endEFICondition(buffer, j))

					// Increment number of tabs
					numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_ORDERED_LIST_OP 0x23
			else if(buffer[j] == EFI_IFR_ORDERED_LIST_OP) {

				// Create temp
				EFI_IFR_ORDERED_LIST temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.maximumEntries = static_cast<unsigned char>(buffer[j + 4]);
				temp.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				temp.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 7]) + (static_cast<unsigned char>(buffer[j + 8]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Ordered List: " << strings[temp.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.questionId;
			}

			// Otherwise check if opcode is EFI_IFR_VARSTORE_OP 0x24
			else if(buffer[j] == EFI_IFR_VARSTORE_OP) {

				// Create temp
				EFI_IFR_VARSTORE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				for(uint32_t k = j + 2; k < j + 6; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.guid.insert(0, 1, lowNibble);
					temp.guid.insert(0, 1, highNibble);
				}
				temp.guid.push_back('-');
				for(uint32_t k = j + 6; k < j + 8; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.guid.insert(9, 1, lowNibble);
					temp.guid.insert(9, 1, highNibble);
				}
				temp.guid.push_back('-');
				for(uint32_t k = j + 8; k < j + 10; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.guid.insert(14, 1, lowNibble);
					temp.guid.insert(14, 1, highNibble);
				}
				temp.guid.push_back('-');
				for(uint32_t k = j + 10; k < j + 12; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.guid.push_back(highNibble);
					temp.guid.push_back(lowNibble);
				}
				temp.guid.push_back('-');
				for(uint32_t k = j + 12; k < j + 18; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.guid.push_back(highNibble);
					temp.guid.push_back(lowNibble);
				}
				temp.varId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 18]) + (static_cast<unsigned char>(buffer[j + 19]) << 8));
				temp.size = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 20]) + (static_cast<unsigned char>(buffer[j + 21]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Variable Store: 0x" << hex << uppercase << temp.varId;
			}

			// Otherwise check if opcode is EFI_IFR_VARSTORE_SELECT_OP 0x25
			else if(buffer[j] == EFI_IFR_VARSTORE_SELECT_OP) {

				// Create temp
				EFI_IFR_VARSTORE_SELECT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.varId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Variable Store Select: 0x" << hex << uppercase << temp.varId;
			}

			// Otherwise check if opcode is EFI_IFR_VARSTORE_SELECT_PAIR_OP 0x26
			else if(buffer[j] == EFI_IFR_VARSTORE_SELECT_PAIR_OP) {

				// Create temp
				EFI_IFR_VARSTORE_SELECT_PAIR temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);
				temp.primaryVarId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.secondaryVarId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Variable Store Select Pair: 0x" << hex << uppercase << temp.primaryVarId << "and 0x" << temp.secondaryVarId;
			}

			// Otherwise check if opcode is EFI_IFR_TRUE_OP 0x27
			else if(buffer[j] == EFI_IFR_TRUE_OP) {

				// Create temp
				EFI_IFR_TRUE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "True";

				// Check if there isn't a following conditional statement
				if(endEFICondition(buffer, j))

					// Increment number of tabs
					numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_FALSE_OP 0x28
			else if(buffer[j] == EFI_IFR_FALSE_OP) {

				// Create temp
				EFI_IFR_FALSE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "False";

				// Check if there isn't a following conditional statement
				if(endEFICondition(buffer, j))

					// Increment number of tabs
					numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_GT_OP 0x29
			else if(buffer[j] == EFI_IFR_GT_OP) {

				// Create temp
				EFI_IFR_GT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "GT";
				
				// Check if there isn't a following conditional statement
				if(endEFICondition(buffer, j))

					// Increment number of tabs
					numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_GE_OP 0x2A
			else if(buffer[j] == EFI_IFR_GE_OP) {

				// Create temp
				EFI_IFR_GE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "GE";
				
				// Check if there isn't a following conditional statement
				if(endEFICondition(buffer, j))

					// Increment number of tabs
					numberOfTabs++;
			}

			// Otherwise check if opcode is EFI_IFR_OEM_DEFINED_OP 0x2B
			else if(buffer[j] == EFI_IFR_OEM_DEFINED_OP) {

				// Display offset
				fout << "0x" << hex << uppercase << j << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "OEM Defined:";
			}

			// Otherwise check if opcode is EFI_IFR_OEM_OP 0xFE
			else if(buffer[j] == EFI_IFR_OEM_OP) {

				// Display offset
				fout << "0x" << hex << uppercase << j << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "OEM:";
			}

			// Otherwise check if opcode is EFI_IFR_NV_ACCESS_COMMAND 0xFF
			else if(buffer[j] == EFI_IFR_NV_ACCESS_COMMAND) {

				// Display offset
				fout << "0x" << hex << uppercase << j << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "NV Access Command:";
			}
		
			// Display code
			fout << " {";
			for(int k = 0; k < static_cast<unsigned char>(buffer[j + 1]); k++) {
				fout << hex << uppercase << setw(2) << setfill('0') << static_cast<unsigned int>(static_cast<unsigned char>(buffer[j + k]));
				if(k != static_cast<unsigned char>(buffer[j + 1]) - 1)
					fout << ' ';
			}
			fout << '}' << endl;
		}
	
	// Close file
	fout.close();
}

bool endEFICondition(const string &buffer, uint32_t j) {

	// Return if following opcode isn't a condition
	return buffer[j + static_cast<unsigned char>(buffer[j + 1])] != EFI_IFR_EQ_ID_VAL_OP && buffer[j + static_cast<unsigned char>(buffer[j + 1])] != EFI_IFR_EQ_ID_ID_OP && buffer[j + static_cast<unsigned char>(buffer[j + 1])] != EFI_IFR_EQ_ID_LIST_OP && buffer[j + static_cast<unsigned char>(buffer[j + 1])] != EFI_IFR_EQ_VAR_VAL_OP && buffer[j + static_cast<unsigned char>(buffer[j + 1])] != EFI_IFR_AND_OP && buffer[j + static_cast<unsigned char>(buffer[j + 1])] != EFI_IFR_OR_OP && buffer[j + static_cast<unsigned char>(buffer[j + 1])] != EFI_IFR_NOT_OP && buffer[j + static_cast<unsigned char>(buffer[j + 1])] != EFI_IFR_TRUE_OP && buffer[j + static_cast<unsigned char>(buffer[j + 1])] != EFI_IFR_FALSE_OP && buffer[j + static_cast<unsigned char>(buffer[j + 1])] != EFI_IFR_GT_OP && buffer[j + static_cast<unsigned char>(buffer[j + 1])] != EFI_IFR_GE_OP;
}
