#ifndef GUARD_font_h
#define GUARD_font_h

#include <iostream>
#include <string>

namespace Fonts{ class Font; }    
std::ostream& operator<<(std::ostream&, const Fonts::Font&);

namespace Fonts
{
    class Font
    {
    public:
    #ifdef _WIN32
        enum Code {
            FG_RED         = 4,
            FG_GREEN       = 2,
            FG_BLUE        = 1,
            FG_CYAN        = 3,
            FG_MAGENTA     = 5,
            FG_YELLOW      = 6,
            FG_DEFAULT     = 7,
            BG_RED         = 41,
            BG_GREEN       = 42,
            BG_BLUE        = 44,
            BG_DEFAULT     = 49,
            BOLD           = 1,
            DEFAULT_WEIGHT = 22
        };
    #else 
        enum Code {
            FG_RED         = 31,
            FG_GREEN       = 32,
            FG_BLUE        = 34,
            FG_CYAN        = 36,
            FG_MAGENTA     = 35,
            FG_YELLOW      = 33,
            FG_DEFAULT     = 39,
            BG_RED         = 41,
            BG_GREEN       = 42,
            BG_BLUE        = 44,
            BG_DEFAULT     = 49,
            BOLD           = 1,
            DEFAULT_WEIGHT = 22
        };
    #endif

        Font(unsigned color_code, unsigned weight_code) : color(color_code), weight(weight_code) {}
        Font(unsigned color_code) : color(color_code), weight(DEFAULT_WEIGHT) {}
        Font() : color(FG_DEFAULT), weight(DEFAULT_WEIGHT) {}
        friend std::ostream& ::operator<<(std::ostream&, const Font&);
    private: 
        unsigned color;
        unsigned weight;
    };

    const Font input(Font::FG_DEFAULT);
    const Font prompt(Font::FG_CYAN, Font::BOLD);
    const Font error(Font::FG_RED, Font::BOLD);
    const Font heading(Font::FG_YELLOW, Font::BOLD);
    const Font highlight(Font::FG_YELLOW, Font::BOLD);
    const Font message(Font::FG_CYAN);
    const Font success(Font::FG_GREEN, Font::BOLD);
    const Font norm;
};

#endif