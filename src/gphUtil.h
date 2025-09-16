#ifndef CANV_GPH_UTIL
#define CANV_GPH_UTIL
#include <string>
#include <sys/ioctl.h>
#include <unordered_map>

namespace gph {
    winsize getWinSize();

    class Colors {
    public:
        static const Colors& getInstance();

        std::string getColorId(std::string color) const;

    private:
        Colors();

        std::unordered_map<std::string, std::string> colorMap;

        Colors(const Colors&) = delete;
        Colors& operator=(const Colors&) = delete;
    };
};

#endif