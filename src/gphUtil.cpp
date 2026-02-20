#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include "gphUtil.h"

// graphics utility namespace
namespace gph {
    winsize getWinSize() {
        winsize window;

        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window) == -1) {
            std::cerr << ("Failed to get window size");

            window.ws_col = 1;
            window.ws_row = 1;
            window.ws_xpixel = 1;
            window.ws_ypixel = 1;
        }

        return window;
    };
};
