#define UNICODE
#define _UNICODE

#include <windows.h>
#include <commctrl.h>
#include <psapi.h>
#include <wchar.h>
#include "taskmanager.h"
#include <tchar.h>

void ListRunningProcesses(HWND hwndListView);
void TerminateProcessByPid(HWND hwnd, DWORD processId);

// Task Manager Window Procedure

LRESULT CALLBACK TaskManagerWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == 2) {  // "Refresh" button clicked
                ListRunningProcesses(GetDlgItem(hwnd, 1));
            }

            else if (LOWORD(wParam) == 3) {  // "Terminate" button clicked
                HWND hwndListView = GetDlgItem(hwnd, 1);
                int selectedItem = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);

                if (selectedItem != -1) {
                    wchar_t processIdStr[32] = {0};  
                    ListView_GetItemText(hwndListView, selectedItem, 1, processIdStr, 31);  
                    
                    // Just some debugging garbage. Enable if encountering difficulties
                    // Debugging: Show raw Process ID string retrieved from ListView
                    //wchar_t debugMessage[128];
                    //swprintf(debugMessage, 128, L"Retrieved Process ID: %s", processIdStr);
                    //MessageBox(hwnd, debugMessage, L"Debug", MB_OK);

                    DWORD processID = wcstoul(processIdStr, NULL, 10); // Convert Process ID

                    // Debugging: Show converted Process ID
                    //swprintf(debugMessage, 128, L"Converted Process ID: %lu", processID);
                    //MessageBox(hwnd, debugMessage, L"Debug", MB_OK);

                    if (processID > 0) {
                        TerminateProcessByPid(hwnd, processID);
                    } else {
                        MessageBox(hwnd, L"Invalid process ID.", L"Error", MB_ICONERROR);
                    }
                } else {
                    MessageBox(hwnd, L"No process selected.", L"Error", MB_ICONERROR);
                }
            }
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


void CreateTaskManagerWindow(HWND hwndParent) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = TaskManagerWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"TaskManagerWindowClass";
    RegisterClass(&wc);

    HWND hwndTaskManager = CreateWindowEx(0, L"TaskManagerWindowClass", L"Task Manager", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                          CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, hwndParent, NULL, wc.hInstance, NULL);
    if (!hwndTaskManager) return;

    HWND hwndListView = CreateWindowEx(0, WC_LISTVIEW, L"", WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL,
                                       10, 10, 460, 250, hwndTaskManager, (HMENU)1, wc.hInstance, NULL);
    if (!hwndListView) return;

    ListView_SetExtendedListViewStyle(hwndListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    LVCOLUMN lvColumn = {0};
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
    lvColumn.pszText = L"Process Name";
    lvColumn.cx = 250;
    ListView_InsertColumn(hwndListView, 0, &lvColumn);

    lvColumn.pszText = L"Process ID";
    lvColumn.cx = 100;
    ListView_InsertColumn(hwndListView, 1, &lvColumn);

    lvColumn.pszText = L"Memory Usage";
    lvColumn.cx = 100;
    ListView_InsertColumn(hwndListView, 2, &lvColumn);

    // Add CPU Usage Column
    lvColumn.pszText = L"CPU Time (ms)";
    lvColumn.cx = 100;
    ListView_InsertColumn(hwndListView, 3, &lvColumn);
    
    CreateWindowEx(0, L"BUTTON", L"Refresh", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                   10, 270, 100, 30, hwndTaskManager, (HMENU)2, wc.hInstance, NULL);

    CreateWindowEx(0, L"BUTTON", L"Terminate", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                   120, 270, 100, 30, hwndTaskManager, (HMENU)3, wc.hInstance, NULL);
    
    ListRunningProcesses(hwndListView);
}


void ListRunningProcesses(HWND hwndListView) {
    ListView_DeleteAllItems(hwndListView);

    LVITEM lvItem = {0};
    lvItem.mask = LVIF_TEXT;

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    if (EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        cProcesses = cbNeeded / sizeof(DWORD);
        for (unsigned int i = 0; i < cProcesses; i++) {
            DWORD processID = aProcesses[i];
            if (processID == 0) continue;

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
            if (hProcess) {
                TCHAR szProcessName[MAX_PATH] = TEXT("Unknown");
                HMODULE hMod;
                DWORD cbNeeded;
                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                    GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
                }

                // Insert Process Name
                lvItem.pszText = szProcessName;
                lvItem.iItem = i;
                int rowIndex = ListView_InsertItem(hwndListView, &lvItem);

                // Insert Process ID
                wchar_t processIdStr[32];
                swprintf(processIdStr, 32, L"%lu", processID);
                ListView_SetItemText(hwndListView, rowIndex, 1, processIdStr);

                // Fetch and Insert Memory Usage
                PROCESS_MEMORY_COUNTERS pmc;
                wchar_t memoryUsageStr[32];
                if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                    swprintf(memoryUsageStr, 32, L"%lu KB", pmc.WorkingSetSize / 1024);
                } else {
                    wcscpy(memoryUsageStr, L"N/A");
                }
                ListView_SetItemText(hwndListView, rowIndex, 2, memoryUsageStr);

                // Fetch and Insert CPU Usage
                FILETIME ftCreation, ftExit, ftKernel, ftUser;
                wchar_t cpuUsageStr[32] = L"N/A";
                if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
                    ULARGE_INTEGER kernelTime, userTime;
                    kernelTime.LowPart = ftKernel.dwLowDateTime;
                    kernelTime.HighPart = ftKernel.dwHighDateTime;
                    userTime.LowPart = ftUser.dwLowDateTime;
                    userTime.HighPart = ftUser.dwHighDateTime;

                    ULONGLONG totalTime = (kernelTime.QuadPart + userTime.QuadPart) / 10000;
                    swprintf(cpuUsageStr, 32, L"%llu ms", totalTime);
                }
                ListView_SetItemText(hwndListView, rowIndex, 3, cpuUsageStr);

                CloseHandle(hProcess);
            }
        }
    }
}


void TerminateProcessByPid(HWND hwnd, DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess) {
        if (TerminateProcess(hProcess, 0)) {
            wchar_t successStr[256];
            swprintf(successStr, sizeof(successStr) / sizeof(wchar_t), L"Successfully terminated process %lu", processId);
            MessageBox(hwnd, successStr, L"Success", MB_OK | MB_ICONINFORMATION);
        } else {
            wchar_t errorStr[256];
            swprintf(errorStr, sizeof(errorStr) / sizeof(wchar_t), L"Failed to terminate process %lu", processId);
            MessageBox(hwnd, errorStr, L"Error", MB_OK | MB_ICONERROR);
        }
        CloseHandle(hProcess);
    } else {
        MessageBox(hwnd, L"Failed to open process for termination.", L"Error", MB_OK | MB_ICONERROR);
    }
}
