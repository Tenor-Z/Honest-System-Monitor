#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#include <tchar.h>
#include <stdlib.h>
#include <commctrl.h> // Required for tooltips
#include <time.h>

#include "warnings.h"
#include "taskmanager.h"

#define REFRESH_RATE 5000  // Auto-refresh every 5 seconds
#define HIGH_CPU_THRESHOLD 80
#define HIGH_RAM_THRESHOLD 80

HFONT hFont;
HWND hCpuLabel, hRamLabel, hUptimeLabel, hRefreshButton;
HWND hCreditsButton;
HWND hTaskManagerButton;


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateSystemStats(HWND hwnd);
void ShowWarningMessage(const char *message);
void CreateControls(HWND hwnd);
void GetUptimeString(wchar_t *uptimeText, size_t size);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    InitCommonControls();
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("HonestMonitor");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, _T("Window Class Registration Failed!"), _T("Error"), MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindow(_T("HonestMonitor"), _T("Honest System Monitor"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL, hInstance, NULL);
    if (!hwnd) {
        MessageBox(NULL, _T("Window Creation Failed!"), _T("Error"), MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd); // Ensures the window is immediately updated
    CreateControls(hwnd);

    // Seed the random number generator ONCE at startup
    srand((unsigned int)time(NULL));

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            SetTimer(hwnd, 1, REFRESH_RATE, NULL); // Start the refresh timer
            break;
        
        case WM_TIMER:
            UpdateSystemStats(hwnd); // Update system stats on timer
            break;
        
        case WM_COMMAND:
            if ((HWND)lParam == hRefreshButton) {
                UpdateSystemStats(hwnd); // Manual refresh on button click
            }
            else if ((HWND)lParam == hCreditsButton) {  // Handle Credits button click
            MessageBoxW(hwnd, 
                L"Honest System Monitor v1.0\n"
                L"Created by Tyler Bifolchi (Tenor-Z)\n"
                L"Built with MinGW, WinAPI, and Visual Studio Code\n",
                L"Credits", MB_OK | MB_ICONINFORMATION);
            }
            else if ((HWND)lParam == hTaskManagerButton) {  // Open task manager window
                CreateTaskManagerWindow(hwnd);
            }
            break;
        
        case WM_DESTROY:
            KillTimer(hwnd, 1); // Clean up the timer
            PostQuitMessage(0); // Post quit message to exit the application
            DeleteObject(hFont);
            break;
        
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void CreateControls(HWND hwnd) {
    // Create the custom font for labels
    hFont = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Segoe UI"));
    
    // Create CPU usage label
    hCpuLabel = CreateWindowExW(0, L"STATIC", L"CPU Usage: Loading...", WS_VISIBLE | WS_CHILD, 20, 20, 300, 30, hwnd, NULL, NULL, NULL);
    SendMessageW(hCpuLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Create RAM usage label
    hRamLabel = CreateWindowExW(0, L"STATIC", L"RAM Usage: Loading...", WS_VISIBLE | WS_CHILD, 20, 60, 300, 30, hwnd, NULL, NULL, NULL);
    SendMessageW(hRamLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Create Uptime label
    hUptimeLabel = CreateWindowExW(0, L"STATIC", L"Uptime: Loading...", WS_VISIBLE | WS_CHILD, 20, 100, 300, 30, hwnd, NULL, NULL, NULL);
    SendMessageW(hUptimeLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Create Refresh button
    hRefreshButton = CreateWindowExW(0, L"BUTTON", L"Refresh", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 20, 150, 100, 30, hwnd, (HMENU)1, NULL, NULL);
    SendMessageW(hRefreshButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Create Task Manager button
    hTaskManagerButton = CreateWindowExW(0, L"BUTTON", L"Task Manager", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 260, 150, 100, 30, hwnd, (HMENU)4, NULL, NULL);
    SendMessageW(hTaskManagerButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Create Credits button 
    hCreditsButton = CreateWindowExW(0, L"BUTTON", L"Credits", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 140, 150, 100, 30, hwnd, (HMENU)2, NULL, NULL);
    SendMessageW(hCreditsButton, WM_SETFONT, (WPARAM)hFont, TRUE);

    TOOLINFOW toolInfo = {0};  // Use TOOLINFOW for wide strings
    toolInfo.cbSize = sizeof(TOOLINFOW);   // Specify size of TOOLINFOW structure
    toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;  // Tooltip flags
    toolInfo.hwnd = hwnd;   // Parent window handle
    toolInfo.uId = (UINT_PTR)hRefreshButton;   // Tool's identifier (button handle)
    toolInfo.lpszText = (LPWSTR)L"Click to refresh system stats"; 

    HWND hwndToolTip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, NULL, NULL);
    SendMessage(hwndToolTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
}



void UpdateSystemStats(HWND hwnd) {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    double ramUsage = memInfo.dwMemoryLoad; // By percentage

    FILETIME idleTime, kernelTime, userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);
    Sleep(100);
    FILETIME idleTime2, kernelTime2, userTime2;
    GetSystemTimes(&idleTime2, &kernelTime2, &userTime2);

    ULARGE_INTEGER idle, kernel, user;
    idle.LowPart = idleTime.dwLowDateTime;
    idle.HighPart = idleTime.dwHighDateTime;
    kernel.LowPart = kernelTime.dwLowDateTime;
    kernel.HighPart = kernelTime.dwHighDateTime;
    user.LowPart = userTime.dwLowDateTime;
    user.HighPart = userTime.dwHighDateTime;

    ULARGE_INTEGER idle2, kernel2, user2;
    idle2.LowPart = idleTime2.dwLowDateTime;
    idle2.HighPart = idleTime2.dwHighDateTime;
    kernel2.LowPart = kernelTime2.dwLowDateTime;
    kernel2.HighPart = kernelTime2.dwHighDateTime;
    user2.LowPart = userTime2.dwLowDateTime;
    user2.HighPart = userTime2.dwHighDateTime;

    ULONGLONG idleDiff = idle2.QuadPart - idle.QuadPart;
    ULONGLONG kernelDiff = kernel2.QuadPart - kernel.QuadPart;
    ULONGLONG userDiff = user2.QuadPart - user.QuadPart;
    double cpuUsage = (double)(kernelDiff + userDiff - idleDiff) / (kernelDiff + userDiff) * 100.0;

    wchar_t cpuText[50], ramText[50], uptimeText[50];
    swprintf(cpuText, 50, L"CPU Usage: %.2f%%", cpuUsage);
    swprintf(ramText, 50, L"RAM Usage: %.2f%%", ramUsage);
    GetUptimeString(uptimeText, 50);

    SetWindowTextW(hCpuLabel, cpuText);
    SetWindowTextW(hRamLabel, ramText);
    SetWindowTextW(hUptimeLabel, uptimeText);

    if (cpuUsage > HIGH_CPU_THRESHOLD) {
        ShowWarningMessage(GetRandomCpuWarningMessage());
    }
    if (ramUsage > HIGH_RAM_THRESHOLD) {
        ShowWarningMessage(GetRandomRamWarningMessage());
    }
}


void GetUptimeString(wchar_t *uptimeText, size_t size) {
    ULONGLONG uptimeMilliseconds = GetTickCount64();
    DWORD uptimeSeconds = (DWORD)(uptimeMilliseconds / 1000);
    DWORD uptimeMinutes = uptimeSeconds / 60;
    DWORD uptimeHours = uptimeMinutes / 60;
    DWORD uptimeDays = uptimeHours / 24;

    swprintf(uptimeText, size, L"Uptime: %d days, %d hours, %d minutes", uptimeDays, uptimeHours % 24, uptimeMinutes % 60);
}


void ShowWarningMessage(const char *message) {
    static time_t lastWarningTime = 0;
    time_t currentTime = time(NULL);
    
    if (currentTime - lastWarningTime >= 60) {  // 60 seconds cooldown
        MessageBox(NULL, message, "Honest System Monitor Warning", MB_OK | MB_ICONWARNING);
        lastWarningTime = currentTime;
    }
}
