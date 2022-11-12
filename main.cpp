// Header files
#include <windows.h>
#include <ShlObj.h>
#include <string>
#include <vector>
#include <fstream>
#include "EFI.h"
#include "UEFI.h"

using namespace std;


// Manifest
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


// Global constants

// Handel actions
enum { NO_ACTION, EXTRACT_BUTTON_ACTION, BROWSE_BUTTON_ACTION, FILE_LOCATION_EDIT_ACTION };

// Error codes
enum { IFR_EXTRACTED, FILE_NOT_FOUND, UNKNOWN_PROTOCOL };

// Global definitions
enum type { EFI, UEFI, UNKNOWN };


// Global variables
HWND fileLocationEdit, typeText;
string fileLocation, outputFile;
int errorCode;
string buffer;
type protocol;
vector<string> strings;


// Function prototypes

/*
Name: WndProc
Purpose: Windows procedure
*/
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/*
Name: fileBrowser
Purpose: Allows user to select file
*/
void fileBrowser(HWND hwnd);

/*
Name: saveFile
Purpose: Allows user to select output file
*/
bool saveFile(HWND hwnd);

/*
Name: showDialog
Purpose: Shows warnings and messages after actions
*/
void showDialog(HWND hwnd);

/*
Name: fileExists
Purpose: Checks if a file exists
*/
bool fileExists(const string &file);

/*
Name: readFile
Purpose: Reads specified file
*/
void readFile(const string &file, string &buffer);

/*
Name: getType
Purpose: Determines if file uses EFI or UEFI IFR protocol
*/
type getType(const string &buffer);


// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) 
{
    // Initialize variables
    static TCHAR appName[] = TEXT("IFRExtractor LS");
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclass;

    // Initialize window class
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = 0;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = HICON(LoadImage(GetModuleHandle(NULL), TEXT("IDI_ICON1"), IMAGE_ICON, 16, 16, 0));
    wndclass.hIconSm = NULL;
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = HBRUSH((COLOR_WINDOW + 1));
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = appName;

    // Register windows class
    RegisterClassEx(&wndclass);

    // Create window
    hwnd = CreateWindow(appName,
        TEXT("IFRExtractor LS v0.3.7"),
        WS_SYSMENU | WS_MINIMIZEBOX,
        0, 0,
        350, 120,
        NULL,
        NULL,
        hInstance,
        NULL);

    // Show window
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Return
    return msg.wParam;
}


// Supporting function implementation
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

    // Check message
    switch (message) {
    
	// Create
    case WM_CREATE:
        // Center window
        RECT rectangle;
        GetWindowRect(hwnd, &rectangle);
        int xPos;
        xPos = (GetSystemMetrics(SM_CXSCREEN) - rectangle.right) / 2;
        int yPos;
        yPos = (GetSystemMetrics(SM_CYSCREEN) - rectangle.bottom) / 2;
        SetWindowPos(hwnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

        // Create module location text
        HWND moduleLocationText;
        moduleLocationText = CreateWindowEx(WS_EX_TRANSPARENT,
            TEXT("static"),
            TEXT("Module Location"),
            WS_CHILD | WS_VISIBLE,
            10, 16,
            100, 25,
            hwnd,
            HMENU(NO_ACTION),
            NULL,
            NULL);
        SendMessage(moduleLocationText, WM_SETFONT, WPARAM(GetStockObject(DEFAULT_GUI_FONT)), MAKELPARAM(FALSE, 0));

        // Create protocol text
        HWND protocolText;
        protocolText = CreateWindowEx(WS_EX_TRANSPARENT,
            TEXT("static"),
            TEXT("Protocol:"),
            WS_CHILD | WS_VISIBLE,
            10, 52,
            100, 25,
            hwnd,
            HMENU(NO_ACTION),
            NULL,
            NULL);
        SendMessage(protocolText, WM_SETFONT, WPARAM(GetStockObject(DEFAULT_GUI_FONT)), MAKELPARAM(FALSE, 0));

        // Create type text
        typeText = CreateWindowEx(WS_EX_TRANSPARENT,
            TEXT("static"),
            TEXT(""),
            WS_CHILD | WS_VISIBLE,
            55, 50,
            100, 25,
            hwnd,
            HMENU(NO_ACTION),
            NULL,
            NULL);

        // Create file location edit
        fileLocationEdit = CreateWindow(TEXT("edit"),
            NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_EX_CLIENTEDGE,
            93, 13,
            200, 17,
            hwnd,
            HMENU(FILE_LOCATION_EDIT_ACTION),
            NULL,
            NULL);
        SendMessage(fileLocationEdit, WM_SETFONT, WPARAM(GetStockObject(DEFAULT_GUI_FONT)), MAKELPARAM(FALSE, 0));
        TCHAR fileLocationEditValue[MAX_PATH];
        if (__argc < 2) {
            GetEnvironmentVariable(("userprofile"), fileLocationEditValue, MAX_PATH);
            strcat_s(fileLocationEditValue, TEXT("\\module.rom"));
        }
        else
            strcpy_s(fileLocationEditValue, TEXT(__argv[1]));
        SetWindowText(fileLocationEdit, TEXT(fileLocationEditValue));

        // Create browse button
        HWND browseButton;
        browseButton = CreateWindow(TEXT("button"),
            TEXT(".."),
            WS_VISIBLE | WS_CHILD,
            298, 12,
            30, 19,
            hwnd,
            HMENU(BROWSE_BUTTON_ACTION),
            NULL,
            NULL);
        SendMessage(browseButton, WM_SETFONT, WPARAM(GetStockObject(DEFAULT_GUI_FONT)), MAKELPARAM(FALSE, 0));

        // Create extract button
        HWND extractButton;
        extractButton = CreateWindow(TEXT("button"),
            TEXT("Extract"),
            WS_VISIBLE | WS_CHILD,
            248, 45,
            80, 25,
            hwnd,
            HMENU(EXTRACT_BUTTON_ACTION),
            NULL,
            NULL);
        SendMessage(extractButton, WM_SETFONT, WPARAM(GetStockObject(DEFAULT_GUI_FONT)), MAKELPARAM(FALSE, 0));

        // Break
        break;

    // Command
    case WM_COMMAND:

        // Check if extract button action
        if (LOWORD(wParam) == EXTRACT_BUTTON_ACTION) {

            // Reset error code
            errorCode = IFR_EXTRACTED;

            // Refresh file location settings
            SetWindowText(fileLocationEdit, TEXT(fileLocation.c_str()));

            // Check if file exists
            if (fileExists(fileLocation)) {

                // Check if protocol is unknown
                if (protocol == UNKNOWN) {

                    // Set error code
                    errorCode = UNKNOWN_PROTOCOL;

                    // Display message
                    showDialog(hwnd);

                    // Break
                    break;
                }

                // Clear strings
                strings.clear();

                // Check if output file was canceled
                if (!saveFile(hwnd))
                    // Break
                    break;

                // Check if protocol is EFI
                if (protocol == EFI) {

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
                else if (protocol == UEFI) {

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

                // Set error code
                errorCode = FILE_NOT_FOUND;

            // Display message
            showDialog(hwnd);
        }

        // Otherwise check if browse button action
        else if (LOWORD(wParam) == BROWSE_BUTTON_ACTION)

            // Browse folders
            fileBrowser(hwnd);

        // Otherwise check if file location edit action
        else if (LOWORD(wParam) == FILE_LOCATION_EDIT_ACTION)

            // Check if file location edit was changed
            if (HIWORD(wParam) == EN_CHANGE) {

                // Set file location
                TCHAR tempBuffer[MAX_PATH];
                GetWindowText(fileLocationEdit, tempBuffer, MAX_PATH);
                fileLocation = tempBuffer;

                // Check if file exists
                if (fileExists(fileLocation)) {

                    // Read in file
                    readFile(fileLocation, buffer);

                    // Get protocol
                    protocol = getType(buffer);
                }

                // Otherwise
                else

                    // Set protocol
                    protocol = UNKNOWN;

                // Clear type text
                RECT rectangle;
                GetClientRect(typeText, &rectangle);
                InvalidateRect(typeText, &rectangle, TRUE);
                MapWindowPoints(typeText, hwnd, (POINT *)&rectangle, 2);
                RedrawWindow(hwnd, &rectangle, NULL, RDW_ERASE | RDW_INVALIDATE);

                // Update type text
                SendMessage(typeText, WM_SETTEXT, NULL, NULL);
            }

        break;

    // Static color
    case WM_CTLCOLORSTATIC:

        // Make static control transparent
        HDC hdcStatic;
        hdcStatic = (HDC)wParam;

        // Check if caller is type text
        if ((HWND)lParam == typeText) {

            // Check if protocol is unknown
            if (protocol == UNKNOWN) {

                // Set text
                SetWindowText(typeText, TEXT("Unknown"));

                // Set colot
                SetTextColor(hdcStatic, RGB(255, 0, 0));
            }

            // Otherwise
            else {

                // Check if protocol is EFI
                if (protocol == EFI)

                    // Set text
                    SetWindowText(typeText, TEXT("EFI"));

                // Otherwise check if protocol is UEFI
                else if (protocol == UEFI)

                    // Set text
                    SetWindowText(typeText, TEXT("UEFI"));

                // Set color
                SetTextColor(hdcStatic, RGB(0, 255, 0));
            }
        }

        // Otherwise
        else

            // Set colot
            SetTextColor(hdcStatic, RGB(0, 0, 0));

        // Make background transparent
        SetBkMode(hdcStatic, TRANSPARENT);

        // Return result
        return (LRESULT)GetStockObject(NULL_BRUSH);

    // Close
    case WM_DESTROY:

        // Quit
        PostQuitMessage(0);

        // Break
        break;
    }

    // Return
    return DefWindowProc(hwnd, message, wParam, lParam);
}

void fileBrowser(HWND hwnd) 
{
	// Initialize variables
	OPENFILENAME browserInfo;
	CHAR szFile[MAX_PATH];

	// Set dialog options
	ZeroMemory(&browserInfo, sizeof(browserInfo));
	browserInfo.lStructSize = sizeof(browserInfo);
	browserInfo.hwndOwner = hwnd;
	browserInfo.lpstrFilter = TEXT("All files(*.*)\0*.*\0");
	browserInfo.lpstrFile = szFile;
	browserInfo.lpstrFile[0] = '\0';
	browserInfo.nMaxFile = MAX_PATH;
	browserInfo.nFilterIndex = 1;
	browserInfo.lpstrInitialDir = NULL;
	browserInfo.lpstrFileTitle = NULL;
	browserInfo.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Check if file was selected
	if (GetOpenFileName(&browserInfo)) {
		// Update file location edit
		SetWindowText(fileLocationEdit, TEXT(browserInfo.lpstrFile));
	}
}

bool saveFile(HWND hwnd) 
{
    // Initialize variables
    OPENFILENAME browserInfo;
    string fileName = fileLocation.substr(0, fileLocation.find_last_of('.')) + " IFR";
    fileName.reserve(MAX_PATH);

    // Set dialog options
    ZeroMemory(&browserInfo, sizeof(browserInfo));
    browserInfo.lStructSize = sizeof(browserInfo);
    browserInfo.hwndOwner = hwnd;
    browserInfo.lpstrFilter = (LPCSTR)"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    browserInfo.lpstrFile = (LPSTR)fileName.c_str();
    browserInfo.nMaxFile = MAX_PATH;
    browserInfo.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    browserInfo.lpstrDefExt = (LPCSTR)"txt";

    // Check if file was selected
    if (GetSaveFileName(&browserInfo)) {

        // Set output file
        outputFile = browserInfo.lpstrFile;

        // Return true
        return true;
    }

    // Return false
    return false;
}

void showDialog(HWND hwnd) 
{
    // Initialize variables
    string header, message;

    // Check if error code is IFR extracted
    if (errorCode == IFR_EXTRACTED) {

        // Set header
        header = "Message";

        // Set message
        message = "IFR extracted successfully";

        // Display message
        MessageBox(hwnd, TEXT(message.c_str()), TEXT(header.c_str()), MB_OK | MB_ICONINFORMATION);
    }

    // Otherwise
    else {

        // Set header
        header = "Error";

        // Check if error code equals file not found
        if (errorCode == FILE_NOT_FOUND)

            // Set message
            message = "File not found";

        // Otherwise check if error code equals unknown protocol
        else if (errorCode == UNKNOWN_PROTOCOL)

            // Set message
            message = "Unknown protocol detected";

        // Display message
        MessageBox(hwnd, TEXT(message.c_str()), TEXT(header.c_str()), MB_OK | MB_ICONWARNING);
    }
}

bool fileExists(const string &file) 
{
    // Open file
#if (_MSC_VER < 1600)
    ifstream fin(file.c_str());
#else
    ifstream fin(file);
#endif

    // Return if first character doesn't equal EOF
    return fin.peek() != EOF;
}

void readFile(const string &file, string &buffer) 
{
    // Initialize variables
#if (_MSC_VER < 1600)
    ifstream fin(file.c_str(), ios::binary);
#else
    ifstream fin(file, ios::binary);
#endif

    // Clear buffer
    buffer.clear();

    // Read in file
    while (fin.peek() != EOF)
        buffer.push_back(fin.get());

    // Close file
    fin.close();
}

type getType(const string &buffer)
{

    type currtype = UNKNOWN;

    // Go through buffer
    for(unsigned int i = 0; i < buffer.size() - 9; i++) {
    
        // Check if an EFI string package was found
        if((buffer[i] != '\x00' || buffer[i + 1] != '\x00' || buffer[i + 2] != '\x00' || buffer[i + 3] != '\x00') && buffer[i + 4] == '\x02' && buffer[i + 5] == '\x00' && (buffer[i + 6] != '\x00' || buffer[i + 7] != '\x00' || buffer[i + 8] != '\x00' || buffer[i + 9] != '\x00') && i + static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24) + 4] >= 'a' && buffer[i + static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24) + 4] <= 'z' && i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) + 4] == '\x02' && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) + 5] == '\x00') {
            // Notice legacy EFI
            currtype = EFI;
        // Otherwise check if a UEFI string package was found
        } else if((buffer[i] != '\x00' || buffer[i + 1] != '\x00' || buffer[i + 2] != '\x00') && buffer[i + 3] == '\x04' && buffer[i + 4] == '\x34' && buffer[i + 44] == '\x01' && buffer[i + 45] == '\x00' && buffer[i + 48] == '\x2D' && i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 1] == '\x00' && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 2] == '\x00') {
            // Prefer UEFI over EFI
            return UEFI;
        }
    }
    
    return currtype;
}
