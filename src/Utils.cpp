#include "Utils.hpp"

#include <windows.h>

std::string Utils::getCurrDir()
{
    char *buf = new char[512];

    GetCurrentDirectoryA(512, buf);

    std::string s(buf);

    delete[] buf;

    return s;
}
