#include <string>
#include <vector>
#include <cstdint>
#include "gphUtil.h"
#include "ngph/texture.h"
#include "grid.h"

namespace gph {

    class Texture::Impl {
    public:
        Grid grid;

        Impl(Grid grid): grid(grid) {};
    };

    // Builder constructor; 
    Texture::Builder::Builder(int xSize,int ySize): pImpl(new Impl(Grid(xSize, ySize))) {}

    // Builder deconstructor
    Texture::Builder::~Builder() {
        delete this->pImpl;
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
        Grid::Pixel pix;
        pix.symbol = symbol;
        pix.textColor = textColorId;
        pix.backColor = backColorId;

        for (uint32_t xShift = 0; xShift < xSize; xShift++) {
            for (uint32_t ySift = 0; ySift < ySize; ySift++) {
                this->pImpl->grid.getPixel(xPos+xShift, yPos+ySift) = pix;
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
        Impl* pImpl = this->pImpl;
        this->pImpl = nullptr;
        
        return Texture(pImpl);
    }

    Texture::Texture(Impl* pImpl): pImpl(pImpl) {}

    Texture::~Texture() {
        delete this->pImpl;
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

    // copy constructor
    Texture::Texture(const Texture& other) {
        this->pImpl = new Impl(other.pImpl->grid);
    }

    // move constructor
    Texture::Texture(Texture&& other) noexcept {
        other.pImpl = nullptr;
    }
    
    // copy assignment
    Texture& Texture::operator=(const Texture& other) {
        if (this != &other) {
            delete this->pImpl;
            this->pImpl = new Impl(other.pImpl->grid);
        }

        return *this;
    }
    
    // move assignment
    Texture& Texture::operator=(Texture&& other) noexcept {
        if (this != &other) {
            delete pImpl; // Free existing resource
            pImpl = other.pImpl; // Steal the pointer
            other.pImpl = nullptr; // Nullify the source
        }

        return *this;
    }
}