#include "Menu.h"
#include <iostream>

Menu::Menu(ProcessManager& pm) : processManager(pm) {}

void Menu::printMenu() 
{
    std::cout << "Choose sorting method:\n";
    std::cout << "1. By Name (Alphabetical)\n";
    std::cout << "2. By Memory Size\n";
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

        switch (choice) 
        {
        case 1:
            processManager.sortByName();
            processManager.printProcessList();
            break;
        case 2:
            processManager.sortByMemory();
            processManager.printProcessList();
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
