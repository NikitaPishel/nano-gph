#ifndef CANV_GPH_COLORS
#define CANV_GPH_COLORS
#include <string>
#include <sys/ioctl.h>
#include <unordered_map>

namespace gph {
    class Colors {
    private:
        Colors();
        
        std::unordered_map<std::string, std::string> colorMap;
        
    public:
        static const Colors& getInstance();

        std::string getColorId(std::string color) const;

        Colors(const Colors&) = delete;
        Colors& operator=(const Colors&) = delete;
        Colors(Colors&&) = delete;
        Colors& operator=(Colors&&) = delete;
        
    };
};

#endif