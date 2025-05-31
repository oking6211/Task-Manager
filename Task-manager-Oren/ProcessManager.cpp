#include "ProcessManager.h"
#include "Utils.h" // For formatMemory function

// Refresh the list of currently running processes on the system
bool ProcessManager::refreshProcessList()
{
    processList.clear();  // Clear any existing processes from the list

    // Take a snapshot of all running processes
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(PROCESSENTRY32W);

    // Get the first process from the snapshot
    if (!Process32FirstW(snapshot, &entry))
    {
        CloseHandle(snapshot);
        return false;
    }

    do
    {
        ProcessInfo pinfo;
        pinfo.pid = entry.th32ProcessID;
        pinfo.name = entry.szExeFile; // Process executable name
        pinfo.isAccessible = true;    // Assume accessible at first
        pinfo.memoryUsage = 0;

        // Try to open the process for querying memory info
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pinfo.pid);
        if (hProcess)
        {
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
            {
                pinfo.memoryUsage = pmc.WorkingSetSize;  // Store current memory usage
            }
            CloseHandle(hProcess);
        }
        else
        {
            pinfo.isAccessible = false; // Mark as inaccessible if cannot open
        }

        processList.push_back(pinfo); // Add this process info to the list
    } while (Process32NextW(snapshot, &entry)); // Continue through the snapshot

    CloseHandle(snapshot);
    return true; // Successfully refreshed process list
}

// Get the current list of processes (read-only)
const std::vector<ProcessInfo>& ProcessManager::getProcessList() const
{
    return processList;
}

// Sort processes alphabetically by name (case-insensitive), inaccessible processes last
void ProcessManager::sortByName()
{
    std::sort(processList.begin(), processList.end(), [this](const ProcessInfo& a, const ProcessInfo& b)
        {
            // Ensure inaccessible processes go last
            if (!a.isAccessible && b.isAccessible) return false;
            if (a.isAccessible && !b.isAccessible) return true;

            // Clean names by removing ".exe" and convert to lowercase for comparison
            std::wstring aClean = cleanName(a.name);
            std::wstring bClean = cleanName(b.name);
            std::transform(aClean.begin(), aClean.end(), aClean.begin(), towlower);
            std::transform(bClean.begin(), bClean.end(), bClean.begin(), towlower);

            return aClean < bClean;
        });
}

// Sort processes by memory usage descending, inaccessible processes last
void ProcessManager::sortByMemory()
{
    std::sort(processList.begin(), processList.end(), [](const ProcessInfo& a, const ProcessInfo& b)
        {
            // Inaccessible processes are always last
            if (!a.isAccessible && b.isAccessible) return false;
            if (a.isAccessible && !b.isAccessible) return true;

            return a.memoryUsage > b.memoryUsage;
        });
}

// Remove ".exe" extension for cleaner display
std::wstring ProcessManager::cleanName(const std::wstring& name) const
{
    size_t pos = name.find(L".exe");
    if (pos != std::wstring::npos)
    {
        return name.substr(0, pos);
    }
    return name;
}

void ProcessManager::printProcessList(const std::vector<ProcessInfo>& list) const
{
    size_t nameWidth = getLongestNameLength() + 5;

    std::wcout << std::left << std::setw(10) << L"PID"
        << std::setw(nameWidth) << L"Name"
        << std::setw(15) << L"Memory"
        << L"\n";

    std::wcout << std::wstring(10 + nameWidth + 15, L'-') << L"\n";

    for (const auto& proc : list)
    {
        std::wcout << std::left << std::setw(10) << proc.pid
            << std::setw(nameWidth) << cleanName(proc.name).c_str();

        if (proc.isAccessible)
            std::wcout << std::setw(15) << formatMemory(proc.memoryUsage).c_str();
        else
            std::wcout << std::setw(15) << L"Access Denied";

        std::wcout << L"\n";
    }
}

// Calculate longest process name length (for formatting output)
size_t ProcessManager::getLongestNameLength() const
{
    size_t maxLength = 0;
    for (const auto& proc : processList)
    {
        if (proc.isAccessible)
        {
            size_t len = cleanName(proc.name).length();
            if (len > maxLength)
                maxLength = len;
        }
    }
    return maxLength;
}

// Print detailed list of processes (PID, Name, Memory usage)
void ProcessManager::printProcessList() const
{
    size_t nameWidth = getLongestNameLength() + 5;

    // Print headers with alignment
    std::wcout << std::left << std::setw(10) << L"PID"
        << std::setw(nameWidth) << L"Name"
        << std::setw(15) << L"Memory"
        << L"\n";

    // Print separator line
    std::wcout << std::wstring(10 + nameWidth + 15, L'-') << L"\n";

    // Print each process info, handling inaccessible processes
    for (const auto& proc : processList)
    {
        std::wcout << std::left << std::setw(10) << proc.pid
            << std::setw(nameWidth) << cleanName(proc.name).c_str();

        if (proc.isAccessible)
        {
            std::wcout << std::setw(15) << formatMemory(proc.memoryUsage).c_str();
        }
        else
        {
            std::wcout << std::setw(15) << L"Access Denied";
        }

        std::wcout << L"\n";
    }
}

// Group processes by name, sum memory, then print sorted by total memory descending
void ProcessManager::printGroupedProcessesByMemory() const
{
    std::map<std::wstring, ProcessGroup> grouped;

    // Aggregate counts and memory per cleaned process name
    for (const auto& p : processList)
    {
        if (p.isAccessible)
        {
            std::wstring name = cleanName(p.name);
            grouped[name].count++;
            grouped[name].totalMemory += static_cast<unsigned long long>(p.memoryUsage);
        }
    }

    // Copy to vector for sorting by total memory usage
    std::vector<std::pair<std::wstring, ProcessGroup>> vecGrouped(grouped.begin(), grouped.end());

    // Sort descending by total memory
    std::sort(vecGrouped.begin(), vecGrouped.end(),
        [](const auto& a, const auto& b)
        {
            return a.second.totalMemory > b.second.totalMemory;
        });

    // Find max name length for formatting
    size_t maxNameLength = 0;
    for (const auto& entry : vecGrouped)
    {
        maxNameLength = std::max(maxNameLength, entry.first.length());
    }

    // Print header
    std::wcout << std::left
        << std::setw(static_cast<int>(maxNameLength) + 4) << L"Process Name"
        << std::setw(12) << L"Instances"
        << L"Total Memory\n";

    std::wcout << std::wstring(maxNameLength + 32, L'-') << L"\n";

    // Print each grouped entry
    for (const auto& entry : vecGrouped)
    {
        std::wcout << std::left
            << std::setw(static_cast<int>(maxNameLength) + 4) << entry.first
            << std::setw(12) << entry.second.count
            << formatMemory(entry.second.totalMemory) << L"\n";
    }
}

// Group processes by name (case-insensitive), sum memory, then print sorted alphabetically ignoring case
void ProcessManager::printGroupedProcessesByName() const
{
    // Use case-insensitive comparator for keys
    std::map<std::wstring, ProcessGroup, CaseInsensitiveCompare> grouped;

    // Aggregate counts and memory per cleaned process name
    for (const auto& p : processList)
    {
        if (p.isAccessible)
        {
            std::wstring name = cleanName(p.name);
            grouped[name].count++;
            grouped[name].totalMemory += p.memoryUsage;
        }
    }

    // Find max name length for formatting
    size_t maxNameLength = 0;
    for (const auto& entry : grouped)
    {
        maxNameLength = std::max(maxNameLength, entry.first.length());
    }

    // Print header
    std::wcout << std::left
        << std::setw(static_cast<int>(maxNameLength) + 4) << L"Process Name"
        << std::setw(12) << L"Instances"
        << L"Total Memory\n";

    std::wcout << std::wstring(maxNameLength + 32, L'-') << L"\n";

    // Print each grouped entry
    for (const auto& entry : grouped)
    {
        std::wcout << std::left
            << std::setw(static_cast<int>(maxNameLength) + 4) << entry.first
            << std::setw(12) << entry.second.count
            << formatMemory(entry.second.totalMemory) << L"\n";
    }
}

std::vector<ProcessInfo> ProcessManager::getProcessesByName(const std::wstring& name) const
{
    std::wstring lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), towlower);

    std::vector<ProcessInfo> result;
    for (const auto& proc : processList)
    {
        std::wstring cleanProcName = cleanName(proc.name);
        std::wstring lowerProcName = cleanProcName;
        std::transform(lowerProcName.begin(), lowerProcName.end(), lowerProcName.begin(), towlower);

        if (lowerProcName == lowerName)
        {
            result.push_back(proc);
        }
    }
    return result;
}

bool ProcessManager::terminateProcessByPID(DWORD pid)
{
    // Open process with terminate rights
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL)
        return false;

    // Attempt to terminate process
    bool success = TerminateProcess(hProcess, 0) != 0;

    CloseHandle(hProcess);
    return success;
}

bool ProcessManager::terminateProcessesByName(const std::wstring& targetName)
{
    bool allTerminated = true;

    // Normalize the target name: lowercase and strip .exe
    std::wstring normalizedTarget = targetName;
    std::transform(normalizedTarget.begin(), normalizedTarget.end(), normalizedTarget.begin(), ::towlower);

    // Refresh process list before attempting termination
    refreshProcessList();

    for (const auto& proc : processList)
    {
        if (!proc.isAccessible)
            continue;

        std::wstring procName = cleanName(proc.name);  // Remove .exe and lowercase
        std::transform(procName.begin(), procName.end(), procName.begin(), ::towlower);

        if (procName == normalizedTarget)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, proc.pid);
            if (hProcess)
            {
                if (!TerminateProcess(hProcess, 0))
                {
                    std::wcout << L"Failed to terminate process PID: " << proc.pid
                        << L" (Error code: " << GetLastError() << L")\n";
                    allTerminated = false;
                }
                else
                {
                    std::wcout << L"Terminated process PID: " << proc.pid << L"\n";
                }
                CloseHandle(hProcess);
            }
            else
            {
                std::wcout << L"Cannot open process PID: " << proc.pid
                    << L" (Error code: " << GetLastError() << L")\n";
                allTerminated = false;
            }
        }
    }

    return allTerminated;
}
