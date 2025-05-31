#include "Utils.h"
#include <sstream>
#include <iomanip>

// Convert memory size in bytes to a human-readable string (MB or GB)
// Shows memory usage with 2 decimal places, appends appropriate unit
std::wstring formatMemory(size_t memoryUsage)
{
    std::wstringstream stream;
    // Convert bytes to megabytes
    double memoryMB = static_cast<double>(memoryUsage) / (1024.0 * 1024.0);

    if (memoryMB >= 1024.0) 
    {
        // If larger than 1 GB, convert to GB and format output
        stream << std::fixed << std::setprecision(2) << (memoryMB / 1024.0) << L" GB";
    }
    else 
    {
        // Otherwise, format output in MB
        stream << std::fixed << std::setprecision(2) << memoryMB << L" MB";
    }

    return stream.str();
}
