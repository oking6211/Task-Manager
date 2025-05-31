#include "ProcessLauncher.h"
#include <windows.h>

bool ProcessLauncher::launch(const std::wstring& programPath)
{
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;

    BOOL result = CreateProcessW(
        nullptr,
        const_cast<wchar_t*>(programPath.c_str()),
        nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);

    if (result) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }
    return false;
}
