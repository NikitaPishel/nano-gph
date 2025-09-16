#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>
#include "gphUtil.h"
#include "grid.h"

namespace gph {
    // Serialized Grid constructor
    GridBuffer::GridBuffer(std::vector<char> buffer): buffer(buffer) {

    }

    // set Pixel instance init values
    Grid::Pixel::Pixel(): symbol(' '), backColor("0"), textColor("7") {

    }

    // set Grid instance init values
    Grid::Grid(int xSize, int ySize) {
        if (xSize < 1 || ySize < 1) {
            throw std::invalid_argument("Invalid grid size: dimension is below 1");
        }
        
        this->xSize = xSize;
        this->ySize = ySize;
        this->gridSize = xSize * ySize;

        this->matrix = std::vector<Pixel>(this->gridSize);
    }

    // change grid size
    void Grid::setGridSize(int xSize, int ySize) {
        if (xSize < 1 || ySize < 1) {
            throw std::invalid_argument("Invalid grid size: dimension is below 1");
        }
        
        // find new matrix size
        uint32_t gridSize = xSize*ySize;

        using PixelRecord = std::pair<uint32_t, Pixel>;
        uint32_t prjSize = std::min(this->gridSize, gridSize);

        // temporary stores projected matrix part (same size as the smallest matrix)
        std::vector<PixelRecord> prjMatrix(prjSize);

        uint32_t oldIndex = 0;

        // loop through old vector and rearrange pixel to keep the same coordinates
        for (uint32_t i = 0; i < prjSize; i++) {

            // calculate pixel x-position based on its old grid index
            uint32_t xPos = oldIndex % this->xSize;
            
            // check if it is out of range
            if (xPos >= xSize) {
                // calculate new index that will be in range, and find new x-position
                oldIndex = oldIndex - xPos + this->xSize;
                xPos = oldIndex % this->xSize;
            }
            
            // calculate pixel y-position based on its old grid index
            uint32_t yPos = (oldIndex - xPos) / this->xSize;

            // calculate new pixel index based on its position
            uint32_t newIndex = xSize*yPos + xPos;

            // store pixel in projection list
            Pixel pix = this->matrix[oldIndex];
            prjMatrix[i] = {newIndex, pix};
            
            // replace projected pixel from the matrix with blank pixel
            this->matrix[oldIndex] = Pixel();

            // go to the next index
            oldIndex++;
        }

        this->xSize = xSize;
        this->ySize = ySize;
        this->gridSize = gridSize;
        
        this->matrix.resize(gridSize);

        // project pixels to a new matrix
        for (uint32_t i = 0; i < prjSize; i++) {
            uint32_t newIndex = prjMatrix[i].first;
            Pixel pix = prjMatrix[i].second;
            this->matrix[newIndex] = pix;
        };
    }

    // get an access to a pixel (used if you need full control compared to setPixel) or its second version with indirect access
    Grid::Pixel& Grid::getPixel(int xPos, int yPos) {
        if (xPos < 0 || xPos >= xSize || yPos < 0 || yPos >= ySize) {
            throw std::out_of_range("Pixel index out of range.");
        }

        // calculate an index of a pixel in a vector matrix holder
        uint32_t pixIndex = xPos + yPos*xSize;

        return this->matrix[pixIndex];
    }

    const Grid::Pixel& Grid::getPixel(int xPos, int yPos) const {
        if (xPos < 0 || xPos >= xSize || yPos < 0 || yPos >= ySize) {
            throw std::out_of_range("Pixel index out of range.");
        }
        
        uint32_t pixIndex = xPos + yPos*xSize;

        return this->matrix[pixIndex];
    }

    // get an access to a pixel (used if you need full control compared to setPixel) or its second version with indirect access
    Grid::Pixel& Grid::getPixelByIndex(int index) {
        if (index >= this->gridSize) {
            throw std::out_of_range("Pixel index out of range.");
        }

        return this->matrix[index];
    }

    const Grid::Pixel& Grid::getPixelByIndex(int index) const {
        if (index >= this->gridSize) {
            throw std::out_of_range("Pixel index out of range.");
        }
        
        return this->matrix[index];
    }
    
    // update pixel parameters (or add a pixel)
    void Grid::setPixel(int xPos, int yPos, char symbol, std::string textColor, std::string backColor) {
        if (xPos < 0 || xPos >= xSize || yPos < 0 || yPos >= ySize) {
            throw std::out_of_range("Pixel index out of range.");
        }
        
        Pixel& slctPixel = this->getPixel(xPos, yPos);
        slctPixel.symbol = symbol;
        slctPixel.textColor = textColor;
        slctPixel.backColor = backColor;
    }
    
    void Grid::addPixel(int xPos, int yPos, Pixel pix) {
        if (xPos < 0 || xPos >= xSize || yPos < 0 || yPos >= ySize) {
            throw std::out_of_range("Pixel index out of range.");
        }
        
        this->getPixel(xPos, yPos) = pix;
    }
    
    const std::pair<uint32_t, uint32_t> Grid::getPixelPos(int index) const {
        if (index < 0 || index >= this->gridSize) {
            throw std::out_of_range("Pixel index out of range.");
        }

        std::pair<uint32_t, uint32_t> pos;

        pos.first = index % this->xSize;
        pos.second = (index - pos.first) / this->xSize;
        return pos;
    }

    GridBuffer Grid::newBuffer() const {
        std::vector<char> binGrid;

        auto append = [&](const void* data, std::size_t size) {
            const char* bytes = reinterpret_cast<const char*>(data);
            binGrid.insert(binGrid.end(), bytes, bytes + size);
        };

        append(&this->xSize, sizeof(this->xSize));
        append(&this->ySize, sizeof(this->ySize));

        for (int y = 0; y < this->ySize; ++y) {
            for (int x = 0; x < this->xSize; ++x) {
                const Grid::Pixel pix = this->getPixel(x, y);
                append(&pix.symbol, sizeof(pix.symbol));
                
                // append the size of string as it is non-fixed size
                uint8_t textLen = pix.textColor.size();
                append(&textLen, sizeof(textLen));
                append(pix.textColor.data(), textLen);

                uint8_t backLen = pix.backColor.size();
                append(&backLen, sizeof(backLen));
                append(pix.backColor.data(), backLen);
            }
        }
        
        GridBuffer buffer(binGrid);

        return buffer;
    }

    // return unserialized buffer data
    Grid GridBuffer::unpack() {
        std::size_t offset = 0;

        auto read = [&](void* data, std::size_t size) {
            std::memcpy(data, this->buffer.data() + offset, size);
            offset += size;
        };

        uint32_t xSize;
        uint32_t ySize;

        read(&xSize, sizeof(xSize));
        read(&ySize, sizeof(ySize));

        Grid grid(xSize, ySize);

        for (int y = 0; y < ySize; ++y) {
            for (int x = 0; x < xSize; ++x) {
                char symbol;
                read(&symbol, sizeof(symbol));

                uint8_t textLen;
                read(&textLen, sizeof(textLen));
                std::string textColor(textLen, '\0');
                read(textColor.data(), textLen);

                uint8_t backLen;
                read(&backLen, sizeof(backLen));
                std::string backColor(backLen, '\0');
                read(backColor.data(), backLen);

                grid.setPixel(x, y, symbol, textColor, backColor);
            }
        }

        return grid;
    }

    uint64_t GridBuffer::getSize() {
        return this->buffer.size();
    }
}