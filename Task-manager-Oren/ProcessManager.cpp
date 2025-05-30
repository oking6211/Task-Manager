#include "ProcessManager.h"


bool ProcessManager::refreshProcessList() 
{
    processList.clear();

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) 
    {
        std::wcerr << L"Failed to take process snapshot.\n";
        return false;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(snapshot, &pe)) 
    {
        CloseHandle(snapshot);
        std::wcerr << L"Failed to get first process.\n";
        return false;
    }

    do {
        ProcessInfo pi;
        pi.pid = pe.th32ProcessID;
        pi.name = pe.szExeFile;
        pi.memoryUsage = 0;
        pi.isAccessible = false;

        HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pi.pid);
        if (!hProc) 
        {
            hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pi.pid);
        }

        if (hProc)
        {
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(hProc, &pmc, sizeof(pmc))) 
            {
                pi.memoryUsage = pmc.WorkingSetSize;
                pi.isAccessible = true;
            }
            CloseHandle(hProc);
        }

        processList.push_back(pi);

    } while (Process32Next(snapshot, &pe));

    CloseHandle(snapshot);
    return true;
}

const std::vector<ProcessInfo>& ProcessManager::getProcessList() const 
{
    return processList;
}

void ProcessManager::printProcessList() const 
{
    const int widthPID = 8;
    const int widthMemory = 15;

    size_t widthName = getLongestNameLength() + 2;  


    std::wcout << std::left
        << std::setw(widthPID) << L"PID"
        << std::setw(widthName) << L"Name"
        << std::setw(widthMemory) << L"Memory Usage"
        << L"\n";

    std::wcout << std::wstring(widthPID + widthName + widthMemory, L'=') << L"\n";

    for (const auto& p : processList) 
    {
        std::wcout << std::left
            << std::setw(widthPID) << p.pid
            << std::setw(widthName) << p.name;

        if (p.isAccessible) {
            double mem = static_cast<double>(p.memoryUsage);
            std::wstring unit = L"Bytes";

            if (mem > 1024) {
                mem /= 1024;
                unit = L"KB";
            }
            if (mem > 1024) {
                mem /= 1024;
                unit = L"MB";
            }
            if (mem > 1024) {
                mem /= 1024;
                unit = L"GB";
            }

            std::wcout << std::fixed << std::setprecision(2)
                << std::setw(widthMemory - 3) << mem << L" " << unit;
        }
        else {
            std::wcout << std::setw(widthMemory) << L"Access Denied";
        }

        std::wcout << L"\n";
    }
}


void ProcessManager::sortByName() {
    std::sort(processList.begin(), processList.end(),
        [this](const ProcessInfo& a, const ProcessInfo& b) {
            if (!a.isAccessible && b.isAccessible) return false;
            if (a.isAccessible && !b.isAccessible) return true;
            if (!a.isAccessible && !b.isAccessible) return false;

            return cleanName(a.name) < cleanName(b.name);
        });
}


void ProcessManager::sortByMemory()
{
    std::sort(processList.begin(), processList.end(), [](const ProcessInfo& a, const ProcessInfo& b)
        {
        return a.memoryUsage > b.memoryUsage; 
        });
}

size_t ProcessManager::getLongestNameLength() const
{
    size_t maxLen = 0;
    for (const auto& p : processList) 
    {
        if (p.isAccessible)
        {
            if (p.name.length() > maxLen)
            {
                maxLen = p.name.length();
            }
        }  
    }
    return maxLen;
}

std::wstring ProcessManager::cleanName(const std::wstring& name) 
{
    size_t start = 0;
    while (start < name.size() && !std::iswalpha(name[start])) 
    {
        ++start;
    }
    std::wstring cleaned = name.substr(start);
    for (auto& ch : cleaned) {
        ch = std::towlower(ch);
    }
    return cleaned;
}
