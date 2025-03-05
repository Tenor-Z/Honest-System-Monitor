#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <windows.h>

// Function to create the task manager window and list running processes
void CreateTaskManagerWindow(HWND hwnd);

// Function to terminate a process by its PID
void TerminateProcessByPid(HWND hwnd, DWORD processId);

// Function to refresh the process list
void RefreshProcessList(HWND hwndList);

#endif // TASK_MANAGER_H
