#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <tlhelp32.h>
#include <cstdlib>
#include <tchar.h>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <GL/gl.h>
#include <wincodec.h> // For WIC
#pragma comment(lib, "windowscodecs.lib") // Link against the WIC library

#define ADDR_CLDBDOSOMESTUFF 0x100010A0
#define ADDR_CLDBDOSOMEOTHERSTUFF 0x10001000
#define ADDR_CLDBDOYETMORESTUFF 0x100011F0

bool isFocusInstalled = false; // Global flag

// create global hWND variable

HWND focusHWND = NULL;

HWND bringWindowToTopHWND = NULL;

HWND setWindowFocusHWND = NULL;
HWND setWindowFocushWndInsertAfter = NULL;
int setWindowFocusX = 0;
int setWindowFocusY = 0;
int setWindowFocuscx = 0;
int setWindowFocuscy = 0;
UINT setWindowFocusuFlags = 0;



BYTE originalBytesForGetForeground[5] = { 0 };
BYTE originalBytesForShowWindow[5] = { 0 };
BYTE originalBytesForSetWindowPos[5] = { 0 };
BYTE orginalBytesForSetFocus[5] = { 0 };
BYTE originalBytesForOpenProcess[5] = { 0 };
BYTE originalBytesForTerminateProcess[5] = { 0 };
BYTE originalBytesForExitProcess[5] = { 0 };
BYTE originalBytesForEmptyClipboard[5] = { 0 };
BYTE originalBytesForSetClipboardData[5] = { 0 };
BYTE originalBytesForCreateFileA[5] = { 0 };
BYTE originalBytesForDoSomeStuff[5] = { 0 };
BYTE originalBytesForDoSomeOtherStuff[5] = { 0 };
BYTE originalBytesForDoYetMoreStuff[5] = { 0 };

HANDLE(WINAPI* originalCreateFileA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE) = nullptr;


// My custom functions
HWND WINAPI MyGetForegroundWindow();
BOOL WINAPI MyShowWindow(HWND hWnd);
BOOL WINAPI MySetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
HWND WINAPI MySetFocus(HWND hWnd);
HWND WINAPI MyGetWindow(HWND hWnd, UINT uCmd);
BOOL WINAPI MyEnumWindows(WNDENUMPROC lpEnumFunc, LPARAM lParam);
int WINAPI MyGetWindowTextW(HWND hWnd, LPWSTR lpString, int nMaxCount);
BOOL WINAPI MyK32EnumProcesses(DWORD* pProcessIds, DWORD cb, DWORD* pBytesReturned);
HANDLE WINAPI MyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
BOOL WINAPI MyTerminateProcess(HANDLE hProcess, UINT uExitCode);
VOID WINAPI MyExitProcess(UINT uExitCode);
BOOL WINAPI MyEmptyClipboard();
HANDLE WINAPI MySetClipboardData(UINT uFormat, HANDLE hMem);
HANDLE WINAPI MyCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);


extern "C" int MyCLDBDoSomeStuff(int* a1) {
    if (*a1 >> 6 & 1) {
        // todo check if this actually matters
        auto v1 = 1024;
        v1 += 2048;
        auto v6 = v1 + 2048;
        *a1 += 2 * v6;

        return v6;
    }

    return 0;
}

extern "C" bool MyCLDBDoSomeOtherStuff(int* a1) {
    // todo check if this actually matters
    *a1 += 2048;

    return TRUE; // In C++, true is the equivalent of TRUE in C.
}

extern "C" bool MyCLDBDoYetMoreStuff() {
    return TRUE; // true in C++.
}

HANDLE WINAPI MyCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    if (dwShareMode == 0) {
        dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        std::cout << "Intercepted CreateFileA: Modified dwShareMode to allow read, write, delete sharing." << std::endl;
    }

    // Call the original CreateFileA function using the saved function pointer
    return originalCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

// Implement MySetClipboardData which does nothing
HANDLE WINAPI MySetClipboardData(UINT uFormat, HANDLE hMem) {
    // This custom function does nothing
    std::cout << "SetClipboardData hook called, but not setting clipboard data." << std::endl;
    return NULL; // Indicate failure or that the data was not set
}

BOOL WINAPI MyEmptyClipboard() {
    // This custom function pretends to clear the clipboard but does nothing
    std::cout << "EmptyClipboard hook called, but not clearing the clipboard." << std::endl;
    return TRUE; // Pretend success
}

HANDLE WINAPI MyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId) {
    std::cout << "OpenProcess hook called, but not opening process." << std::endl;
    return NULL;
}

BOOL WINAPI MyTerminateProcess(HANDLE hProcess, UINT uExitCode) {
    std::cout << "TerminateProcess hook called, but not terminating process." << std::endl;
    //   if (hProcess == OpenProcess(PROCESS_TERMINATE, FALSE, findDiscordProcessId())) {
       //	std::cout << "Discord process found, but not terminating process." << std::endl;
       //	return TRUE; // Simulate success
       //}
    return FALSE;
}

VOID WINAPI MyExitProcess(UINT uExitCode) {
    std::cout << "ExitProcess hook called, but not exiting process." << std::endl;
}

BOOL WINAPI MyK32EnumProcesses(DWORD* pProcessIds, DWORD cb, DWORD* pBytesReturned) {
    // This custom function behaves as if no processes are running
    std::cout << "K32EnumProcesses hook called, but pretending no processes exist." << std::endl;
    if (pBytesReturned != NULL) {
        *pBytesReturned = 0; // Indicate no processes were written to the buffer
    }
    return TRUE; // Indicate the function succeeded
}

int WINAPI MyGetWindowTextW(HWND hWnd, LPWSTR lpString, int nMaxCount) {
    // This custom function behaves as if no window title is retrieved
    std::cout << "GetWindowTextW hook called, but not returning actual window text." << std::endl;
    if (nMaxCount > 0) {
        lpString[0] = L'\0'; // Return an empty string
    }
    return 0; // Indicate that no characters were copied to the buffer
}

BOOL WINAPI MySetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    setWindowFocusHWND = hWnd;
    setWindowFocushWndInsertAfter = hWndInsertAfter;
    setWindowFocusX = X;
    setWindowFocusY = Y;
    setWindowFocuscx = cx;
    setWindowFocuscy = cy;
    setWindowFocusuFlags = uFlags;
    // This custom function does nothing
    std::cout << "SetWindowPos hook called, but not changing window position." << std::endl;
    return TRUE; // Pretend success
}

BOOL WINAPI MyShowWindow(HWND hWnd) {
    bringWindowToTopHWND = hWnd;
    // This custom function does nothing
    std::cout << "ShowWindow hook called, but not bringing window to top." << std::endl;
    return TRUE; // Pretend success
}

HWND WINAPI MyGetWindow(HWND hWnd, UINT uCmd) {
    // This custom function behaves as if there are no windows to return
    std::cout << "GetWindow hook called, but pretending no related window." << std::endl;
    return NULL; // Indicate no window found
}

BOOL WINAPI MyEnumWindows(WNDENUMPROC lpEnumFunc, LPARAM lParam) {
    // This custom function behaves as if no windows are open
    std::cout << "EnumWindows hook called, but pretending no windows exist." << std::endl;
    return TRUE; // Indicate success but don't actually call the callback
}

void InstallDllHook(void* targetFunction, void* myFunction, BYTE* originalBytes) {
    DWORD oldProtect;
    // Attempt to change the memory protection to modify the target function
    if (VirtualProtect(targetFunction, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        memcpy(originalBytes, targetFunction, 5); // Backup original bytes

        DWORD offset = (DWORD)myFunction - (DWORD)targetFunction - 5;
        *((BYTE*)targetFunction) = 0xE9; // JMP instruction
        *((DWORD*)((BYTE*)targetFunction + 1)) = offset;

        // Restore the original memory protection
        VirtualProtect(targetFunction, 5, oldProtect, &oldProtect);
    }
    else {
        std::cerr << "Failed to change memory protection for hook installation." << std::endl;
    }
}


void InstallHook() {
    std::cout << "Installing hooks..." << std::endl;
    DWORD oldProtect;

    /*InstallDllHook((void*)ADDR_CLDBDOSOMESTUFF, MyCLDBDoSomeStuff, originalBytesForDoSomeStuff);
    InstallDllHook((void*)ADDR_CLDBDOSOMEOTHERSTUFF, MyCLDBDoSomeOtherStuff, originalBytesForDoSomeOtherStuff);
    InstallDllHook((void*)ADDR_CLDBDOYETMORESTUFF, MyCLDBDoYetMoreStuff, originalBytesForDoYetMoreStuff);*/

    //// Hook CreateFileA
    //void* targetCreateFileA = CreateFileA;
    //DWORD jumpCreateFileA = (DWORD)MyCreateFileA - (DWORD)targetCreateFileA - 5;
    //memcpy(originalBytesForCreateFileA, targetCreateFileA, sizeof(originalBytesForCreateFileA));
    //VirtualProtect(targetCreateFileA, sizeof(originalBytesForCreateFileA), PAGE_EXECUTE_READWRITE, &oldProtect);
    //*((BYTE*)targetCreateFileA) = 0xE9;
    //*((DWORD*)((BYTE*)targetCreateFileA + 1)) = jumpCreateFileA;
    //VirtualProtect(targetCreateFileA, sizeof(originalBytesForCreateFileA), oldProtect, &oldProtect);


    //// Hook SetClipboardData
    //void* targetSetClipboardData = SetClipboardData;
    //DWORD jumpSetClipboardData = (DWORD)MySetClipboardData - (DWORD)targetSetClipboardData - 5;
    //memcpy(originalBytesForSetClipboardData, targetSetClipboardData, sizeof(originalBytesForSetClipboardData));
    //VirtualProtect(targetSetClipboardData, sizeof(originalBytesForSetClipboardData), PAGE_EXECUTE_READWRITE, &oldProtect);
    //*((BYTE*)targetSetClipboardData) = 0xE9;
    //*((DWORD*)((BYTE*)targetSetClipboardData + 1)) = jumpSetClipboardData;
    //VirtualProtect(targetSetClipboardData, sizeof(originalBytesForSetClipboardData), oldProtect, &oldProtect);

    // Hook EmptyClipboard
    void* targetEmptyClipboard = EmptyClipboard;
    DWORD jumpEmptyClipboard = (DWORD)MyEmptyClipboard - (DWORD)targetEmptyClipboard - 5;
    memcpy(originalBytesForEmptyClipboard, targetEmptyClipboard, sizeof(originalBytesForEmptyClipboard));
    VirtualProtect(targetEmptyClipboard, sizeof(originalBytesForEmptyClipboard), PAGE_EXECUTE_READWRITE, &oldProtect);
    *((BYTE*)targetEmptyClipboard) = 0xE9;
    *((DWORD*)((BYTE*)targetEmptyClipboard + 1)) = jumpEmptyClipboard;
    VirtualProtect(targetEmptyClipboard, sizeof(originalBytesForEmptyClipboard), oldProtect, &oldProtect);

    // Hook GetForegroundWindow
    void* targetGetForegroundWindow = GetForegroundWindow;
    DWORD jumpGetForeground = (DWORD)MyGetForegroundWindow - (DWORD)targetGetForegroundWindow - 5;
    memcpy(originalBytesForGetForeground, targetGetForegroundWindow, sizeof(originalBytesForGetForeground));
    VirtualProtect(targetGetForegroundWindow, sizeof(originalBytesForGetForeground), PAGE_EXECUTE_READWRITE, &oldProtect);
    *((BYTE*)targetGetForegroundWindow) = 0xE9;
    *((DWORD*)((BYTE*)targetGetForegroundWindow + 1)) = jumpGetForeground;
    VirtualProtect(targetGetForegroundWindow, sizeof(originalBytesForGetForeground), oldProtect, &oldProtect);

    // Hook TerminateProcess
    void* targetTerminateProcess = TerminateProcess;
    DWORD jumpTerminateProcess = ((DWORD)MyTerminateProcess - (DWORD)targetTerminateProcess - 5);
    memcpy(originalBytesForTerminateProcess, targetTerminateProcess, sizeof(originalBytesForTerminateProcess));
    VirtualProtect(targetTerminateProcess, sizeof(originalBytesForTerminateProcess), PAGE_EXECUTE_READWRITE, &oldProtect);
    *((BYTE*)targetTerminateProcess) = 0xE9;
    *((DWORD*)((BYTE*)targetTerminateProcess + 1)) = jumpTerminateProcess;
    VirtualProtect(targetTerminateProcess, sizeof(originalBytesForTerminateProcess), oldProtect, &oldProtect);

    // Hook ExitProcess
    void* targetExitProcess = ExitProcess;
    DWORD jumpExitProcess = ((DWORD)MyExitProcess - (DWORD)targetExitProcess - 5);
    memcpy(originalBytesForExitProcess, targetExitProcess, sizeof(originalBytesForExitProcess));
    VirtualProtect(targetExitProcess, sizeof(originalBytesForExitProcess), PAGE_EXECUTE_READWRITE, &oldProtect);
    *((BYTE*)targetExitProcess) = 0xE9;
    *((DWORD*)((BYTE*)targetExitProcess + 1)) = jumpExitProcess;
    VirtualProtect(targetExitProcess, sizeof(originalBytesForExitProcess), oldProtect, &oldProtect);

    // create a message box to show the dll is loaded
    MessageBox(NULL, L"Injected :)", L"UndownUnlock", MB_OK);
}

void InstallFocus() {
    if (isFocusInstalled) {
        return;
    }
    isFocusInstalled = true;
    std::cout << "Installing focus..." << std::endl;
    DWORD oldProtect;

    // Hook BringWindowToTop
    void* targetShowWindow = BringWindowToTop;
    DWORD jumpBringWindowToTop = (DWORD)MyShowWindow - (DWORD)targetShowWindow - 5;
    memcpy(originalBytesForShowWindow, targetShowWindow, sizeof(originalBytesForShowWindow));
    VirtualProtect(targetShowWindow, sizeof(originalBytesForShowWindow), PAGE_EXECUTE_READWRITE, &oldProtect);
    *((BYTE*)targetShowWindow) = 0xE9;
    *((DWORD*)((BYTE*)targetShowWindow + 1)) = jumpBringWindowToTop;
    VirtualProtect(targetShowWindow, sizeof(originalBytesForShowWindow), oldProtect, &oldProtect);

    // Hook SetWindowPos
    void* targetSetWindowPos = SetWindowPos;
    DWORD jumpSetWindowPos = (DWORD)MySetWindowPos - (DWORD)targetSetWindowPos - 5;
    memcpy(originalBytesForSetWindowPos, targetSetWindowPos, sizeof(originalBytesForSetWindowPos));
    VirtualProtect(targetSetWindowPos, sizeof(originalBytesForSetWindowPos), PAGE_EXECUTE_READWRITE, &oldProtect);
    *((BYTE*)targetSetWindowPos) = 0xE9;
    *((DWORD*)((BYTE*)targetSetWindowPos + 1)) = jumpSetWindowPos;
    VirtualProtect(targetSetWindowPos, sizeof(originalBytesForSetWindowPos), oldProtect, &oldProtect);

    // Hook SetFocus
    void* targetSetFocus = SetFocus;
    DWORD jumpSetFocus = (DWORD)MySetFocus - (DWORD)targetSetFocus - 5;
    memcpy(orginalBytesForSetFocus, targetSetFocus, sizeof(orginalBytesForSetFocus));
    VirtualProtect(targetSetFocus, sizeof(orginalBytesForSetFocus), PAGE_EXECUTE_READWRITE, &oldProtect);
    *((BYTE*)targetSetFocus) = 0xE9;
    *((DWORD*)((BYTE*)targetSetFocus + 1)) = jumpSetFocus;
    VirtualProtect(targetSetFocus, sizeof(orginalBytesForSetFocus), oldProtect, &oldProtect);
}

void UninstallFocus() {
    if (!isFocusInstalled) {
        return;
    }
    isFocusInstalled = false;
    std::cout << "Uninstalling focus..." << std::endl;
    DWORD oldProtect;

    // Unhook BringWindowToTop
    void* targetShowWindow = BringWindowToTop;
    VirtualProtect(targetShowWindow, sizeof(originalBytesForShowWindow), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(targetShowWindow, originalBytesForShowWindow, sizeof(originalBytesForShowWindow));
    VirtualProtect(targetShowWindow, sizeof(originalBytesForShowWindow), oldProtect, &oldProtect);

    // Unhook SetWindowPos
    void* targetSetWindowPos = SetWindowPos;
    VirtualProtect(targetSetWindowPos, sizeof(originalBytesForSetWindowPos), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(targetSetWindowPos, originalBytesForSetWindowPos, sizeof(originalBytesForSetWindowPos));
    VirtualProtect(targetSetWindowPos, sizeof(originalBytesForSetWindowPos), oldProtect, &oldProtect);

    // Unhook SetFocus
    void* targetSetFocus = SetFocus;
    VirtualProtect(targetSetFocus, sizeof(orginalBytesForSetFocus), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(targetSetFocus, orginalBytesForSetFocus, sizeof(orginalBytesForSetFocus));
    VirtualProtect(targetSetFocus, sizeof(orginalBytesForSetFocus), oldProtect, &oldProtect);
}

void UninstallHook() {
    DWORD oldProtect;

    // Unhook SetClipboardData
    void* targetSetClipboardData = SetClipboardData;
    VirtualProtect(targetSetClipboardData, sizeof(originalBytesForSetClipboardData), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(targetSetClipboardData, originalBytesForSetClipboardData, sizeof(originalBytesForSetClipboardData));
    VirtualProtect(targetSetClipboardData, sizeof(originalBytesForSetClipboardData), oldProtect, &oldProtect);

    // Unhook EmptyClipboard
    void* targetEmptyClipboard = EmptyClipboard;
    VirtualProtect(targetEmptyClipboard, sizeof(originalBytesForEmptyClipboard), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(targetEmptyClipboard, originalBytesForEmptyClipboard, sizeof(originalBytesForEmptyClipboard));
    VirtualProtect(targetEmptyClipboard, sizeof(originalBytesForEmptyClipboard), oldProtect, &oldProtect);

    // Unhook GetForegroundWindow
    void* targetGetForegroundWindow = GetForegroundWindow;
    VirtualProtect(targetGetForegroundWindow, sizeof(originalBytesForGetForeground), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(targetGetForegroundWindow, originalBytesForGetForeground, sizeof(originalBytesForGetForeground));
    VirtualProtect(targetGetForegroundWindow, sizeof(originalBytesForGetForeground), oldProtect, &oldProtect);
}

// Helper function to determine if the given window is the main window of the current process
BOOL IsMainWindow(HWND handle) {
    return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

// Callback function for EnumWindows
BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam) {
    DWORD processID = 0;
    GetWindowThreadProcessId(handle, &processID);
    if (GetCurrentProcessId() == processID && IsMainWindow(handle)) {
        // Stop enumeration if a main window is found, and return its handle
        *reinterpret_cast<HWND*>(lParam) = handle;
        return FALSE;
    }
    return TRUE;
}

// Function to find the main window of the current process
HWND FindMainWindow() {
    HWND mainWindow = NULL;
    EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&mainWindow));
    return mainWindow;
}


// Helper function to determine if the given window has the title "Respondus LockDown Browser"
BOOL IsTargetWindowWithTitle(HWND handle, LPCSTR targetTitle) {
    const int bufferSize = 256;
    wchar_t title[bufferSize];
    if (GetWindowText(handle, title, bufferSize) > 0) {
        // Check if the window's title is the one we're looking for
        if (wcscmp(title, L"Respondus LockDown Browser") == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

// Callback function for EnumWindows
BOOL CALLBACK TargetEnumWindowsCallback(HWND handle, LPARAM lParam) {
    DWORD processID = 0;
    GetWindowThreadProcessId(handle, &processID);
    // Use a specific window title instead of checking if it's the main window
    LPCSTR targetTitle = "LockDown Browser";
    if (IsTargetWindowWithTitle(handle, targetTitle)) {
        // Stop enumeration if the target window is found, and return its handle
        *reinterpret_cast<HWND*>(lParam) = handle;
        return FALSE; // return FALSE to stop enumeration
    }
    return TRUE; // return TRUE to continue enumeration
}

// Function to find the window with title "Respondus LockDown Browser"
HWND FindTargetWindow() {
    HWND targetWindow = NULL;
    EnumWindows(TargetEnumWindowsCallback, reinterpret_cast<LPARAM>(&targetWindow));
    return targetWindow;
}

// Define a structure to hold window information
struct WindowInfo {
    std::wstring title;
};

// Callback function to enumerate windows
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    const int bufferSize = 256;
    wchar_t title[bufferSize];
    std::vector<WindowInfo>* windows = reinterpret_cast<std::vector<WindowInfo>*>(lParam);

    if (GetWindowText(hwnd, title, bufferSize) > 0 && IsWindowVisible(hwnd)) {
        // Add window title to the vector if it has a title and is visible
        windows->push_back(WindowInfo{ title });
    }
    return TRUE; // Continue enumeration
}

// Function to write window titles to a file
void WriteWindowTitlesToFile(const std::vector<WindowInfo>& windows, const std::wstring& filePath) {
    std::wofstream file(filePath);
    if (file.is_open()) {
        for (const auto& window : windows) {
            file << window.title << std::endl;
        }
        file.close();
    }
    else {
        // Handle file open error
        MessageBoxW(NULL, L"Unable to open file for writing.", L"Error", MB_ICONERROR);
    }
}

// Your hook function implementation
HWND WINAPI MyGetForegroundWindow() {
    HWND hWnd = FindMainWindow();
    if (hWnd != NULL) {
        std::cout << "Returning the main window of the current application." << std::endl;
        return hWnd;
    }
    std::cout << "Main window not found, returning NULL." << std::endl;
    return NULL;
}

HWND WINAPI MySetFocus(HWND _hWnd) {
    focusHWND = _hWnd;
    HWND hWnd = FindMainWindow(); // Find the main window of the current process
    if (hWnd != NULL) {
        std::cout << "Returning the main window of the current application due to '[' key press." << std::endl;
        return hWnd; // Return the main window handle if found
    }
    else {
        std::cout << "Main window not found, returning NULL." << std::endl;
        return NULL; // If main window is not found, return NULL
    }
}

// Keyboard hook handle
HHOOK hKeyboardHook;

// Keyboard hook callback
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        if (wParam == WM_KEYDOWN) {
            switch (p->vkCode) {
                // VK_UP is the virtual key code for the Up arrow key
            case VK_UP:
                //CaptureOpenGLScreen();
                InstallFocus();
                std::cout << "Up arrow key pressed, installing focus hook." << std::endl;
                break;
                // VK_DOWN is the virtual key code for the Down arrow key
            case VK_DOWN:
                UninstallFocus();
                // call the set focus with the focusHWND
                if (focusHWND != NULL) {
                    SetFocus(focusHWND);
                }
                if (bringWindowToTopHWND != NULL) {
                    BringWindowToTop(bringWindowToTopHWND);
                }
                if (setWindowFocusHWND != NULL) {
                    SetWindowPos(setWindowFocusHWND, setWindowFocushWndInsertAfter, setWindowFocusX, setWindowFocusY, setWindowFocuscx, setWindowFocuscy, setWindowFocusuFlags);
                }
                std::cout << "Down arrow key pressed, uninstalling focus hook." << std::endl;
                break;
            }
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

// Place this in a suitable location in your existing code.
void SetupKeyboardHook() {
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        // Start the hook setup in a new thread to keep DllMain non-blocking

        InstallHook(); // Sets up your custom 
        std::thread([]() {
            SetupKeyboardHook(); // Sets up the keyboard hook
            }).detach();
            break;
    case DLL_PROCESS_DETACH:
        UninstallHook(); // Uninstall your custom hooks
        if (hKeyboardHook != nullptr) {
            UnhookWindowsHookEx(hKeyboardHook); // Uninstall the keyboard hook
        }
        break;
    }
    return TRUE;
}