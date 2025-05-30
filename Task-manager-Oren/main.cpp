#include "ProcessManager.h"
#include "Menu.h"
#include <iostream>

int main() 
{
    ProcessManager pm;

    if (!pm.refreshProcessList()) 
    {
        std::cerr << "Failed to refresh process list.\n";
        return 1;
    }

    Menu menu(pm);
    menu.runMenu();

    return 0;
}
