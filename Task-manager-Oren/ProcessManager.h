#pragma once
#define NOMINMAX
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cwctype>
#include <vector>
#include <map>
#include <sstream>
#include "Utils.h"
#pragma comment(lib, "psapi.lib")

struct ProcessInfo
{
    DWORD pid;
    std::wstring name;
    unsigned long long memoryUsage;
    bool isAccessible;
};

struct ProcessGroup {
    int count = 0;
    size_t totalMemory = 0;
};

class ProcessManager
{
public:
    bool refreshProcessList();
    const std::vector<ProcessInfo>& getProcessList() const;
    void printProcessList() const;

    void sortByName();
    void sortByMemory();

    void printGroupedProcessesByMemory() const;
    void printGroupedProcessesByName() const;

private:
    std::vector<ProcessInfo> processList;
    size_t getLongestNameLength() const;
    std::wstring cleanName(const std::wstring& name) const;
};
