// Header files
#include <iostream>
#include <fstream>
#include <iomanip>
#include "UEFI.h"


// Supporting function implementation
void getUEFIStringPackages(vector<UEFI_IFR_STRING_PACK> &stringPackages, const string &buffer) {

	// Initialize variables
	UEFI_IFR_STRING_PACK tempStringPackage;

	// Go through buffer
	for(unsigned int i = 0; i < buffer.size() - 45; i++)

		// Check if string pakage was found
		if((buffer[i] != '\x00' || buffer[i + 1] != '\x00' || buffer[i + 2] != '\x00') && buffer[i + 3] == '\x04' && buffer[i + 4] == '\x34' && buffer[i + 44] == '\x01' && buffer[i + 45] == '\x00' && buffer[i + 48] == '\x2D' && i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 1] == '\x00' && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 2] == '\x00') {

			// Set temp string package
			tempStringPackage.header.offset = i;

			tempStringPackage.header.length = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16));

			tempStringPackage.header.type = static_cast<unsigned char>(buffer[i + 3]);
			tempStringPackage.headerSize = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 4]) + (static_cast<unsigned char>(buffer[i + 5]) << 8) + (static_cast<unsigned char>(buffer[i + 6]) << 16) + (static_cast<unsigned char>(buffer[i + 7]) << 24));

			tempStringPackage.stringOffset = i + static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 8]) + (static_cast<unsigned char>(buffer[i + 9]) << 8) + (static_cast<unsigned char>(buffer[i + 10]) << 16) + (static_cast<unsigned char>(buffer[i + 11]) << 24));

			for(int j = 0; j < 16; j++)
				tempStringPackage.languageWindow.push_back(static_cast<uint16_t>(static_cast<unsigned char>(buffer[i + 12 + j * 2]) + (static_cast<unsigned char>(buffer[i + 13 + j * 2]) << 8)));

			for(int j = i + 46; buffer[j] != '\x00'; j++)
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
	for(unsigned int i = 0; i < stringPackages.size(); i++) {

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
	for(unsigned int i = 0; i < stringPackages.size(); i++) {

		// Check if language isn't english
		if(stringPackages[i].language != "en-US")

			// Continue
			continue;
		
		// Set structure offset
		stringPackages[i].structureOffset = strings.size();
		
		// Add language string to list
		strings.push_back(stringPackages[i].language);
		
		// Loop while string exists
		for(index = stringPackages[i].stringOffset; buffer[index] == '\x14'; index += 2) {

			// Fill string
			for(index++; buffer[index] != '\x00'; index += 2)
				temp.push_back(buffer[index]);

			// Add string to list
			strings.push_back(temp);

			// Clear temp
			temp.clear();
		
			// Skip % padding
			while(buffer[index + 1] == '\x25')
				index += 2;
		}
	}

	// Go through strings
	for(unsigned int i = 0; i < strings.size(); i++) {

		// Change carriage return characters to spaces
		for(unsigned int j = 0; j < strings[i].size(); j++)
			if(strings[i][j] == '\r')
				strings[i][j] = ' ';

		// Remove leading spaces
		while (strings[i].size() > 1 && strings[i][0] == ' ')
			strings[i].erase(0, 1);

		// Change double spaces
		for(unsigned int j = 0; strings[i].size() && j < strings[i].size() - 1; j++)
			if(strings[i][j] == ' ' && strings[i][j + 1] == ' ')
				strings[i].erase(j, 1);
	}
}

void displayUEFIStrings(const vector<string> &strings) {

	// Print strings
	cout << "Strings:" << endl;

	// Go through strings
	for(uint32_t i = 0; i < strings.size(); i++)

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
	for(unsigned int i = 0; i < stringPackages.size(); i++) {

		// Check if language isn't english
		if(stringPackages[i].language != "en-US")

			// Continue
			continue;
		
		// Get string candidate
		stringCandidates.push_back(i);
	}

	// Go through buffer
	for(uint32_t i = 0; i < buffer.size() - 4; i++)

		// Check if form set was found
		if((buffer[i] != '\x00' || buffer[i + 1] != '\x00' || buffer[i + 2] != '\x00') && buffer[i + 3] == '\x02' && buffer[i + 4] == '\x0E' && i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 1] == '\x02' && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 2] == '\x29') {
		
			// Get chosen candidate for form set's string package
			chosenCandidate = stringCandidates[0];
			for(unsigned int j = 1; j < stringCandidates.size(); j++)
				if(abs(static_cast<signed>(stringPackages[j].header.offset - i)) < abs(static_cast<signed>(stringPackages[chosenCandidate].header.offset - i)))
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

void generateUEFIIFRDump(const string &outputFile, const vector<UEFI_IFR_STRING_PACK> &stringPackages, const vector<UEFI_IFR_FORM_SET_PACK> &formSets, const string &buffer, const vector<string> &strings) {

	// Initialize variables
	uint8_t numberOfTabs = 0;
	unsigned char highNibble, lowNibble;
	vector<uint8_t> conditionalStack;
	ofstream fout(outputFile.c_str());

	// Display protocol
	for(uint8_t i = 0; i < 32; i++)
		fout << ' ';
	fout << "UEFI Protocol Detected" << endl;
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

	// Display IFR
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
		for(uint32_t j = formSets[i].header.offset + 4; j < formSets[i].header.offset + formSets[i].header.length; j += static_cast<unsigned char>(buffer[j + 1]) & 0x7F) {

			// Check if opcode if UEFI_IFR_FORM_OP 0x01
			if(buffer[j] == UEFI_IFR_FORM_OP) {

				// Create temp
				UEFI_IFR_FORM temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.formId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.titleString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Form: " << strings[temp.titleString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Form ID: 0x" << hex << uppercase << temp.formId;
				
				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(FORM);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_SUBTITLE_OP 0x02
			else if(buffer[j] == UEFI_IFR_SUBTITLE_OP) {

				// Create temp
				UEFI_IFR_SUBTITLE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.statement.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.statement.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.flags = static_cast<unsigned char>(buffer[j + 6]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Subtitle: " << strings[temp.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset];

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_TEXT_OP 0x03
			else if(buffer[j] == UEFI_IFR_TEXT_OP) {

				// Create temp
				UEFI_IFR_TEXT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.statement.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.statement.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.secondaryTextString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Text: " << strings[temp.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset];

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_IMAGE_OP 0x04
			else if(buffer[j] == UEFI_IFR_IMAGE_OP) {

				// Create temp
				UEFI_IFR_IMAGE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.imageId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Image";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_ONE_OF_OP 0x05
			else if(buffer[j] == UEFI_IFR_ONE_OF_OP) {

				// Create temp
				UEFI_IFR_ONE_OF temp;

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

				if(temp.flags == 0x00) {
					temp.data.minimumValue = static_cast<unsigned char>(buffer[j + 14]);
					temp.data.maximumValue = static_cast<unsigned char>(buffer[j + 15]);
					temp.data.step = static_cast<unsigned char>(buffer[j + 16]);
				}
				else if(temp.flags == 0x01) {
					temp.data.minimumValue = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 14]) + (static_cast<unsigned char>(buffer[j + 15]) << 8));
					temp.data.maximumValue = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 16]) + (static_cast<unsigned char>(buffer[j + 17]) << 8));
					temp.data.step = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 18]) + (static_cast<unsigned char>(buffer[j + 19]) << 8));
				}
				else if(temp.flags == 0x02) {
					temp.data.minimumValue = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 14]) + (static_cast<unsigned char>(buffer[i + 15]) << 8) + (static_cast<unsigned char>(buffer[i + 16]) << 16) + (static_cast<unsigned char>(buffer[i + 17]) << 24));
					temp.data.maximumValue = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 18]) + (static_cast<unsigned char>(buffer[i + 19]) << 8) + (static_cast<unsigned char>(buffer[i + 20]) << 16) + (static_cast<unsigned char>(buffer[i + 21]) << 24));
					temp.data.step = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 22]) + (static_cast<unsigned char>(buffer[i + 23]) << 8) + (static_cast<unsigned char>(buffer[i + 24]) << 16) + (static_cast<unsigned char>(buffer[i + 25]) << 24));
				}
				else if(temp.flags == 0x03) {
					temp.data.minimumValue = static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 14]) + (static_cast<unsigned char>(buffer[j + 15]) << 8) + (static_cast<unsigned char>(buffer[j + 16]) << 16) + (static_cast<unsigned char>(buffer[j + 17]) << 24) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 18])) << 32) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 19])) << 40) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 20])) << 48) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 21])) << 56));
					temp.data.maximumValue = static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 22]) + (static_cast<unsigned char>(buffer[j + 23]) << 8) + (static_cast<unsigned char>(buffer[j + 24]) << 16) + (static_cast<unsigned char>(buffer[j + 25]) << 24) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 26])) << 32) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 27])) << 40) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 28])) << 48) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 29])) << 56));
					temp.data.step = static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 30]) + (static_cast<unsigned char>(buffer[j + 31]) << 8) + (static_cast<unsigned char>(buffer[j + 32]) << 16) + (static_cast<unsigned char>(buffer[j + 33]) << 24) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 34])) << 32) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 35])) << 40) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 36])) << 48) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 37])) << 56));
				}

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Setting: " << strings[temp.question.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OPTION);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_CHECKBOX_OP 0x06
			else if(buffer[j] == UEFI_IFR_CHECKBOX_OP) {

				// Create temp
				UEFI_IFR_CHECKBOX temp;

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

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Checkbox: " << strings[temp.question.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_NUMERIC_OP 0x07
			else if(buffer[j] == UEFI_IFR_NUMERIC_OP) {

				// Create temp
				UEFI_IFR_NUMERIC temp;

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

				if(temp.flags == 0x00) {
					temp.data.minimumValue = static_cast<unsigned char>(buffer[j + 14]);
					temp.data.maximumValue = static_cast<unsigned char>(buffer[j + 15]);
					temp.data.step = static_cast<unsigned char>(buffer[j + 16]);
				}
				else if(temp.flags == 0x01) {
					temp.data.minimumValue = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 14]) + (static_cast<unsigned char>(buffer[j + 15]) << 8));
					temp.data.maximumValue = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 16]) + (static_cast<unsigned char>(buffer[j + 17]) << 8));
					temp.data.step = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 18]) + (static_cast<unsigned char>(buffer[j + 19]) << 8));
				}
				else if(temp.flags == 0x02) {
					temp.data.minimumValue = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 14]) + (static_cast<unsigned char>(buffer[i + 15]) << 8) + (static_cast<unsigned char>(buffer[i + 16]) << 16) + (static_cast<unsigned char>(buffer[i + 17]) << 24));
					temp.data.maximumValue = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 18]) + (static_cast<unsigned char>(buffer[i + 19]) << 8) + (static_cast<unsigned char>(buffer[i + 20]) << 16) + (static_cast<unsigned char>(buffer[i + 21]) << 24));
					temp.data.step = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 22]) + (static_cast<unsigned char>(buffer[i + 23]) << 8) + (static_cast<unsigned char>(buffer[i + 24]) << 16) + (static_cast<unsigned char>(buffer[i + 25]) << 24));
				}
				else if(temp.flags == 0x03) {
					temp.data.minimumValue = static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 14]) + (static_cast<unsigned char>(buffer[j + 15]) << 8) + (static_cast<unsigned char>(buffer[j + 16]) << 16) + (static_cast<unsigned char>(buffer[j + 17]) << 24) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 18])) << 32) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 19])) << 40) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 20])) << 48) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 21])) << 56));
					temp.data.maximumValue = static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 22]) + (static_cast<unsigned char>(buffer[j + 23]) << 8) + (static_cast<unsigned char>(buffer[j + 24]) << 16) + (static_cast<unsigned char>(buffer[j + 25]) << 24) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 26])) << 32) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 27])) << 40) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 28])) << 48) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 29])) << 56));
					temp.data.step = static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 30]) + (static_cast<unsigned char>(buffer[j + 31]) << 8) + (static_cast<unsigned char>(buffer[j + 32]) << 16) + (static_cast<unsigned char>(buffer[j + 33]) << 24) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 34])) << 32) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 35])) << 40) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 36])) << 48) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 37])) << 56));
				}

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Numeric: " << strings[temp.question.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << dec << " (" << temp.data.minimumValue << '-' << temp.data.maximumValue << ") , Variable: 0x" << hex << uppercase << temp.question.varOffset;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_PASSWORD_OP 0x08
			else if(buffer[j] == UEFI_IFR_PASSWORD_OP) {

				// Create temp
				UEFI_IFR_PASSWORD temp;

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
				temp.minimumSize = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 13]) + (static_cast<unsigned char>(buffer[j + 14]) << 8));
				temp.maximumSize = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 15]) + (static_cast<unsigned char>(buffer[j + 16]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Password: " << strings[temp.question.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_ONE_OF_OPTION_OP 0x09
			else if(buffer[j] == UEFI_IFR_ONE_OF_OPTION_OP) {

				// Create temp
				UEFI_IFR_ONE_OF_OPTION temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.optionString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.flags = static_cast<unsigned char>(buffer[j + 4]);
				temp.type = static_cast<unsigned char>(buffer[j + 5]);

				if(temp.type == 0x00)
					temp.value = static_cast<unsigned char>(buffer[j + 6]);
				else if(temp.type == 0x01)
					temp.value = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8));
				else if(temp.type == 0x02)
					temp.value = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24));
				else if(temp.type == 0x03)
					temp.value = static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8) + (static_cast<unsigned char>(buffer[j + 8]) << 16) + (static_cast<unsigned char>(buffer[j + 9]) << 24) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 10])) << 32) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 11])) << 40) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 12])) << 48) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 13])) << 56));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Option: " << strings[temp.optionString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Value: ";

				if(temp.type >= 0x00 && temp.type <= 0x03)
					fout << "0x" << hex << uppercase << temp.value;
				else if(temp.type == 0x04)
					fout << "Bool";
				else if(temp.type == 0x05)
					fout << "Time";
				else if(temp.type == 0x06)
					fout << "Date";
				else if(temp.type == 0x07)
					fout << "String";
				else if(temp.type == 0x08)
					fout << "Other";
				else if(temp.type == 0x09)
					fout << "Undefined";
				else if(temp.type == 0x0A)
					fout << "Action";
				else if(temp.type == 0x0B)
					fout << "Buffer";

				if(temp.flags == 0x10)
					fout << " (default)";
				else if(temp.flags == 0x20)
					fout << " (default MFG)";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_SUPPRESS_IF_OP 0x0A
			else if(buffer[j] == UEFI_IFR_SUPPRESS_IF_OP) {

				// Create temp
				UEFI_IFR_SUPPRESS_IF temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Suppress If:";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(CONDITION);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_LOCKED_OP 0x0B
			else if(buffer[j] == UEFI_IFR_LOCKED_OP) {

				// Create temp
				UEFI_IFR_LOCKED temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Locked";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_ACTION_OP 0x0C
			else if(buffer[j] == UEFI_IFR_ACTION_OP) {

				// Create temp
				UEFI_IFR_ACTION temp;

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
				temp.questionConfigString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 13]) + (static_cast<unsigned char>(buffer[j + 14]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Action: " << strings[temp.question.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_RESET_BUTTON_OP 0x0D
			else if(buffer[j] == UEFI_IFR_RESET_BUTTON_OP) {

				// Create temp
				UEFI_IFR_RESET_BUTTON temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.statement.promptString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.statement.helpString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));
				temp.defaultId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 6]) + (static_cast<unsigned char>(buffer[j + 7]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Reset: " << strings[temp.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset];

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_FORM_SET_OP 0x0E
			else if(buffer[j] == UEFI_IFR_FORM_SET_OP) {

				// Create temp
				UEFI_IFR_FORM_SET temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
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
					highNibble = ((buffer[i] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[i] & 0x0F) + '0';
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
				temp.flags = static_cast<unsigned char>(buffer[j + 22]);
				for(uint32_t k = j + 23; k < j + 27; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.classGuid.insert(0, 1, lowNibble);
					temp.classGuid.insert(0, 1, highNibble);
				}
				temp.classGuid.push_back('-');
				for(uint32_t k = j + 27; k < j + 29; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.classGuid.insert(9, 1, lowNibble);
					temp.classGuid.insert(9, 1, highNibble);
				}
				temp.classGuid.push_back('-');
				for(uint32_t k = j + 29; k < j + 31; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.classGuid.insert(14, 1, lowNibble);
					temp.classGuid.insert(14, 1, highNibble);
				}
				temp.classGuid.push_back('-');
				for(uint32_t k = j + 31; k < j + 33; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.classGuid.push_back(highNibble);
					temp.classGuid.push_back(lowNibble);
				}
				temp.classGuid.push_back('-');
				for(uint32_t k = j + 33; k < j + 39; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.classGuid.push_back(highNibble);
					temp.classGuid.push_back(lowNibble);
				}

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Form Set: " << strings[temp.titleString + stringPackages[formSets[i].usingStringPackage].structureOffset];

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(FORM_SET);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_REF_OP 0x0F
			else if(buffer[j] == UEFI_IFR_REF_OP) {

				// Create temp
				UEFI_IFR_REF temp;

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
				temp.formId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 13]) + (static_cast<unsigned char>(buffer[j + 14]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Ref: " << strings[temp.question.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_NO_SUBMIT_IF_OP 0x10
			else if(buffer[j] == UEFI_IFR_NO_SUBMIT_IF_OP) {

				// Create temp
				UEFI_IFR_NO_SUBMIT_IF temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "No Submit If:";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(CONDITION);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_INCONSISTENT_IF_OP 0x11
			else if(buffer[j] == UEFI_IFR_INCONSISTENT_IF_OP) {

				// Create temp
				UEFI_IFR_INCONSISTENT_IF temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Inconsistent If:";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(CONDITION);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_EQ_ID_VAL_OP 0x12
			else if(buffer[j] == UEFI_IFR_EQ_ID_VAL_OP) {

				// Create temp
				UEFI_IFR_EQ_ID_VAL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.value = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Variable 0x" << hex << uppercase << temp.questionId << " equals 0x" << hex << uppercase << temp.value;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_EQ_ID_ID_OP 0x13
			else if(buffer[j] == UEFI_IFR_EQ_ID_ID_OP) {

				// Create temp
				UEFI_IFR_EQ_ID_ID temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.primaryQuestionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.secondaryQuestionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Variable 0x" << hex << uppercase << temp.primaryQuestionId << " equals variable 0x" << hex << uppercase << temp.secondaryQuestionId;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_EQ_ID_LIST_OP 0x14
			else if(buffer[j] == UEFI_IFR_EQ_ID_LIST_OP) {

				// Create temp
				UEFI_IFR_EQ_ID_LIST temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.listLength = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));

				// Create list
				uint32_t index = j + 6;

				for(uint8_t k = 0; k < temp.listLength; k++, index += 2)
					temp.valueList.push_back(static_cast<uint16_t>(static_cast<unsigned char>(buffer[index]) + (static_cast<unsigned char>(buffer[index + 1]) << 8)));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Variable 0x" << hex << uppercase << temp.questionId << " equals value in list (";
				for(uint8_t k = 0; k < temp.listLength; k++) {
					fout << "0x" << hex << uppercase << temp.valueList[k];
					if(k != temp.listLength - 1)
						fout << ", ";
					else
						fout << ')';
				}

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_AND_OP 0x15
			else if(buffer[j] == UEFI_IFR_AND_OP) {

				// Create temp
				UEFI_IFR_AND temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "And";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_OR_OP 0x16
			else if(buffer[j] == UEFI_IFR_OR_OP) {

				// Create temp
				UEFI_IFR_OR temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Or";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_NOT_OP 0x17
			else if(buffer[j] == UEFI_IFR_NOT_OP) {

				// Create temp
				UEFI_IFR_NOT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Not";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_RULE_OP 0x18
			else if(buffer[j] == UEFI_IFR_RULE_OP) {

				// Create temp
				UEFI_IFR_RULE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.ruleId = static_cast<unsigned char>(buffer[j + 2]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Rule";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_GRAY_OUT_IF_OP 0x19
			else if(buffer[j] == UEFI_IFR_GRAY_OUT_IF_OP) {

				// Create temp
				UEFI_IFR_GRAY_OUT_IF temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Grayout If:";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(CONDITION);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_DATE_OP 0x1A
			else if(buffer[j] == UEFI_IFR_DATE_OP) {

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

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Date: " << strings[temp.question.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_TIME_OP 0x1B
			else if(buffer[j] == UEFI_IFR_TIME_OP) {

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

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Time: " << strings[temp.question.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_STRING_OP 0x1C
			else if(buffer[j] == UEFI_IFR_STRING_OP) {

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

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "String: " << strings[temp.question.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_REFRESH_OP 0x1D
			else if(buffer[j] == UEFI_IFR_REFRESH_OP) {

				// Create temp
				UEFI_IFR_REFRESH temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.refreshInterval = static_cast<unsigned char>(buffer[j + 2]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Refresh";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_DISABLE_IF_OP 0x1E
			else if(buffer[j] == UEFI_IFR_DISABLE_IF_OP) {

				// Create temp
				UEFI_IFR_DISABLE_IF temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Disable If:";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(CONDITION);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_ANIMATION_OP 0x1F
			else if(buffer[j] == UEFI_IFR_ANIMATION_OP) {

				// Create temp
				UEFI_IFR_ANIMATION temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.animationId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Animation";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_TO_LOWER_OP 0x20
			else if(buffer[j] == UEFI_IFR_TO_LOWER_OP) {

				// Create temp
				UEFI_IFR_TO_LOWER temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Convert to Lower";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_TO_UPPER_OP 0x21
			else if(buffer[j] == UEFI_IFR_TO_UPPER_OP) {

				// Create temp
				UEFI_IFR_TO_UPPER temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Convert to Upper";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_MAP_OP 0x22
			else if(buffer[j] == UEFI_IFR_MAP_OP) {

				// Create temp
				UEFI_IFR_MAP temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Map";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_ORDERED_LIST_OP 0x23
			else if(buffer[j] == UEFI_IFR_ORDERED_LIST_OP) {

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

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Ordered List: " << strings[temp.question.statement.promptString + stringPackages[formSets[i].usingStringPackage].structureOffset] << ", Variable: 0x" << hex << uppercase << temp.question.varOffset;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_VARSTORE_OP 0x24
			else if(buffer[j] == UEFI_IFR_VARSTORE_OP) {

				// Create temp
				UEFI_IFR_VARSTORE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
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
				temp.varStoreId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 18]) + (static_cast<unsigned char>(buffer[j + 19]) << 8));
				temp.size = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 20]) + (static_cast<unsigned char>(buffer[j + 21]) << 8));
				for(uint32_t index = j + 22; buffer[index] != '\x00'; index++)
					temp.name.push_back(buffer[index]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Var Store: 0x" << hex << uppercase << temp.varStoreId << "[" << dec << temp.size << "] (" << temp.name << ')';

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_VARSTORE_NAME_VALUE_OP 0x25
			else if(buffer[j] == UEFI_IFR_VARSTORE_NAME_VALUE_OP) {

				// Create temp
				UEFI_IFR_VARSTORE_NAME_VALUE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.varStoreId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				for(uint32_t k = j + 4; k < j + 8; k++) {
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
				for(uint32_t k = j + 8; k < j + 10; k++) {
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
				for(uint32_t k = j + 10; k < j + 12; k++) {
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
				for(uint32_t k = j + 12; k < j + 14; k++) {
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
				for(uint32_t k = j + 14; k < j + 20; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.guid.push_back(highNibble);
					temp.guid.push_back(lowNibble);
				}

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Var Store Name Value: 0x" << hex << uppercase << temp.varStoreId;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_VARSTORE_EFI_OP 0x26
			else if(buffer[j] == UEFI_IFR_VARSTORE_EFI_OP) {

				// Create temp
				UEFI_IFR_VARSTORE_EFI temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.varStoreId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				for(uint32_t k = j + 4; k < j + 8; k++) {
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
				for(uint32_t k = j + 8; k < j + 10; k++) {
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
				for(uint32_t k = j + 10; k < j + 12; k++) {
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
				for(uint32_t k = j + 12; k < j + 14; k++) {
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
				for(uint32_t k = j + 14; k < j + 20; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.guid.push_back(highNibble);
					temp.guid.push_back(lowNibble);
				}
				temp.attributes = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 20]) + (static_cast<unsigned char>(buffer[i + 21]) << 8) + (static_cast<unsigned char>(buffer[i + 22]) << 16) + (static_cast<unsigned char>(buffer[i + 23]) << 24));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Var Store EFI: 0x" << hex << uppercase << temp.varStoreId;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_VARSTORE_DEVICE_OP 0x27
			else if(buffer[j] == UEFI_IFR_VARSTORE_DEVICE_OP) {

				// Create temp
				UEFI_IFR_VARSTORE_DEVICE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.devicePathString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Var Store Device";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_VERSION_OP 0x28
			else if(buffer[j] == UEFI_IFR_VERSION_OP) {

				// Create temp
				UEFI_IFR_VERSION temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Version";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_END_OP 0x29
			else if(buffer[j] == UEFI_IFR_END_OP) {

				// Create temp
				UEFI_IFR_END temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.condition = conditionalStack.back();

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Decrement number of tabs
				numberOfTabs--;

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Decrement conditional stack
				conditionalStack.pop_back();

				// Display temp
				fout << "End";
				if(temp.condition == FORM_SET)
					fout << " Form Set";
				else if(temp.condition == FORM)
					fout << " Form";
				else if(temp.condition == CONDITION)
					fout << " If";
				else if(temp.condition == OPTION)
					fout << " of Options";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_MATCH_OP 0x2A
			else if(buffer[j] == UEFI_IFR_MATCH_OP) {

				// Create temp
				UEFI_IFR_MATCH temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Match";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_GET_OP 0x2B
			else if(buffer[j] == UEFI_IFR_GET_OP) {

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

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Get: 0x" << hex << uppercase << temp.varOffset;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_SET_OP 0x2C
			else if(buffer[j] == UEFI_IFR_SET_OP) {

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


				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Set: 0x" << hex << uppercase << temp.varOffset;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_READ_OP 0x2D
			else if(buffer[j] == UEFI_IFR_READ_OP) {

				// Create temp
				UEFI_IFR_READ temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Read";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_WRITE_OP 0x2E
			else if(buffer[j] == UEFI_IFR_WRITE_OP) {

				// Create temp
				UEFI_IFR_WRITE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Write";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_EQUAL_OP 0x2F
			else if(buffer[j] == UEFI_IFR_EQUAL_OP) {

				// Create temp
				UEFI_IFR_EQUAL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Equal";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_NOT_EQUAL_OP 0x30
			else if(buffer[j] == UEFI_IFR_NOT_EQUAL_OP) {

				// Create temp
				UEFI_IFR_NOT_EQUAL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Not Equal";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_GREATER_THAN_OP 0x31
			else if(buffer[j] == UEFI_IFR_GREATER_THAN_OP) {

				// Create temp
				UEFI_IFR_GREATER_THAN temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Greater Than";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_GREATER_EQUAL_OP 0x32
			else if(buffer[j] == UEFI_IFR_GREATER_EQUAL_OP) {

				// Create temp
				UEFI_IFR_GREATER_EQUAL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Greater Than or Equal";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_LESS_THAN_OP 0x33
			else if(buffer[j] == UEFI_IFR_LESS_THAN_OP) {

				// Create temp
				UEFI_IFR_LESS_THAN temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Less Than";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_LESS_EQUAL_OP 0x34
			else if(buffer[j] == UEFI_IFR_LESS_EQUAL_OP) {

				// Create temp
				UEFI_IFR_LESS_EQUAL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Less Than or Equal";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_BITWISE_AND_OP 0x35
			else if(buffer[j] == UEFI_IFR_BITWISE_AND_OP) {

				// Create temp
				UEFI_IFR_BITWISE_AND temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Bitwise And";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_BITWISE_OR_OP 0x36
			else if(buffer[j] == UEFI_IFR_BITWISE_OR_OP) {

				// Create temp
				UEFI_IFR_BITWISE_OR temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Bitwise Or";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_BITWISE_NOT_OP 0x37
			else if(buffer[j] == UEFI_IFR_BITWISE_NOT_OP) {

				// Create temp
				UEFI_IFR_BITWISE_NOT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Bitwise Not";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_SHIFT_LEFT_OP 0x38
			else if(buffer[j] == UEFI_IFR_SHIFT_LEFT_OP) {

				// Create temp
				UEFI_IFR_SHIFT_LEFT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Shift Left";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_SHIFT_RIGHT_OP 0x39
			else if(buffer[j] == UEFI_IFR_SHIFT_RIGHT_OP) {

				// Create temp
				UEFI_IFR_SHIFT_RIGHT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Shift Right";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_ADD_OP 0x3A
			else if(buffer[j] == UEFI_IFR_ADD_OP) {

				// Create temp
				UEFI_IFR_ADD temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Add";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_SUBTRACT_OP 0x3B
			else if(buffer[j] == UEFI_IFR_SUBTRACT_OP) {

				// Create temp
				UEFI_IFR_SUBTRACT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Subtract";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_MULTIPLY_OP 0x3C
			else if(buffer[j] == UEFI_IFR_MULTIPLY_OP) {

				// Create temp
				UEFI_IFR_MULTIPLY temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Multiply";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_DIVIDE_OP 0x3D
			else if(buffer[j] == UEFI_IFR_DIVIDE_OP) {

				// Create temp
				UEFI_IFR_DIVIDE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Divide";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_MODULO_OP 0x3E
			else if(buffer[j] == UEFI_IFR_MODULO_OP) {

				// Create temp
				UEFI_IFR_MODULO temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Modulo";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_RULE_REF_OP 0x3F
			else if(buffer[j] == UEFI_IFR_RULE_REF_OP) {

				// Create temp
				UEFI_IFR_RULE_REF temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.ruleId = static_cast<unsigned char>(buffer[j + 2]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Rule Ref";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_QUESTION_REF1_OP 0x40
			else if(buffer[j] == UEFI_IFR_QUESTION_REF1_OP) {

				// Create temp
				UEFI_IFR_QUESTION_REF1 temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.questionId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[i + 2]) + (static_cast<unsigned char>(buffer[i + 3]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Question Ref: 0x" << hex << uppercase << temp.questionId;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_QUESTION_REF2_OP 0x41
			else if(buffer[j] == UEFI_IFR_QUESTION_REF2_OP) {

				// Create temp
				UEFI_IFR_QUESTION_REF2 temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Question Ref";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_UINT8_OP 0x42
			else if(buffer[j] == UEFI_IFR_UINT8_OP) {

				// Create temp
				UEFI_IFR_UINT8 temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.value = static_cast<unsigned char>(buffer[j + 2]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "8 Bit Unsigned Int: 0x" << hex << uppercase << temp.value;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_UINT16_OP 0x43
			else if(buffer[j] == UEFI_IFR_UINT16_OP) {

				// Create temp
				UEFI_IFR_UINT16 temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.value = static_cast<uint16_t>(static_cast<unsigned char>(buffer[i + 2]) + (static_cast<unsigned char>(buffer[i + 3]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "16 Bit Unsigned Int: 0x" << hex << uppercase << temp.value;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_UINT32_OP 0x44
			else if(buffer[j] == UEFI_IFR_UINT32_OP) {

				// Create temp
				UEFI_IFR_UINT32 temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.value = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 2]) + (static_cast<unsigned char>(buffer[i + 3]) << 8) + (static_cast<unsigned char>(buffer[i + 4]) << 16) + (static_cast<unsigned char>(buffer[i + 5]) << 24));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "32 Bit Unsigned Int: 0x" << hex << uppercase << temp.value;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_UINT64_OP 0x45
			else if(buffer[j] == UEFI_IFR_UINT64_OP) {

				// Create temp
				UEFI_IFR_UINT64 temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.value = static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8) + (static_cast<unsigned char>(buffer[j + 4]) << 16) + (static_cast<unsigned char>(buffer[j + 5]) << 24) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 6])) << 32) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 7])) << 40) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 8])) << 48) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 9])) << 56));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "64 Bit Unsigned Int: 0x" << hex << uppercase << temp.value;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_TRUE_OP 0x46
			else if(buffer[j] == UEFI_IFR_TRUE_OP) {

				// Create temp
				UEFI_IFR_TRUE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "True";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_FALSE_OP 0x47
			else if(buffer[j] == UEFI_IFR_FALSE_OP) {

				// Create temp
				UEFI_IFR_FALSE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "False";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_TO_UINT_OP 0x48
			else if(buffer[j] == UEFI_IFR_TO_UINT_OP) {

				// Create temp
				UEFI_IFR_TO_UINT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Convert to Unsigned Int";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_TO_STRING_OP 0x49
			else if(buffer[j] == UEFI_IFR_TO_STRING_OP) {

				// Create temp
				UEFI_IFR_TO_STRING temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.format = static_cast<unsigned char>(buffer[j + 2]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Convert to String";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_TO_BOOLEAN_OP 0x4A
			else if(buffer[j] == UEFI_IFR_TO_BOOLEAN_OP) {

				// Create temp
				UEFI_IFR_TO_BOOLEAN temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Boolean";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_MID_OP 0x4B
			else if(buffer[j] == UEFI_IFR_MID_OP) {

				// Create temp
				UEFI_IFR_MID temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Mid";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_FIND_OP 0x4C
			else if(buffer[j] == UEFI_IFR_FIND_OP) {

				// Create temp
				UEFI_IFR_FIND temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.format = static_cast<unsigned char>(buffer[j + 2]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Find";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_TOKEN_OP 0x4D
			else if(buffer[j] == UEFI_IFR_TOKEN_OP) {

				// Create temp
				UEFI_IFR_TOKEN temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Token";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_STRING_REF1_OP 0x4E
			else if(buffer[j] == UEFI_IFR_STRING_REF1_OP) {

				// Create temp
				UEFI_IFR_STRING_REF1 temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.stringId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "String Ref: " << strings[temp.stringId + stringPackages[formSets[i].usingStringPackage].structureOffset];

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_STRING_REF2_OP 0x4F
			else if(buffer[j] == UEFI_IFR_STRING_REF2_OP) {

				// Create temp
				UEFI_IFR_STRING_REF2 temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "String Ref";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_CONDITIONAL_OP 0x50
			else if(buffer[j] == UEFI_IFR_CONDITIONAL_OP) {

				// Create temp
				UEFI_IFR_CONDITIONAL temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Conditional";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_QUESTION_REF3_OP 0x51
			else if(buffer[j] == UEFI_IFR_QUESTION_REF3_OP) {

				// Create temp
				UEFI_IFR_QUESTION_REF3 temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Question Ref";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_ZERO_OP 0x52
			else if(buffer[j] == UEFI_IFR_ZERO_OP) {

				// Create temp
				UEFI_IFR_ZERO temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Zero";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_ONE_OP 0x53
			else if(buffer[j] == UEFI_IFR_ONE_OP) {

				// Create temp
				UEFI_IFR_ONE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "One";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_ONES_OP 0x54
			else if(buffer[j] == UEFI_IFR_ONES_OP) {

				// Create temp
				UEFI_IFR_ONES temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Ones";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_UNDEFINED_OP 0x55
			else if(buffer[j] == UEFI_IFR_UNDEFINED_OP) {

				// Create temp
				UEFI_IFR_UNDEFINED temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Undefined";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_LENGTH_OP 0x56
			else if(buffer[j] == UEFI_IFR_LENGTH_OP) {

				// Create temp
				UEFI_IFR_LENGTH temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Length";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_DUP_OP 0x57
			else if(buffer[j] == UEFI_IFR_DUP_OP) {

				// Create temp
				UEFI_IFR_DUP temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Dup";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_THIS_OP 0x58
			else if(buffer[j] == UEFI_IFR_THIS_OP) {

				// Create temp
				UEFI_IFR_THIS temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "This";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_SPAN_OP 0x59
			else if(buffer[j] == UEFI_IFR_SPAN_OP) {

				// Create temp
				UEFI_IFR_SPAN temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.flags = static_cast<unsigned char>(buffer[j + 2]);

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Span";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_VALUE_OP 0x5A
			else if(buffer[j] == UEFI_IFR_VALUE_OP) {

				// Create temp
				UEFI_IFR_VALUE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Value";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_DEFAULT_OP 0x5B
			else if(buffer[j] == UEFI_IFR_DEFAULT_OP) {

				// Create temp
				UEFI_IFR_DEFAULT temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.defaultId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.type = static_cast<unsigned char>(buffer[j + 4]);

				if(temp.type == 0x00)
					temp.value = static_cast<unsigned char>(buffer[j + 5]);
				else if(temp.type == 0x01)
					temp.value = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8));
				else if(temp.type == 0x02)
					temp.value = static_cast<uint32_t>(static_cast<unsigned char>(buffer[i + 5]) + (static_cast<unsigned char>(buffer[i + 6]) << 8) + (static_cast<unsigned char>(buffer[i + 7]) << 16) + (static_cast<unsigned char>(buffer[i + 8]) << 24));
				else if(temp.type == 0x03)
					temp.value = static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 5]) + (static_cast<unsigned char>(buffer[j + 6]) << 8) + (static_cast<unsigned char>(buffer[j + 7]) << 16) + (static_cast<unsigned char>(buffer[j + 8]) << 24) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 9])) << 32) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 10])) << 40) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 11])) << 48) + (static_cast<uint64_t>(static_cast<unsigned char>(buffer[j + 12])) << 56));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Default: ";
				if(temp.type == 0x00)
					fout << "8 Bit";
				else if(temp.type == 0x01)
					fout << "16 Bit";
				else if(temp.type == 0x02)
					fout << "32 Bit";
				else if(temp.type == 0x03)
					fout << "64 Bit";
				else if(temp.type == 0x04)
					fout << "Bool";
				else if(temp.type == 0x05)
					fout << "Time";
				else if(temp.type == 0x06)
					fout << "Date";
				else if(temp.type == 0x07)
					fout << "String";
				else if(temp.type == 0x08)
					fout << "Other";
				else if(temp.type == 0x09)
					fout << "Undefined";
				else if(temp.type == 0x0A)
					fout << "Action";
				else if(temp.type == 0x0B)
					fout << "Buffer";

				if(temp.type >= 0x00 && temp.type <= 0x03)
					fout << ", Value: 0x" << hex << uppercase << temp.value;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_DEFAULTSTORE_OP 0x5C
			else if(buffer[j] == UEFI_IFR_DEFAULTSTORE_OP) {

				// Create temp
				UEFI_IFR_DEFAULTSTORE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.defaultNameString = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));
				temp.defaultId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 4]) + (static_cast<unsigned char>(buffer[j + 5]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Default Store: " << strings[temp.defaultNameString + stringPackages[formSets[i].usingStringPackage].structureOffset] << " 0x" << hex << uppercase << temp.defaultId;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_FORM_MAP_OP 0x5D
			else if(buffer[j] == UEFI_IFR_FORM_MAP_OP) {

				// Create temp
				UEFI_IFR_FORM_MAP temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				temp.formId = static_cast<uint16_t>(static_cast<unsigned char>(buffer[j + 2]) + (static_cast<unsigned char>(buffer[j + 3]) << 8));

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Form Map: 0x" << hex << uppercase << temp.formId;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_CATENATE_OP 0x5E
			else if(buffer[j] == UEFI_IFR_CATENATE_OP) {

				// Create temp
				UEFI_IFR_CATENATE temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Catenate";

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_GUID_OP 0x5F
			else if(buffer[j] == UEFI_IFR_GUID_OP) {

				// Create temp
				UEFI_IFR_GUID temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
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

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Guid: " << temp.guid;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}

			// Otherwise check if opcode if UEFI_IFR_SECURITY_OP 0x60
			else if(buffer[j] == UEFI_IFR_SECURITY_OP) {

				// Create temp
				UEFI_IFR_SECURITY temp;

				// Fill temp
				temp.header.offset = j;
				temp.header.opcode = buffer[j];
				temp.header.length = static_cast<unsigned char>(buffer[j + 1]) & 0x7F;
				temp.header.scope = static_cast<unsigned char>(buffer[j + 1]) & 0x80;
				for(uint32_t k = j + 2; k < j + 6; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.permission.insert(0, 1, lowNibble);
					temp.permission.insert(0, 1, highNibble);
				}
				temp.permission.push_back('-');
				for(uint32_t k = j + 6; k < j + 8; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.permission.insert(9, 1, lowNibble);
					temp.permission.insert(9, 1, highNibble);
				}
				temp.permission.push_back('-');
				for(uint32_t k = j + 8; k < j + 10; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.permission.insert(14, 1, lowNibble);
					temp.permission.insert(14, 1, highNibble);
				}
				temp.permission.push_back('-');
				for(uint32_t k = j + 10; k < j + 12; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.permission.push_back(highNibble);
					temp.permission.push_back(lowNibble);
				}
				temp.permission.push_back('-');
				for(uint32_t k = j + 12; k < j + 18; k++) {
					highNibble = ((buffer[k] >> 4) & 0x0F) + '0';
					lowNibble = (buffer[k] & 0x0F) + '0';
					if(highNibble > 0x39 && highNibble < 0x41)
						highNibble += 0x07;
					if(lowNibble > 0x39 && lowNibble < 0x41)
						lowNibble += 0x07;
					temp.permission.push_back(highNibble);
					temp.permission.push_back(lowNibble);
				}

				// Display offset
				fout << "0x" << hex << uppercase << temp.header.offset << ' ';

				// Display tabs
				for(uint8_t k = 0; k < numberOfTabs; k++)
					fout << '\t';

				// Display temp
				fout << "Security: " << temp.permission;

				// Check if scope
				if(temp.header.scope) {

					// Increment number of tabs
					numberOfTabs++;

					// Add to conditional stack
					conditionalStack.push_back(OTHER);
				}
			}
		
			// Display code
			fout << " {";
			for(int k = 0; k < static_cast<unsigned char>(buffer[j + 1] & 0x7F); k++) {
				fout << hex << uppercase << setw(2) << setfill('0') << static_cast<unsigned int>(static_cast<unsigned char>(buffer[j + k]));
				if(k != static_cast<unsigned char>(buffer[j + 1] & 0x7F) - 1)
					fout << ' ';
			}
			fout << '}' << endl;
		}
	
	// Close file
	fout.close();
}
