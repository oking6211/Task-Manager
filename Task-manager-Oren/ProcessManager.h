#pragma once

// Prevents Windows headers from defining conflicting macros like min/max
#define NOMINMAX

// Windows-specific headers for working with processes
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

// Standard C++ headers
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cwctype>
#include <vector>
#include <map>
#include <sstream>

#include "Utils.h"

// Link against the Psapi library (used for memory info)
#pragma comment(lib, "psapi.lib")

// Holds information about a single process
struct ProcessInfo
{
    DWORD pid;                        // Process ID
    std::wstring name;               // Name of the process
    unsigned long long memoryUsage;  // Memory used by the process (in bytes)
    bool isAccessible;               // Can we read its memory info?
};

// Used to sort process names in a case-insensitive way
struct CaseInsensitiveCompare
{
    bool operator()(const std::wstring& a, const std::wstring& b) const
    {
        std::wstring lowerA = a;
        std::wstring lowerB = b;
        // Convert both strings to lowercase for fair comparison
        std::transform(lowerA.begin(), lowerA.end(), lowerA.begin(), towlower);
        std::transform(lowerB.begin(), lowerB.end(), lowerB.begin(), towlower);
        return lowerA < lowerB;
    }
};

// Used for grouping processes by name
struct ProcessGroup 
{
    int count = 0;              // How many instances of the process
    size_t totalMemory = 0;     // Combined memory usage of all instances
};

// Manages the list of processes and handles sorting/printing
class ProcessManager
{
public:
    // Fills the internal process list with current system processes
    bool refreshProcessList();

    // Gives read-only access to the process list
    const std::vector<ProcessInfo>& getProcessList() const;

    // Prints all processes individually
    void printProcessList() const;

    // Sorts the list alphabetically by name (case-insensitive)
    void sortByName();

    // Sorts the list by memory usage, largest first
    void sortByMemory();

    // Groups by process name and prints total memory for each group (sorted by memory)
    void printGroupedProcessesByMemory() const;

    // Groups by process name and prints total memory for each group (sorted by name)
    void printGroupedProcessesByName() const;

private:
    // Stores all the processes currently retrieved from the system
    std::vector<ProcessInfo> processList;

    // Finds the longest process name (used for formatting)
    size_t getLongestNameLength() const;

    // Removes the ".exe" extension from process names
    std::wstring cleanName(const std::wstring& name) const;
};
