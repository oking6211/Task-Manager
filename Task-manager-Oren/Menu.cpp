#include "Menu.h"
#include <iostream>
#include <string>

Menu::Menu(ProcessManager& pm) : processManager(pm) {}

void Menu::printMenu()
{
    std::cout << "Choose an option:\n";
    std::cout << "1. Sort by Name (Alphabetical)\n";
    std::cout << "2. Sort by Memory Size\n";
    std::cout << "3. Launch a new program\n";    
    std::cout << "4. Terminate a process by Name\n";
    std::cout << "5. Live Monitoring\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter choice: ";
}

void Menu::runMenu()
{
    int choice = -1;
    while (choice != 0)
    {
        printMenu();
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  

        switch (choice)
        {
        case 1:
            processManager.sortByName();
            processManager.printGroupedProcessesByName();
            break;
        case 2:
            processManager.sortByMemory();
            processManager.printGroupedProcessesByMemory();
            break;
        case 3:
            launchProcess();
            break;
        case 4:
            terminateProcessByName();  
            break;
        case 5:
            liveMonitor();
        case 0:
            std::cout << "Goodbye!\n";
            break;
        default:
            std::cout << "Invalid choice!\n";
            break;
        }
    }
}

void Menu::launchProcess()
{
    std::wstring program;
    std::wcout << L"Enter the program name or full path to launch (e.g. notepad.exe): ";
    std::getline(std::wcin, program);

    if (program.empty())
    {
        std::wcout << L"No input given. Returning to menu.\n";
        return;
    }

    if (processLauncher.launch(program))
    {
        std::wcout << L"Program launched successfully.\n";
        processManager.refreshProcessList();
    }
    else
    {
        std::wcout << L"Failed to launch the program.\n";
    }
}

void Menu::terminateProcessByName()
{
    std::wcout << L"Enter the process name to terminate (without .exe): ";
    std::wstring nameToKill;
    std::getline(std::wcin, nameToKill);

    if (nameToKill.empty())
    {
        std::wcout << L"No name entered. Aborting.\n";
        return;
    }

    if (processManager.terminateProcessesByName(nameToKill)) 
    {
        std::wcout << L"All processes named \"" << nameToKill << L"\" terminated successfully.\n";
    }
    else {
        std::wcout << L"Some processes could not be terminated.\n";
    }
}

void Menu::liveMonitor()
{
    std::map<std::wstring, size_t> previousMemory;

    std::wcout << L"Live monitoring started. Press Ctrl+C to quit.\n";

    while (true)
    {
        processManager.refreshProcessList();
        const std::vector<ProcessInfo>& list = processManager.getProcessList();

        printGroupedProcessesLive(list, previousMemory);

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void Menu::printGroupedProcessesLive(
    const std::vector<ProcessInfo>& processList,
    std::map<std::wstring, size_t>& previousMemory)
{
    std::map<std::wstring, ProcessGroup> currentGroups;

    // Group current processes by cleaned name
    for (size_t i = 0; i < processList.size(); ++i)
    {
        const ProcessInfo& proc = processList[i];
        if (!proc.isAccessible) continue;

        std::wstring name = processManager.cleanName(proc.name);
        currentGroups[name].count++;
        currentGroups[name].totalMemory += proc.memoryUsage;
    }

    // Find max name length
    size_t maxNameLength = 12;
    for (std::map<std::wstring, ProcessGroup>::iterator it = currentGroups.begin(); it != currentGroups.end(); ++it)
    {
        if (it->first.length() > maxNameLength)
            maxNameLength = it->first.length();
    }

    std::wcout << std::left
        << std::setw(static_cast<int>(maxNameLength) + 4) << L"Process Name"
        << std::setw(12) << L"Instances"
        << std::setw(15) << L"Memory"
        << L"Delta\n";
    std::wcout << std::wstring(maxNameLength + 36, L'-') << L"\n";

    // Display each group
    for (std::map<std::wstring, ProcessGroup>::iterator it = currentGroups.begin(); it != currentGroups.end(); ++it)
    {
        const std::wstring& name = it->first;
        const ProcessGroup& group = it->second;

        size_t prevMem = previousMemory.count(name) ? previousMemory[name] : 0;
        size_t currMem = group.totalMemory;

        std::wstring deltaStr;
        if (previousMemory.count(name))
        {
            long long delta = static_cast<long long>(currMem) - static_cast<long long>(prevMem);
            if (delta > 0)
                deltaStr = L"+" + formatMemory(delta);
            else if (delta < 0)
                deltaStr = L"-" + formatMemory(-delta);
            else
                deltaStr = L"0 MB";
        }
        else
        {
            deltaStr = L"N/A";
        }

        previousMemory[name] = currMem;

        std::wcout << std::left
            << std::setw(static_cast<int>(maxNameLength) + 4) << name
            << std::setw(12) << group.count
            << std::setw(15) << formatMemory(currMem)
            << deltaStr << L"\n";
    }
}


