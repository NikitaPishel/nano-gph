#ifndef CANV_GRID_H
#define CANV_GRID_H
#include <vector>
#include <cstdint>

namespace gph {

    // for circular dependency
    class GridBuffer;

    // matrix holder class with basic matrix control
    class Grid {
        
    public:
        // nested struct that holds information about characters in the matrix
        struct Pixel {
            char symbol;
            std::string textColor; // 16 colors of the basic linux terminal
            std::string backColor; // same 16 colors

            // struct constructor (for standard member list)
            Pixel();
        };
        
        // matrix dimensions to exclude constant vector size recall
        uint32_t xSize;
        uint32_t ySize;
        
        // Basic control methods
        Grid(int xSize = 0, int ySize = 0);
        void setGridSize(int xSize, int ySize);
        
        void setPixel(int xPos, int yPos, char symbol = ' ', std::string textColor = "0", std::string backColor = "0");
        void addPixel(int xPos, int yPos, Pixel pix);
        
        Pixel& getPixel(int xPos, int yPos);
        const Pixel& getPixel(int xPos, int yPos) const;
        
        Pixel& getPixelByIndex(int index);
        const Pixel& getPixelByIndex(int index) const;
        
        // get x and y position of a pixel with index x
        const std::pair<uint32_t, uint32_t> getPixelPos(int index) const;
        
        // get a serialized copy of a matrix
        GridBuffer newBuffer() const;
        
        // total grid size. Used for internal functionality (indexing)
        uint32_t gridSize;
        
    private:
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
