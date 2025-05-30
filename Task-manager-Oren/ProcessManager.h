#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cwctype>
#include <vector>

#pragma comment(lib, "psapi.lib")

// Structure to hold info about each process
struct ProcessInfo
{
    DWORD pid;                // Process ID
    std::wstring name;        // Process name (wide string)
    SIZE_T memoryUsage;       // Memory used by the process in bytes
    bool isAccessible;        // True if memory info is accessible, false otherwise
};

// Class responsible for retrieving, storing, sorting and printing process info
class ProcessManager
{
public:
    // Refresh the internal process list by querying the system
    bool refreshProcessList();

    // Get the current list of processes (read-only)
    const std::vector<ProcessInfo>& getProcessList() const;

    // Print the process list to the console in a formatted way
    void printProcessList() const;

    // Sort the process list alphabetically by process name
    void sortByName();

    // Sort the process list by memory usage (descending)
    void sortByMemory();

private:
    std::vector<ProcessInfo> processList;  // Container holding all the process info structs

    // Helper: Find the longest process name length for formatting output
    size_t getLongestNameLength() const;

    // Helper: Clean a process name for sorting (remove non-alphabetic prefix, lowercase)
    std::wstring cleanName(const std::wstring& name);
};
