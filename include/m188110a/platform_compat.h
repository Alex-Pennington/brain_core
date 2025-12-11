// platform_compat.h - Cross-platform compatibility shims
// For Paul Brain modem core

#ifndef PLATFORM_COMPAT_H
#define PLATFORM_COMPAT_H

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
    #include <windows.h>
    #include <winbase.h>
#else
    // Linux/Unix compatibility
    #include <unistd.h>
    
    // Sleep in milliseconds
    inline void Sleep(unsigned int ms) {
        usleep(ms * 1000);
    }
    
    // Stub for any other Windows types if needed
    typedef unsigned long DWORD;
    
    // sprintf_s shim (just use sprintf on Linux)
    #define sprintf_s(buf, ...) sprintf(buf, __VA_ARGS__)
    
    // gmtime_s shim - Windows has (result, time), POSIX has (time, result)
    inline int gmtime_s(struct tm* result, const time_t* time) {
        struct tm* tmp = gmtime_r(time, result);
        return (tmp == nullptr) ? -1 : 0;
    }
#endif

#endif // PLATFORM_COMPAT_H
