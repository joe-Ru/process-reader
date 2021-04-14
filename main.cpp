#include <iostream>
#include <Windows.h>
#include <string>
#include <processthreadsapi.h>
#include <tlhelp32.h>
#include <libloaderapi.h>
#include <psapi.h>
#include <tchar.h>


using namespace std;

HANDLE GetProcess(int pID) {

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);

    if (hProcess == nullptr) {
        cout << "OpenProcess failed. GetLastError = " << dec << GetLastError() << endl;
        return 0;
    }

    return hProcess;
}


int main() {
    while (true) {

        int processID = 0;

        cout << "Enter the PID: ";
        cin >> processID;

        //Check if process ID is open
        HANDLE hProcess = GetProcess(processID);

        HMODULE hMods[1024];
        DWORD cbNeeded;
        HMODULE firstMod = nullptr;
        unsigned int i;

        if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
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
        bool ModuleInfo = GetModuleInformation(hProcess, firstMod, &ModInfo, sizeof(MODULEINFO));

        if(ModuleInfo) {
            cout << "Size Of The Image: " << ModInfo.SizeOfImage << endl;
            cout << "Base of the DLL: " << ModInfo.lpBaseOfDll << endl;
            cout << "Entry Point: " << ModInfo.EntryPoint << endl;
        } else {
            cout << dec << GetLastError() << endl;
        }

        system("pause>nul");
        CloseHandle(hProcess);
    }
    return 0;
}

