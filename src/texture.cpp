#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include "ngph/texture.h"
#include "grid.h"
#include "gphUtil.h"

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
        return this->pImpl->grid.getXSize();
    }

    int Texture::Builder::getYSize() const {
        return this->pImpl->grid.getYSize();
    }

    // all same but with id instead of color name
    Texture::Builder& Texture::Builder::setPixel(int xPos, int yPos, char32_t symbol, Rgb textColor, Rgb backColor) {
        this->pImpl->grid.setPixel(xPos, yPos, symbol, textColor, backColor);

        return *this;
    }

    // also uses color id
    Texture::Builder& Texture::Builder::setPixelByGridIndex(int index, char32_t symbol, Rgb textColor, Rgb backColor) {
        Grid::Pixel pix;
        pix.symbol = symbol;
        pix.textColor = textColor;
        pix.backColor = backColor;

        this->pImpl->grid.getPixelByIndex(index) = pix;

        // pointer for chain method calls
        return *this;
    }

    // fill the whole grid with the same pixel (uses color id)
    Texture::Builder& Texture::Builder::fillTexture(char32_t symbol, Rgb textColor, Rgb backColor) {
        Grid::Pixel pix;
        pix.symbol = symbol;
        pix.textColor = textColor;
        pix.backColor = backColor;

        for (uint32_t i = 0; i < this->pImpl->grid.getGridSize(); i++) {
            this->pImpl->grid.getPixelByIndex(i) = pix;
        }

        // pointer for chain method calls
        return *this;
    }

    // fill a chosen row with the same pixel (uses color id)
    Texture::Builder& Texture::Builder::fillRow(int yPos, char32_t symbol, Rgb textColor, Rgb backColor){
        Grid::Pixel pix;
        pix.symbol = symbol;
        pix.textColor = textColor;
        pix.backColor = backColor;

        for (uint32_t i = 0; i < this->pImpl->grid.getXSize(); i++) {
            this->pImpl->grid.getPixel(i, yPos) = pix;
        }

        // pointer for chain method calls
        return *this;
    }

    // fill a chosen column with the same pixel (uses color id)
    Texture::Builder& Texture::Builder::fillCol(int xPos, char32_t symbol, Rgb textColor, Rgb backColor) {
        Grid::Pixel pix;
        pix.symbol = symbol;
        pix.textColor = textColor;
        pix.backColor = backColor;

        for (uint32_t i = 0; i < this->pImpl->grid.getYSize(); i++) {
            this->pImpl->grid.getPixel(xPos, i) = pix;
        }

        // pointer for chain method calls
        return *this;
    }

    Texture::Builder& Texture::Builder::addBox(int xPos, int yPos, int xSize, int ySize, char32_t symbol, Rgb textColor, Rgb backColor) {
        if (xPos < 0 || yPos < 0 || xPos >= this->pImpl->grid.getXSize() || yPos >= this->pImpl->grid.getYSize()) {
            throw std::out_of_range("Box position out of range");
        }

        if (xSize < 1 || ySize < 1) {
            throw std::invalid_argument("Invalid box size");
        }

        Grid::Pixel pix;
        pix.symbol = symbol;
        pix.textColor = textColor;
        pix.backColor = backColor;

        for (int xShift = 0; xShift < xSize; xShift++) {
            for (int yShift = 0; yShift < ySize; yShift++) {
                int xPixPos = xPos + xShift;
                int yPixPos = yPos + yShift;

                if (xPixPos < this->pImpl->grid.getXSize() && yPixPos < this->pImpl->grid.getYSize()) {
                    this->pImpl->grid.getPixel(xPixPos, yPixPos) = pix;
                }
            }
        }

        return *this;
    }

    Texture::Builder& Texture::Builder::addText(int xPos, int yPos, const std::u32string& text, const Rgb textColor, const Rgb backColor, TexFlag flag) {
        std::pair<int, int> cursor{xPos, yPos};
        return this->addText(cursor, text, textColor, backColor, flag);
    }

    // starts at cursor and moves it to where the text ended, so another addText can continue from it
    Texture::Builder& Texture::Builder::addText(std::pair<int, int>& cursor, const std::u32string& text, const Rgb textColor, const Rgb backColor, TexFlag flag) {
        int xPos = cursor.first;
        int yPos = cursor.second;
        int xSize = this->getXSize();
        int ySize = this->getYSize();

        if (flag == TexFlag::HORIZONTAL) {
            // Simulate layout with no wrapping to find the required width
            int curX = xPos;
            int maxX = xSize;
            for (char32_t cp : text) {
                if (cp == U'\n' || cp == U'\r') {
                    curX = 0;
                } else if (cp == U'\t') {
                    curX = (curX / 4 + 1) * 4;
                } else {
                    curX += isWideChar(cp) ? 2 : 1;
                }
                maxX = std::max(maxX, curX);
            }
            if (maxX > xSize) {
                this->pImpl->grid.setGridSize(maxX, ySize);

                // fill the newly added columns with the text background
                this->addBox(xSize, 0, maxX - xSize, ySize, U' ', backColor, backColor);
                xSize = maxX;
            }
        } else if (flag == TexFlag::VERTICAL) {
            // Simulate layout with wrapping at current xSize to find the required height
            int curX = xPos;
            int curY = yPos;
            for (char32_t cp : text) {
                if (cp == U'\n') {
                    curX = 0;
                    curY++;
                } else if (cp == U'\t') {
                    curX = (curX / 4 + 1) * 4;
                } else if (cp == U'\r') {
                    curX = 0;
                } else {
                    int charWidth = isWideChar(cp) ? 2 : 1;
                    if (curX > 0 && curX + charWidth > xSize) {
                        curX = 0;
                        curY++;
                    }
                    curX += charWidth;
                }
            }
            int neededY = curY + 1;
            if (neededY > ySize) {
                this->pImpl->grid.setGridSize(xSize, neededY);

                // fill the newly added rows with the text background
                this->addBox(0, ySize, xSize, neededY - ySize, U' ', backColor, backColor);
                ySize = neededY;
            }
        }

        int curX = xPos;
        int curY = yPos;

        for (char32_t cp : text) {
            if (cp == U'\n') {
                curX = 0;
                curY++;
            } else if (cp == U'\t') {
                curX = (curX / 4 + 1) * 4;
            } else if (cp == U'\r') {
                curX = 0;
            } else {
                bool wide = isWideChar(cp);
                int charWidth = wide ? 2 : 1;

                // wrap lazily, only when the next char doesn't fit, so the returned
                // cursor stays on the current row and chained calls render the same
                if (curX > 0 && curX + charWidth > xSize) {
                    curX = 0;
                    curY++;
                }
                if (curY >= ySize) break;

                this->pImpl->grid.setPixel(curX, curY, cp, textColor, backColor);

                // blank the right-half cell so it never shows stale content
                if (wide && curX + 1 < xSize) {
                    this->pImpl->grid.setPixel(curX + 1, curY, U' ', backColor, backColor);
                }

                curX += charWidth;
            }
        }

        cursor = {curX, curY};

        return *this;
    }

    Texture::Builder& Texture::Builder::addTexture(int xPos, int yPos, const Texture& newTex) {
        // partially visible textures are clipped, but a texture that lies
        // entirely outside would draw nothing, so treat it as an error
        if (xPos >= this->getXSize() || yPos >= this->getYSize() ||
            xPos + newTex.getXSize() <= 0 || yPos + newTex.getYSize() <= 0) {
            throw std::out_of_range("Texture is entirely outside the bounds");
        }

        const Grid& grid = newTex.getGrid();

        // iterate through indexes of a grid and copy pixels with a shift
        for (int i = 0; i < grid.getGridSize(); i++) {
            const Grid::Pixel pix = grid.getPixelByIndex(i);

            std::pair<uint32_t, uint32_t> pixPos = grid.getPixelPos(i);

            int xShift = static_cast<int>(pixPos.first) + xPos;
            int yShift = static_cast<int>(pixPos.second) + yPos;

            if (xShift >= 0 && xShift < static_cast<int>(this->getXSize()) &&
                yShift >= 0 && yShift < static_cast<int>(this->getYSize())) {
                this->pImpl->grid.addPixel(xShift, yShift, pix);
            }
        }

        return *this;
    }

    Texture::Builder& Texture::Builder::fillWithTexture(const Texture& newTex) {
        const Grid& grid = newTex.getGrid();

        // calculate the amount of tiles that will be needed to fill the screen
        uint32_t xSize = (this->getXSize() + grid.getXSize() - 1) / grid.getXSize();
        uint32_t ySize = (this->getYSize() + grid.getYSize() - 1) / grid.getYSize();

        // iterate through indexes of a grid and copy pixels with a shift
        for (uint32_t xCount = 0; xCount < xSize; xCount++) {
            for (uint32_t yCount = 0; yCount < ySize; yCount++) {
                uint32_t xShift = xCount * grid.getXSize();
                uint32_t yShift = yCount * grid.getYSize();

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

    Texture Texture::Builder::snapshot() {
        if (!pImpl) throw std::runtime_error("Builder already used!");
        return Texture(new Impl(pImpl->grid, true));
    }

    Texture::Texture(Impl* pImpl): pImpl(pImpl) {}

    Texture::Texture(int xSize, int ySize): pImpl(new Impl(Grid(xSize, ySize), true)) {}

    Texture::~Texture() {
        if (pImpl && this->pImpl->ownedByTex) {
            delete this->pImpl;
        }
    }

    int Texture::getXSize() const{
        return this->pImpl->grid.getXSize();
    }

    int Texture::getYSize() const{
        return this->pImpl->grid.getYSize();
    }

    char32_t Texture::getSymbol(int xPos, int yPos) const {
        return this->pImpl->grid.getPixel(xPos, yPos).symbol;
    }

    Rgb Texture::getFgColor(int xPos, int yPos) const {
        return this->pImpl->grid.getPixel(xPos, yPos).textColor;
    }

    Rgb Texture::getBgColor(int xPos, int yPos) const {
        return this->pImpl->grid.getPixel(xPos, yPos).backColor;
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
