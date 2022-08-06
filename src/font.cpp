#include "font.h"
#include <iostream>

#ifdef _WIN32
    #include <windows.h>

    std::ostream& operator<<(std::ostream& os, const Fonts::Font& font)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        
        SetConsoleTextAttribute(hConsole, font.color);

        return os;
    }
#else 
    std::ostream& operator<<(std::ostream& os, const Fonts::Font& font)
    {
        return os << "\033[" << font.weight << ';' << font.color << 'm';
    }
#endif