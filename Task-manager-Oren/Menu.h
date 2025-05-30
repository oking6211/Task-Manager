#pragma once

#include "ProcessManager.h"

// Class to handle the user interface menu
class Menu
{
public:
    // Constructor takes a reference to the ProcessManager to interact with processes
    Menu(ProcessManager& pm);

    // Main loop to run the menu until user exits
    void runMenu();

private:
    // Prints the menu options to the console
    void printMenu();

    // Reference to ProcessManager instance for accessing and managing processes
    ProcessManager& processManager;
};
