#include <iostream>
#include <Windows.h>
#include <string>
#include <processthreadsapi.h>
#include <tlhelp32.h>
#include <libloaderapi.h>
#include <psapi.h>
#include <tchar.h>
#include <sstream>


using namespace std;

#define MY_TRACE(msg, ...) MyTraceImpl(__LINE__, __FILE__, msg, __VA_ARGS__)

void MyTraceImpl(int line, const char* fileName, const char* msg, ...)
{
    va_list args;
    char buffer[256] = { 0 };
    sprintf_s(buffer, "%s(%d) : ", fileName, line);
    OutputDebugString(buffer);

    // retrieve the variable arguments
            va_start(args, msg);
    vsprintf_s(buffer, msg, args);
    OutputDebugString(buffer);
            va_end(args);
}

HANDLE GetProcess(int pID) {

    HANDLE hProcess = OpenProcess(
            PROCESS_ALL_ACCESS,
            FALSE,
            pID);

    if (hProcess == nullptr) {
        cout << "OpenProcess failed. GetLastError = " << dec << GetLastError() << endl;
        return 0;
    }

    return hProcess;
}


void WriteToTargetProcess(HANDLE hProcess) {

    int memoryAddress = 0;
    int intToWrite = 0;
    int intToRead = 0;
    cout << "Enter the address for the target process: " << endl;
    cin >> hex >> memoryAddress;

    bool readingProcess = ReadProcessMemory(
            hProcess,
            (LPCVOID)memoryAddress,
            &intToRead,
            sizeof(int),
            NULL);

    readingProcess? cout << "Changing value: " << intToRead << endl: cout << "Reading Process Failed: " << GetLastError() << endl;

    cout << "Enter the value to be inserted: " << endl;
    cin >> dec >> intToWrite;

    bool writeProcess = WriteProcessMemory(
            hProcess,
            (LPVOID)memoryAddress,
            &intToWrite,
            sizeof(int),
            NULL);

    writeProcess? cout << "Value changed" << endl: cout << "Writing Process Failed: " << GetLastError() << endl;

}

void testDebug() {
    string myVar = "Hello";
    MY_TRACE("hello world! %d, %d\n", 10, 42);
}

int main() {

    testDebug();

    int processID = 0;

    cout << "Enter the PID: ";
    cin >> processID;

    //Check if process ID is open
    HANDLE hProcess = GetProcess(processID);

    HMODULE hMods[1024];
    DWORD cbNeeded;
    HMODULE firstMod = nullptr;
    unsigned int i;

    if (EnumProcessModules(
            hProcess,
            hMods,
            sizeof(hMods),
            &cbNeeded)) {
        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            TCHAR szModName[MAX_PATH];
            // Get the full path to the module's file.
            if (GetModuleFileNameEx(
                    hProcess,
                    hMods[i],
                    szModName,
                    sizeof(szModName) / sizeof(TCHAR))) {
                // Print the module name and handle value.
                _tprintf(TEXT("\t%s (0x%08X)\n"), szModName, hMods[i]);
            }
            firstMod = hMods[0];
        }
    } else {
        cout << "Enum Process Failed: " << endl;
        cout << dec << GetLastError() << endl;
    }

//        TCHAR baseName[MAX_PATH];
//        DWORD ModuleName = GetModuleBaseNameA(hProcess, firstMod, baseName, sizeof(baseName) / sizeof(TCHAR));
//
//        cout << baseName << endl;

    MODULEINFO ModInfo;
    bool ModuleInfo = GetModuleInformation(
            hProcess,
            firstMod,
            &ModInfo,
            sizeof(MODULEINFO));

    if(ModuleInfo) {
        cout << "Size Of The Image: " << ModInfo.SizeOfImage << endl;
        cout << "Base of the DLL: " << ModInfo.lpBaseOfDll << endl;
        cout << "Entry Point: " << ModInfo.EntryPoint << endl;
    } else {
        cout << dec << GetLastError() << endl;
    }

    WriteToTargetProcess(hProcess);

    system("pause>nul");
    CloseHandle(hProcess);
    return 0;
}