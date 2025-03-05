#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <windows.h>

void CreateTaskManagerWindow(HWND hwnd);

void TerminateProcessByPid(HWND hwnd, DWORD processId);

void RefreshProcessList(HWND hwndList);

#endif // TASK_MANAGER_H
