// Header files
#include <string>
#include <vector>
#include <fstream>
#include "EFI.h"
#include "UEFI.h"
#include <iostream>
#include <map>

using namespace std;


// Error codes
enum {IFR_EXTRACTED, FILE_NOT_FOUND, UNKNOWN_PROTOCOL};

// Global definitions
enum type {EFI, UEFI, UNKNOWN};


// Global variables
string fileLocation, outputFile;
int errorCode;
string buffer;
type protocol;
vector<string> strings;

std::ostream& operator<<(std::ostream& out, const type value){
#define ENUM_ELEMENT(e) { e, #e }
    static map<type, string> strs =
    {
    	ENUM_ELEMENT(EFI),
	ENUM_ELEMENT(UEFI),
	ENUM_ELEMENT(UNKNOWN)
    };
#undef ENUM_ELEMENT

    return out << strs[value];
}


bool fileExists(const string &file) {

	// Open file
	ifstream fin(file);

	// Return if first characetr doesn't equal EOF
	return fin.peek() != EOF;
}

void readFile(const string &file, string &buffer) {

	// Initialize variables
	ifstream fin(file, ios::binary);

	// Clear buffer
	buffer.clear();

	// Read in file
	while(fin.peek() != EOF)
		buffer.push_back(fin.get());

	// Close file
	fin.close();
}

type getType(const string &buffer) {

	// Go through buffer
	for(unsigned int i = 0; i < buffer.size() - 9; i++)
	
		// Check if an EFI string pakage was found
		if((buffer[i] != '\x00' || buffer[i + 1] != '\x00' || buffer[i + 2] != '\x00' || buffer[i + 3] != '\x00') && buffer[i + 4] == '\x02' && buffer[i + 5] == '\x00' && (buffer[i + 6] != '\x00' || buffer[i + 7] != '\x00' || buffer[i + 8] != '\x00' || buffer[i + 9] != '\x00') && i + static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24) + 4] >= 'a' && buffer[i + static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24) + 4] <= 'z' && i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) + 4] == '\x02' && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) + 5] == '\x00')
		
			// Return EFI
			return EFI;
	
		// Otherwise check if a UEFI string pakage was found
		else if((buffer[i] != '\x00' || buffer[i + 1] != '\x00' || buffer[i + 2] != '\x00') && buffer[i + 3] == '\x04' && buffer[i + 4] == '\x34' && buffer[i + 44] == '\x01' && buffer[i + 45] == '\x00' && buffer[i + 48] == '\x2D' && i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 1] == '\x00' && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 2] == '\x00')
	
			// Return UEFI
			return UEFI;
	
	// Return unknown
	return UNKNOWN;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		cout << "Usage: ifrextract input_file output_file." << endl;
		return 1;
	}


	// Reset error code
	errorCode = IFR_EXTRACTED;
	fileLocation = argv[1];
	outputFile = argv[2];

	// Check if file exists
	if(fileExists(fileLocation)) {

		readFile(fileLocation, buffer);
		protocol = getType(buffer);

		cout << "Input: " << fileLocation << endl;
		cout << "Output: " << outputFile << endl;
		cout << "Protocol: " << protocol << endl;


		// Check if protocol is unknown
		if(protocol == UNKNOWN) {

			// Set error code
			errorCode = UNKNOWN_PROTOCOL;

			// Display message
			cout << "error: Unknown protocol" << endl;

			// Break
			return errorCode;
		}

		// Clear strings
		strings.clear();

		// Check if protocol is EFI
		if(protocol == EFI) {

			// Initialize EFI variables
			vector<EFI_IFR_STRING_PACK> stringPackages;
			vector<EFI_IFR_FORM_SET_PACK> formSets;

			// Get EFI string packages
			getEFIStringPackages(stringPackages, buffer);

			// Get EFI strings
			getEFIStrings(stringPackages, strings, buffer);

			// Get EFI form sets
			getEFIFormSets(formSets, buffer, stringPackages, strings);

			// Generate EFI IFR dump
			generateEFIIFRDump(outputFile, stringPackages, formSets, buffer, strings);
		}

		// Otherwise check if protocol is UEFI
		else if(protocol == UEFI) {

			// Initialize UEFI variables
			vector<UEFI_IFR_STRING_PACK> stringPackages;
			vector<UEFI_IFR_FORM_SET_PACK> formSets;

			// Get UEFI string packages
			getUEFIStringPackages(stringPackages, buffer);

			// Get UEFI strings
			getUEFIStrings(stringPackages, strings, buffer);

			// Get UEFI form sets
			getUEFIFormSets(formSets, buffer, stringPackages, strings);

			// Generate UEFI IFR dump
			generateUEFIIFRDump(outputFile, stringPackages, formSets, buffer, strings);
		}
	}

	// Otherwise
	else
	{
		// Set error code
		errorCode = FILE_NOT_FOUND;
		cout << "File not found!" << endl;
	}

	// Return
	return errorCode;
}

