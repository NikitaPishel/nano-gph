#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include "gphUtil.h"
#include "ngph/texture.h"
#include "grid.h"

namespace gph {

    class Texture::Impl {
    public:
        Grid grid;

        // checks if texture owns implementation
        bool ownedByTex;

        Impl(Grid grid, bool ownedByTex = false): grid(grid), ownedByTex(ownedByTex) {};
    };

    // Builder constructor; 
    Texture::Builder::Builder(int xSize,int ySize): pImpl(new Impl(Grid(xSize, ySize))) {}

    // Builder deconstructor
    Texture::Builder::~Builder() {
        delete this->pImpl;
    }

    int Texture::Builder::getXSize() const {
        return this->pImpl->grid.xSize;
    }

    int Texture::Builder::getYSize() const {
        return this->pImpl->grid.ySize;
    }

    // all same but with id instead of color name
    Texture::Builder& Texture::Builder::setPixelById(int xPos, int yPos, char symbol, const std::string& textColorId, const std::string& backColorId) {
        this->pImpl->grid.setPixel(xPos, yPos, symbol, textColorId, backColorId);

        return *this;
    }

    // also uses color id
    Texture::Builder& Texture::Builder::setPixelByGridIndex(int index, char symbol, const std::string& textColorId, const std::string& backColorId) {
        Grid::Pixel pix;
        pix.symbol = symbol;
        pix.textColor = textColorId;
        pix.backColor = backColorId;

        this->pImpl->grid.getPixelByIndex(index) = pix;

        // pointer for chain method calls
        return *this;
    }
    
    // fill the whole grid with the same pixel (uses color id)
    Texture::Builder& Texture::Builder::fillTextureById(char symbol, const std::string& textColorId, const std::string& backColorId) {
        Grid::Pixel pix;
        pix.symbol = symbol;
        pix.textColor = textColorId;
        pix.backColor = backColorId;
        
        for (uint32_t i = 0; i < this->pImpl->grid.gridSize; i++) {
            this->pImpl->grid.getPixelByIndex(i) = pix;
        }

        // pointer for chain method calls
        return *this;
    }
    
    // fill a chosen row with the same pixel (uses color id)
    Texture::Builder& Texture::Builder::fillRowById(int yPos, char symbol, const std::string& textColorId, const std::string& backColorId){
        Grid::Pixel pix;
        pix.symbol = symbol;
        pix.textColor = textColorId;
        pix.backColor = backColorId;
        
        for (uint32_t i = 0; i < this->pImpl->grid.xSize; i++) {
            this->pImpl->grid.getPixel(i, yPos) = pix;
        }
        
        // pointer for chain method calls
        return *this;
    }
    
    // fill a chosen column with the same pixel (uses color id)
    Texture::Builder& Texture::Builder::fillColById(int xPos, char symbol, const std::string& textColorId, const std::string& backColorId) {
        Grid::Pixel pix;
        pix.symbol = symbol;
        pix.textColor = textColorId;
        pix.backColor = backColorId;
        
        for (uint32_t i = 0; i < this->pImpl->grid.ySize; i++) {
            this->pImpl->grid.getPixel(xPos, i) = pix;
        }

        // pointer for chain method calls
        return *this;
    }

    Texture::Builder& Texture::Builder::addBoxById(int xPos, int yPos, int xSize, int ySize, char symbol, const std::string& textColorId, const std::string& backColorId) {
        if (xPos < 0 || yPos < 0 || xPos >= this->pImpl->grid.xSize || yPos >= this->pImpl->grid.ySize) {
            throw std::out_of_range("Box position out of range");
        }

        if (xSize < 1 || ySize < 1) {
            throw std::invalid_argument("Invalid box size");
        }

        Grid::Pixel pix;
        pix.symbol = symbol;
        pix.textColor = textColorId;
        pix.backColor = backColorId;

        for (int xShift = 0; xShift < xSize; xShift++) {
            for (int yShift = 0; yShift < ySize; yShift++) {
                int xPixPos = xPos + xShift;
                int yPixPos = yPos + yShift;
                
                if (xPixPos < this->pImpl->grid.xSize && yPixPos < this->pImpl->grid.ySize) {
                    this->pImpl->grid.getPixel(xPixPos, yPixPos) = pix;
                }
            }
        }
        
        return *this;
    }

    // just uses gphUtil to use grid's setPixel
    Texture::Builder& Texture::Builder::setPixel(int xPos, int yPos, char symbol, const std::string& textColorName, const std::string& backColorName) {
        const Colors& colors = Colors::getInstance();
        std::string textColorId = colors.getColorId(textColorName);
        std::string backColorId = colors.getColorId(backColorName);

        return setPixelById(xPos, yPos, symbol, textColorId, backColorId);
    }
    
    // fill the whole grid with the same pixel
    Texture::Builder& Texture::Builder::fillTexture(char symbol, const std::string& textColorName, const std::string& backColorName) {
        const Colors& colors = Colors::getInstance();
        std::string textColorId = colors.getColorId(textColorName);
        std::string backColorId = colors.getColorId(backColorName);

        return this->fillTextureById(symbol, textColorId, backColorId);
    }
    
    // fill a chosen row with the same pixel
    Texture::Builder& Texture::Builder::fillRow(int yPos, char symbol, const std::string& textColorName, const std::string& backColorName){
        const Colors& colors = Colors::getInstance();
        std::string textColorId = colors.getColorId(textColorName);
        std::string backColorId = colors.getColorId(backColorName);
        
        return fillRowById(yPos, symbol, textColorId, backColorId);
    }
    
    // fill a chosen column with the same pixel
    Texture::Builder& Texture::Builder::fillCol(int xPos, char symbol, const std::string& textColorName, const std::string& backColorName) {
        const Colors& colors = Colors::getInstance();
        std::string textColorId = colors.getColorId(textColorName);
        std::string backColorId = colors.getColorId(backColorName);

        return fillColById(xPos, symbol, textColorId, backColorId);
    }

    Texture::Builder& Texture::Builder::addBox(int xPos, int yPos, int xSize, int ySize, char symbol, const std::string& textColorName, const std::string& backColorName) {
        const Colors& colors = Colors::getInstance();
        std::string textColorId = colors.getColorId(textColorName);
        std::string backColorId = colors.getColorId(backColorName);

        return addBoxById(xPos, yPos, xSize, ySize, symbol, textColorId, backColorId);
    }

    Texture::Builder& Texture::Builder::addTexture(int xPos, int yPos, const Texture& newTex) {
        if (xPos < 0 || yPos < 0) {
            throw std::out_of_range("Texture position out of range (below 0)");
        }

        if (xPos >= this->getXSize() || yPos >= this->getYSize()) {
            throw std::out_of_range("Texture position out of range (overflow)");
        }
        
        const Grid& grid = newTex.getGrid();

        // iterate through indexes of a grid and copy pixels with a shift
        for (int i = 0; i < grid.gridSize; i++) {
            const Grid::Pixel pix = grid.getPixelByIndex(i);
            
            std::pair<uint32_t, uint32_t> pixPos = grid.getPixelPos(i);

            uint32_t xShift = pixPos.first + xPos;
            uint32_t yShift = pixPos.second + yPos;

            if (xShift < this->getXSize() && yShift < this->getYSize()) {
                this->pImpl->grid.addPixel(xShift, yShift, pix);
            }
        }

        return *this;
    }

    Texture::Builder& Texture::Builder::fillWithTexture(const Texture& newTex) {
        const Grid& grid = newTex.getGrid();
        
        // calculate the amount of tiles that will be needed to fill the screen
        uint32_t xSize = (this->getXSize() + grid.xSize - 1) / grid.xSize;
        uint32_t ySize = (this->getYSize() + grid.ySize - 1) / grid.ySize;

        // iterate through indexes of a grid and copy pixels with a shift
        for (uint32_t xCount = 0; xCount < xSize; xCount++) {
            for (uint32_t yCount = 0; yCount < ySize; yCount++) {
                uint32_t xShift = xCount * grid.xSize;
                uint32_t yShift = yCount * grid.ySize;

                if (xShift < this->getXSize() && yShift < this->getYSize()) {
                    this->addTexture(xShift, yShift, newTex);
                }
            }
        }

        return *this;
    }
    
    // change the size of a grid
    Texture::Builder& Texture::Builder::setSize(int xSize, int ySize) {
        this->pImpl->grid.setGridSize(xSize, ySize);
        
        // pointer for chain method calls
        return *this;
    }
    
    Texture::Builder& Texture::Builder::setGrid(Grid grid) {
        this->pImpl->grid = grid;
        
        // pointer for chain method calls
        return *this;
    }

    // Build a Texture
    Texture Texture::Builder::build() {
        if (!pImpl) throw std::runtime_error("Builder already used!");
        this->pImpl->ownedByTex = true;

        Impl* pImpl = this->pImpl;
        this->pImpl = nullptr;
        
        return Texture(pImpl);
    }

    Texture Texture::Builder::create() {
        if (!pImpl) throw std::runtime_error("Builder already used!");
        return Texture(pImpl);
    }

    Texture::Texture(Impl* pImpl): pImpl(pImpl) {}

    Texture::Texture(int xSize, int ySize): pImpl(new Impl(Grid(xSize, ySize), true)) {}

    Texture::~Texture() {
        if (pImpl && this->pImpl->ownedByTex) {
            delete this->pImpl;
        }
    }
    
    int Texture::getXSize() const{
        return this->pImpl->grid.xSize;
    }
    
    int Texture::getYSize() const{
        return this->pImpl->grid.ySize;
    }
    
    const Grid& Texture::getGrid() const {
        return this->pImpl->grid;
    }
    
    GridBuffer Texture::newBuffer() const {
        return GridBuffer(this->pImpl->grid.newBuffer());
    }

    // Copy constructor
    Texture::Texture(const Texture& other) {
        if (other.pImpl) {
            // copy the Grid but not ownership
            pImpl = new Impl(other.pImpl->grid, true); 
        } else {
            pImpl = nullptr;
        }
    }

// move constructor
Texture::Texture(Texture&& other) noexcept {
    this->pImpl = other.pImpl;
    other.pImpl = nullptr;
}

// Copy assignment
Texture& Texture::operator=(const Texture& other) {
    if (this != &other) {
        if (pImpl && pImpl->ownedByTex) {
            delete pImpl;
        }

        if (other.pImpl) {
            pImpl = new Impl(other.pImpl->grid, true);
        } else {
            pImpl = nullptr;
        }
    }
    return *this;
}

// move assignment
Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (pImpl && pImpl->ownedByTex) {
            delete pImpl;
        }
        
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}
}