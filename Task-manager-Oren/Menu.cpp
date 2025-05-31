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