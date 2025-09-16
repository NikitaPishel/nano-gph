#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <unordered_map>
#include "gphUtil.h"

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
            {"none", "-1"},
            {"black", "0"},
            {"red", "1"},
            {"green", "2"},
            {"yellow", "3"},
            {"blue", "4"},
            {"magenta", "5"},
            {"cyan", "6"},
            {"white", "7"},
            {"b_black", "8"},
            {"b_red", "9"},
            {"b_green", "10"},
            {"b_yellow", "11"},
            {"b_blue", "12"},
            {"b_magenta", "13"},
            {"b_cyan", "14"},
            {"b_white", "15"}
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
            std::cerr << "Error: invalid color" << color << "\n" << std::endl;
            return 0;
        }
    }

};