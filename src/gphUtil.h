#ifndef CANV_GPH_UTIL
#define CANV_GPH_UTIL
#include <sys/ioctl.h>
#include <cstdint>

namespace gph {
    winsize getWinSize();

    // Returns true if cp is an East Asian wide character (occupies 2 terminal columns)
    bool isWideChar(char32_t cp);
};

#endif
