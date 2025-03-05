#ifndef WARNINGS_H
#define WARNINGS_H

#include <stdlib.h>
#include <time.h>

// CPU Warning Messages
extern const char* cpuWarningMessages[];
extern const int cpuWarningMessageCount;

// RAM Warning Messages
extern const char* ramWarningMessages[];
extern const int ramWarningMessageCount;

// Function Declarations
const char* GetRandomCpuWarningMessage();
const char* GetRandomRamWarningMessage();

#endif // WARNINGS_H
