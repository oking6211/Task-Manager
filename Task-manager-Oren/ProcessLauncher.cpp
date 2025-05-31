#include "ProcessLauncher.h"
#include <windows.h>

bool ProcessLauncher::launch(const std::wstring& programPath)
{
    // Use ShellExecuteW to open the program
    HINSTANCE result = ShellExecuteW(NULL, L"open", programPath.c_str(), NULL, NULL, SW_SHOW);
    // If result > 32, launch succeeded
    return (INT_PTR)result > 32;
}
