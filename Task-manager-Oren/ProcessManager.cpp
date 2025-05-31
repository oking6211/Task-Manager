#include "ProcessManager.h"

bool ProcessManager::refreshProcessList() {
    processList.clear();

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return false;

    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(snapshot, &entry)) {
        CloseHandle(snapshot);
        return false;
    }

    do {
        ProcessInfo pinfo;
        pinfo.pid = entry.th32ProcessID;
        pinfo.name = entry.szExeFile;
        pinfo.isAccessible = true;
        pinfo.memoryUsage = 0;

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pinfo.pid);
        if (hProcess) {
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                pinfo.memoryUsage = pmc.WorkingSetSize;
            }
            CloseHandle(hProcess);
        }
        else {
            pinfo.isAccessible = false;
        }

        processList.push_back(pinfo);
    } while (Process32NextW(snapshot, &entry));

    CloseHandle(snapshot);
    return true;
}

const std::vector<ProcessInfo>& ProcessManager::getProcessList() const {
    return processList;
}

void ProcessManager::sortByName() {
    std::sort(processList.begin(), processList.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        if (!a.isAccessible && b.isAccessible) return false;
        if (a.isAccessible && !b.isAccessible) return true;
        return a.name < b.name;
        });
}

void ProcessManager::sortByMemory() {
    std::sort(processList.begin(), processList.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        if (!a.isAccessible && b.isAccessible) return false;
        if (a.isAccessible && !b.isAccessible) return true;
        return a.memoryUsage > b.memoryUsage;
        });
}

std::wstring ProcessManager::cleanName(const std::wstring& name) const {
    size_t pos = name.find(L".exe");
    if (pos != std::wstring::npos) {
        return name.substr(0, pos);
    }
    return name;
}

size_t ProcessManager::getLongestNameLength() const {
    size_t maxLength = 0;
    for (const auto& proc : processList) {
        if (proc.isAccessible) {
            size_t len = cleanName(proc.name).length();
            if (len > maxLength)
                maxLength = len;
        }
    }
    return maxLength;
}

void ProcessManager::printProcessList() const {
    size_t nameWidth = getLongestNameLength() + 5;

    std::wcout << std::left << std::setw(10) << L"PID"
        << std::setw(nameWidth) << L"Name"
        << std::setw(15) << L"Memory"
        << L"\n";

    std::wcout << std::wstring(10 + nameWidth + 15, L'-') << L"\n";

    for (const auto& proc : processList) {
        std::wcout << std::left << std::setw(10) << proc.pid
            << std::setw(nameWidth) << cleanName(proc.name).c_str();

        if (proc.isAccessible) {
            std::wcout << std::setw(15) << formatMemory(proc.memoryUsage).c_str();
        }
        else {
            std::wcout << std::setw(15) << L"Access Denied";
        }

        std::wcout << L"\n";
    }
}

void ProcessManager::printGroupedProcessesByMemory() const {
    std::map<std::wstring, ProcessGroup> grouped;
    for (const auto& p : processList) {
        if (p.isAccessible) {
            std::wstring name = cleanName(p.name);
            grouped[name].count++;
            grouped[name].totalMemory += static_cast<unsigned long long>(p.memoryUsage);
        }
    }

    
    std::vector<std::pair<std::wstring, ProcessGroup>> vecGrouped(grouped.begin(), grouped.end());

    
    std::sort(vecGrouped.begin(), vecGrouped.end(),
        [](const auto& a, const auto& b) {
            return a.second.totalMemory > b.second.totalMemory;
        });

    
    size_t maxNameLength = 0;
    for (const auto& entry : vecGrouped) {
        maxNameLength = std::max(maxNameLength, entry.first.length());
    }

    std::wcout << std::left
        << std::setw(static_cast<int>(maxNameLength) + 4) << L"Process Name"
        << std::setw(12) << L"Instances"
        << L"Total Memory\n";

    std::wcout << std::wstring(maxNameLength + 32, L'-') << L"\n";

    for (const auto& entry : vecGrouped) {
        std::wcout << std::left
            << std::setw(static_cast<int>(maxNameLength) + 4) << entry.first
            << std::setw(12) << entry.second.count
            << formatMemory(entry.second.totalMemory) << L"\n";
    }
}

void ProcessManager::printGroupedProcessesByName() const {
    std::map<std::wstring, ProcessGroup> grouped;

    
    for (const auto& p : processList) {
        if (p.isAccessible) {
            std::wstring name = cleanName(p.name);
            grouped[name].count++;
            grouped[name].totalMemory += p.memoryUsage;
        }
    }

    
    size_t maxNameLength = 0;
    for (const auto& entry : grouped) {
        maxNameLength = std::max(maxNameLength, entry.first.length());
    }

    
    std::wcout << std::left
        << std::setw(static_cast<int>(maxNameLength) + 4) << L"Process Name"
        << std::setw(12) << L"Instances"
        << L"Total Memory\n";

    std::wcout << std::wstring(maxNameLength + 32, L'-') << L"\n";

    
    for (const auto& entry : grouped) {
        std::wcout << std::left
            << std::setw(static_cast<int>(maxNameLength) + 4) << entry.first
            << std::setw(12) << entry.second.count
            << formatMemory(entry.second.totalMemory) << L"\n";
    }
}

