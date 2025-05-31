#include "ProcessManager.h"
#include "Menu.h"
#include <iostream>

// Entry point of the program
int main()
{
    // Create an instance of ProcessManager to handle process data
    ProcessManager pm;

    // Try to refresh the list of currently running processes
    if (!pm.refreshProcessList())
    {
        // If refreshing fails, print an error and exit with failure code
        std::cerr << "Failed to refresh process list.\n";
        return 1;
    }

    // Create a menu interface and pass the ProcessManager instance to it
    Menu menu(pm);

    // Start the menu loop to interact with the user
    menu.runMenu();

    // Program finished successfully
    return 0;
}
