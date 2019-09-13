#pragma once
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#define XEYTAN_WINDOWS
#elif defined(__unix__) || defined(__unix) || defined(__linux__)
#define XEYTAN_LINUX
#elif defined(__APPLE__) || defined(__MACH__)
#define XEYTAN_MAC
#else
#error Unknown Platform
#endif