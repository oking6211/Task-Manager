#pragma once

#include "ProcessManager.h"
#include "ProcessLauncher.h"
#include <chrono>
#include <thread>
#include <conio.h> 
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

    //Function for launching
    void launchProcess();

    //Dunction for terminating processes
    void terminateProcessByName();

    //function for live monitoring
    void liveMonitor();

    //function for printing procsses in an orgenized manner
    void printGroupedProcessesLive(const std::vector<ProcessInfo>& processList,
        std::map<std::wstring, size_t>& previousMemory);

    //function to serch for procsses by name
    void searchProcessesByName();

    //Reference to ProcessLauncher instance for launching a new process
    ProcessLauncher processLauncher;

    // Reference to ProcessManager instance for accessing and managing processes
    ProcessManager& processManager;
};
