#include "Menu.h"
#include <iostream>

// Constructor: initializes the menu with a reference to ProcessManager
Menu::Menu(ProcessManager& pm) : processManager(pm) {}

// Prints the menu options to the console
void Menu::printMenu()
{
    std::cout << "Choose sorting method:\n";
    std::cout << "1. By Name (Alphabetical)\n";
    std::cout << "2. By Memory Size\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter choice: ";
}

// Runs the main menu loop until the user chooses to exit
void Menu::runMenu()
{
    int choice = -1; // Store user choice

    // Continue until user enters 0 to exit
    while (choice != 0)
    {
        printMenu();       // Show the menu
        std::cin >> choice; // Get user input

        switch (choice)
        {
        case 1:
            // Sort processes by name and print them grouped by name
            processManager.sortByName();
            processManager.printGroupedProcessesByName();
            break;
        case 2:
            // Sort processes by memory size and print them grouped by memory
            processManager.sortByMemory();
            processManager.printGroupedProcessesByMemory();
            break;
        case 0:
            // Exit message before quitting
            std::cout << "Goodbye!\n";
            break;
        default:
            // Handle invalid inputs 
            std::cout << "Invalid choice!\n";
            break;
        }
    }
}
