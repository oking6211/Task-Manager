#include "Utils.h"
#include <sstream>
#include <iomanip>

std::wstring formatMemory(size_t memoryUsage) {
    std::wstringstream stream;
    double memoryMB = static_cast<double>(memoryUsage) / (1024.0 * 1024.0);

    if (memoryMB >= 1024.0) {
        stream << std::fixed << std::setprecision(2) << (memoryMB / 1024.0) << L" GB";
    }
    else {
        stream << std::fixed << std::setprecision(2) << memoryMB << L" MB";
    }

    return stream.str();
}