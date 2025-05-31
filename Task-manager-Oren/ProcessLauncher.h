#pragma once
#include <string>

class ProcessLauncher
{
public:
    // Launches a program given its name or full path
    // Returns true if successful, false otherwise
    bool launch(const std::wstring& programPath);
};
