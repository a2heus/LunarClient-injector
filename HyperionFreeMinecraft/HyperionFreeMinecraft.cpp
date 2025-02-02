#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <urlmon.h>
#include <string>
#include <thread>

#pragma comment(lib, "urlmon.lib")

#define COLOR_DEFAULT 7
#define COLOR_ERROR 12
#define COLOR_SUCCESS 10
#define COLOR_INFO 11

void SetConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void PrintWithColor(const std::wstring& message, int color, int sleepTime = 1000) {
    SetConsoleColor(color);
    std::wcout << message << std::endl;
    SetConsoleColor(COLOR_DEFAULT);
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
}

DWORD GetProcessID(const std::wstring& processName) {
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(PROCESSENTRY32W);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    DWORD processID = 0;
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (processName == entry.szExeFile) {
                processID = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return processID;
}

bool DownloadDLL(const std::wstring& url, const std::wstring& outputPath) {
    HRESULT hr = URLDownloadToFileW(NULL, url.c_str(), outputPath.c_str(), 0, NULL);
    return SUCCEEDED(hr);
}

bool InjectDLL(DWORD processID, const std::wstring& dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (!hProcess) return false;

    LPVOID pAlloc = VirtualAllocEx(hProcess, NULL, (dllPath.size() + 1) * sizeof(wchar_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pAlloc) {
        CloseHandle(hProcess);
        return false;
    }

    if (!WriteProcessMemory(hProcess, pAlloc, dllPath.c_str(), (dllPath.size() + 1) * sizeof(wchar_t), NULL)) {
        VirtualFreeEx(hProcess, pAlloc, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) {
        VirtualFreeEx(hProcess, pAlloc, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    LPVOID pLoadLibraryW = GetProcAddress(hKernel32, "LoadLibraryW");
    if (!pLoadLibraryW) {
        VirtualFreeEx(hProcess, pAlloc, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryW, pAlloc, 0, NULL);
    if (!hThread) {
        VirtualFreeEx(hProcess, pAlloc, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    CloseHandle(hThread);
    CloseHandle(hProcess);
    return true;
}

int main() {
    SetConsoleColor(COLOR_INFO);
    std::wcout << L"==========================================" << std::endl;
    std::wcout << L"          Hyperion Free Injector          " << std::endl;
    std::wcout << L"==========================================" << std::endl;
    SetConsoleColor(COLOR_DEFAULT);

    std::wstring url = L"https://example.com";  // URL of dll
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring dllPath = std::wstring(tempPath) + L"HyperionFree.dll";

    PrintWithColor(L"[/] Downloading...", COLOR_INFO, 1500);
    if (!DownloadDLL(url, dllPath)) {
        PrintWithColor(L"[-] Error while downloading.", COLOR_ERROR);
        return 1;
    }
    PrintWithColor(L"[+] Succesfully downloaded !.", COLOR_SUCCESS, 1000);

    PrintWithColor(L"[/] Searching for Lunar Client...", COLOR_INFO, 1500);
    DWORD processID = GetProcessID(L"javaw.exe");
    if (processID == 0) {
        PrintWithColor(L"[-] Lunar Client isint launched...", COLOR_ERROR);
        return 1;
    }
    PrintWithColor(L"[+] Lunar client found ! ID: " + std::to_wstring(processID), COLOR_SUCCESS, 1000);

    PrintWithColor(L"[/] Injecting into Lunar Client...", COLOR_INFO, 2000);
    if (!InjectDLL(processID, dllPath)) {
        PrintWithColor(L"[-] Error while Injecting.", COLOR_ERROR);
        return 1;
    }

    PrintWithColor(L"[+] Hyperion Minecraft Free Started !", COLOR_SUCCESS, 1000);
    PrintWithColor(L"[/] Appuyez sur INS pour ouvrir le menu.", COLOR_INFO, 1000);

    return 0;
}
