#ifndef CANV_GPH_COLORS
#define CANV_GPH_COLORS
#include <cstdint>

namespace gph {
    struct Rgb {
        uint8_t r;
        uint8_t g;
        uint8_t b;

        Rgb(uint8_t r=0, uint8_t g=0, uint8_t b=0);
    };
};

#endif
