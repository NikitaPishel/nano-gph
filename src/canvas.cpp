#include <cstdint>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include "gphUtil.h"
#include "grid.h"
#include "ngph/texture.h"
#include "ngph/canvas.h"

namespace gph {

    class Canvas::Impl {
    public:
        Grid canvas;
        Grid canvSnap;

        Impl(Grid canvas) : canvas(canvas), canvSnap(canvas) {};
    };

    // Canvas constructor; Makes a unique_ptr of Impl with Grid sized (xSize, ySize)
    Canvas::Canvas(int xSize, int ySize) : pImpl(std::make_unique<Impl>(Grid(xSize, ySize))) {
        system("clear");

        // hide cursor
        std::cout << "\033[?25l";
    }

    // default destructor
    Canvas::~Canvas() {
        std::string finalClear;
        // when canvas is destroyed, show cursor again;
        finalClear.append("\033[?25h");

        // move cursor for the image to be "on top"
        finalClear.append("\x1b[");
        finalClear.append(std::to_string(this->getYSize()+1)); // row (y)
        finalClear.append(";");
        finalClear.append(std::to_string(1)); // col (x)
        finalClear.append("H");
        finalClear.append("\n");

        std::cout << finalClear;
    };

    // get horizontal canvas size
    uint32_t Canvas::getXSize() const {
        return this->pImpl->canvas.getXSize();
    }

    // get vertical canvas size
    uint32_t Canvas::getYSize() const {
        return this->pImpl->canvas.getYSize();
    }

    uint32_t Canvas::getCanvSize() const {
        return this->pImpl->canvas.getGridSize();
    }

    // set the canvas size
    void Canvas::setSize(int xSize, int ySize) {
        this->pImpl->canvas.setGridSize(xSize, ySize);
        this->pImpl->canvSnap.setGridSize(xSize, ySize);
        system("clear");
    }

    // automatically set the canvas size to the terminal size, if was resized return true
    bool Canvas::updateSize() {
        winsize window = getWinSize();
        const int& xSize = window.ws_col;
        const int& ySize = window.ws_row;

        if (xSize != this->getXSize() || ySize != this->getYSize()){
            this->setSize(xSize, ySize);
            return true;
        }

        return false;
    }

    void Canvas::setPixel(int xPos, int yPos, char8_t symbol, Rgb textColor, Rgb backColor) {
        this->pImpl->canvas.setPixel(xPos, yPos, symbol, textColor, backColor);
    }

    // add a texture to the canvas
    void Canvas::addTexture(int xPos, int yPos, const Texture& newTex) {
        if (xPos < 0 || yPos < 0) {
            throw std::out_of_range("Texture position out of range (below 0)");
        }

        if (xPos >= this->getXSize() || yPos >= this->getYSize()) {
            throw std::out_of_range("Texture position out of range (overflow)");
        }

        const Grid& grid = newTex.getGrid();

        // iterate through indexes of a grid and copy pixels with a shift
        for (int i = 0; i < grid.getGridSize(); i++) {
            const Grid::Pixel pix = grid.getPixelByIndex(i);

            std::pair<uint32_t, uint32_t> pixPos = grid.getPixelPos(i);

            uint32_t xShift = pixPos.first + xPos;
            uint32_t yShift = pixPos.second + yPos;

            if (xShift < this->getXSize() && yShift < this->getYSize()) {
                this->pImpl->canvas.addPixel(xShift, yShift, pix);
            }
        }
    }

    // add a texture to the canvas
    void Canvas::iterateTexture(int xPos, int yPos, int xSize, int ySize, const Texture& newTex) {
        if (xPos < 0 || yPos < 0) {
            throw std::out_of_range("Texture position out of range (below 0)");
        }

        if (xPos >= this->getXSize() || yPos >= this->getYSize()) {
            throw std::out_of_range("Texture position out of range (overflow)");
        }

        const Grid& grid = newTex.getGrid();

        // iterate through indexes of a grid and copy pixels with a shift
        for (uint32_t xCount = 0; xCount < xSize; xCount++) {
            for (uint32_t yCount = 0; yCount < ySize; yCount++) {
                uint32_t xShift = xCount * grid.getXSize() + xPos;
                uint32_t yShift = yCount * grid.getYSize() + yPos;

                if (xShift < this->getXSize() && yShift < this->getYSize()) {
                    this->addTexture(xShift, yShift, newTex);
                }
            }
        }
    }

    void Canvas::fillWithTexture(const Texture& newTex) {
        const Grid& grid = newTex.getGrid();

        // calculate the amount of tiles that will be needed to fill the screen
        uint32_t xSize = (this->getXSize() + grid.getXSize() - 1) / grid.getXSize();
        uint32_t ySize = (this->getYSize() + grid.getYSize() - 1) / grid.getYSize();

        this->iterateTexture(0, 0, xSize, ySize, newTex);
    }

    // Render and display current canvas
    void Canvas::render() {
        std::string renderedImage;

        // reserve space for the string
        // *39 because renderedPix is at max 39 bytes long
        // add y size for each new line ]n
        // add 3 for \033[H at the start of a render (sets cursor to position (0, 0))
        // add 3 for style reset "\033[0m", + 6 in total with \033[H
        size_t renderSize = this->getCanvSize() * 39 + this->getYSize() + 6;
        renderedImage.reserve(renderSize);

        // move cursor to the position (0, 0)
        renderedImage.append("\033[H");

        // iterate through pixels and find their values
        for (int i = 0; i < this->getCanvSize(); i++) {
            const Grid::Pixel& pix = this->pImpl->canvas.getPixelByIndex(i);
            const Grid::Pixel& snapPix = this->pImpl->canvSnap.getPixelByIndex(i);

            // check for diff
            if (
                pix.backColor.r != snapPix.backColor.r ||
                pix.backColor.g != snapPix.backColor.g ||
                pix.backColor.b != snapPix.backColor.b ||
                pix.textColor.r != snapPix.textColor.r ||
                pix.textColor.g != snapPix.textColor.g ||
                pix.textColor.b != snapPix.textColor.b ||
                pix.symbol != snapPix.symbol
            ) {
                std::pair<uint32_t, uint32_t> pixPos = this->pImpl->canvas.getPixelPos(i);

                renderedImage.append("\x1b[");
                renderedImage.append(std::to_string(pixPos.second+1)); // row (y)
                renderedImage.append(";");
                renderedImage.append(std::to_string(pixPos.first+1)); // col (x)
                renderedImage.append("H");

                // format pixel and add it to the rendered image
                renderedImage.append(pix.toAnsiString());
            }
        }

        // style reset
        renderedImage.append("\033[0m");

        // output rendered image to the terminal
        std::cout << renderedImage;
        std::cout.flush();

        // take a snap of the current image
        this->pImpl->canvSnap = this->pImpl->canvas;
    }
}
