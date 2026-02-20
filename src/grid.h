#ifndef CANV_GRID_H
#define CANV_GRID_H
#include <vector>
#include <cstdint>
#include <string>
#include "ngph/colors.h"

namespace gph {

    // for circular dependency
    class GridBuffer;

    // matrix holder class with basic matrix control
    class Grid {

    public:
        // nested struct that holds information about characters in the matrix
        struct Pixel {
            char8_t symbol;
            Rgb textColor;
            Rgb backColor; // background color

            // struct constructor (for standard member list)
            Pixel();

            std::string toAnsiString() const;
        };

        // Basic control methods
        Grid(int xSize = 0, int ySize = 0);
        void setGridSize(int xSize, int ySize);

        void setPixel(int xPos, int yPos, char8_t symbol, Rgb textColor, Rgb backColor);
        void addPixel(int xPos, int yPos, Pixel pix);

        Pixel& getPixel(int xPos, int yPos);
        const Pixel& getPixel(int xPos, int yPos) const;

        Pixel& getPixelByIndex(int index);
        const Pixel& getPixelByIndex(int index) const;

        // get x and y position of a pixel with index x
        const std::pair<uint32_t, uint32_t> getPixelPos(int index) const;

        // get a serialized copy of a matrix
        GridBuffer newBuffer() const;

        uint32_t getXSize() const {return xSize;}
        uint32_t getYSize() const {return ySize;}
        uint32_t getGridSize() const {return gridSize;}

    private:
        // matrix dimensions to exclude constant vector size recall
        uint32_t xSize;
        uint32_t ySize;

        // total grid size. Used for internal functionality (indexing)
        uint32_t gridSize;

        // Pixel grid itself; Vector that works as a projector of a linear indexed matrix
        std::vector<Pixel> matrix;
    };

    class GridBuffer {
    public:
        std::vector<char> buffer;

        // constructor
        GridBuffer(std::vector<char> buffer);

        // return unserialized grid form the buffer
        Grid unpack();

        // return unserialized grid form the buffer
        uint64_t getSize();
    };
}

#endif
