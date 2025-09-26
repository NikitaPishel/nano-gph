#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <unordered_map>
#include "gphUtil.h"
#include "colors.h"

// graphics utility namespace
namespace gph {
    winsize getWinSize() {
        winsize window;

        window.ws_col = 1;
        window.ws_row = 1;
        window.ws_xpixel = 1;
        window.ws_ypixel = 1;

        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window) == -1) {
            std::cerr << "Failed to get window size\n";
        }

        return window;
    };

    const Colors& Colors::getInstance() {
        static Colors instance;
        return instance;
    }

    // constructor, inits color map
    Colors::Colors()
        : colorMap{
            {"black", "0"},
            {"red", "1"},
            {"green", "2"},
            {"yellow", "3"},
            {"blue", "4"},
            {"magenta", "5"},
            {"cyan", "6"},
            {"white", "7"},
            {"grey", "8"},
            {"bRed", "9"},
            {"bGreen", "10"},
            {"bYellow", "11"},
            {"bBlue", "12"},
            {"bMagenta", "13"},
            {"bCyan", "14"},
            {"bWhite", "15"}
        }
        // empty constructor body
    {

    }

    // returns color id, if color not found return black and warn
    std::string Colors::getColorId(std::string color) const {
        auto colorRecord = colorMap.find(color);

        if (colorRecord != this->colorMap.end()) {
            return colorRecord->second;
        }

        else {
            // return black color if there's an invalid color
            std::cerr << "Error: invalid color \'" << color << "\'\n";
            return "0";
        }
    }

};