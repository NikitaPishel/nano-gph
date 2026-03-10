#include <cstdint>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include "gphUtil.h"
#include "grid.h"
#include "ngph/texture.h"
#include "ngph/canvas.h"

namespace {
    static void appendUint(std::string& s, uint32_t n) {
        char buf[10];
        int len = 0;
        if (n == 0) {
            s.push_back('0');
            return;
        }
        while (n > 0) {
            buf[len++] = static_cast<char>('0' + n % 10);
            n /= 10;
        }
        for (int i = len - 1; i >= 0; --i) {
            s.push_back(buf[i]);
        }
    }

    static void appendUtf32AsUtf8(std::string& s, char32_t cp) {
        if (cp <= 0x7F) {
            s.push_back(static_cast<char>(cp));
        } else if (cp <= 0x7FF) {
            s.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            s.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else if (cp <= 0xFFFF) {
            s.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            s.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            s.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else {
            // 4-byte sequence (emoji / wide codepoint); write a null placeholder
            // after so the terminal knows the next cell is occupied
            s.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            s.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            s.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            s.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
            s.push_back('\0');
        }
    }
}

namespace gph {

    class Canvas::Impl {
    public:
        Grid canvas;
        Grid canvSnap;
        bool dirty = false;
        bool redraw = true;

        Impl(Grid canvas) : canvas(canvas), canvSnap(canvas) {};
    };

    // Canvas constructor; Makes a unique_ptr of Impl with Grid sized (xSize, ySize)
    Canvas::Canvas(int xSize, int ySize) : pImpl(std::make_unique<Impl>(Grid(xSize, ySize))) {
        std::cout << "\033[H\033[2J\033[3J";

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
        this->pImpl->redraw = true;
        std::cout << "\033[H\033[2J\033[3J";
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

    void Canvas::setPixel(int xPos, int yPos, char32_t symbol, Rgb textColor, Rgb backColor) {
        this->pImpl->canvas.setPixel(xPos, yPos, symbol, textColor, backColor);
        this->pImpl->dirty = true;
    }

    // add a texture to the canvas
    void Canvas::addTexture(int xPos, int yPos, const Texture& newTex) {
        const Grid& grid = newTex.getGrid();

        // iterate through indexes of a grid and copy pixels with a shift
        for (int i = 0; i < grid.getGridSize(); i++) {
            const Grid::Pixel pix = grid.getPixelByIndex(i);

            std::pair<uint32_t, uint32_t> pixPos = grid.getPixelPos(i);

            int xShift = static_cast<int>(pixPos.first) + xPos;
            int yShift = static_cast<int>(pixPos.second) + yPos;

            if (xShift >= 0 && xShift < static_cast<int>(this->getXSize()) &&
                yShift >= 0 && yShift < static_cast<int>(this->getYSize())) {
                this->pImpl->canvas.addPixel(xShift, yShift, pix);
                this->pImpl->dirty = true;
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

    void Canvas::render() {
        if (this->pImpl->redraw) {
            renderFull();
        } else if (this->pImpl->dirty) {
            renderDiff();
        }
    }

    void Canvas::renderFull() {
        std::string renderedImage;

        size_t renderSize = this->getCanvSize() * 54 + 4;
        renderedImage.reserve(renderSize);

        for (int i = 0; i < this->getCanvSize(); i++) {
            const Grid::Pixel& pix = this->pImpl->canvas.getPixelByIndex(i);
            std::pair<uint32_t, uint32_t> pixPos = this->pImpl->canvas.getPixelPos(i);

            renderedImage.append("\x1b[");
            appendUint(renderedImage, pixPos.second + 1);
            renderedImage.push_back(';');
            appendUint(renderedImage, pixPos.first + 1);
            renderedImage.push_back('H');

            renderedImage.append("\033[38;2;");
            appendUint(renderedImage, pix.textColor.r);
            renderedImage.push_back(';');
            appendUint(renderedImage, pix.textColor.g);
            renderedImage.push_back(';');
            appendUint(renderedImage, pix.textColor.b);
            renderedImage.push_back('m');

            renderedImage.append("\033[48;2;");
            appendUint(renderedImage, pix.backColor.r);
            renderedImage.push_back(';');
            appendUint(renderedImage, pix.backColor.g);
            renderedImage.push_back(';');
            appendUint(renderedImage, pix.backColor.b);
            renderedImage.push_back('m');

            appendUtf32AsUtf8(renderedImage, pix.symbol);
        }

        renderedImage.append("\033[0m");

        std::cout << renderedImage;
        std::cout.flush();

        this->pImpl->canvSnap = this->pImpl->canvas;
        this->pImpl->dirty = false;
        this->pImpl->redraw = false;
    }

    void Canvas::renderDiff() {
        std::string renderedImage;

        size_t renderSize = this->getCanvSize() * 54 + 4;
        renderedImage.reserve(renderSize);

        for (int i = 0; i < this->getCanvSize(); i++) {
            const Grid::Pixel& pix = this->pImpl->canvas.getPixelByIndex(i);
            const Grid::Pixel& snapPix = this->pImpl->canvSnap.getPixelByIndex(i);

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
                appendUint(renderedImage, pixPos.second + 1);
                renderedImage.push_back(';');
                appendUint(renderedImage, pixPos.first + 1);
                renderedImage.push_back('H');

                renderedImage.append("\033[38;2;");
                appendUint(renderedImage, pix.textColor.r);
                renderedImage.push_back(';');
                appendUint(renderedImage, pix.textColor.g);
                renderedImage.push_back(';');
                appendUint(renderedImage, pix.textColor.b);
                renderedImage.push_back('m');

                renderedImage.append("\033[48;2;");
                appendUint(renderedImage, pix.backColor.r);
                renderedImage.push_back(';');
                appendUint(renderedImage, pix.backColor.g);
                renderedImage.push_back(';');
                appendUint(renderedImage, pix.backColor.b);
                renderedImage.push_back('m');

                appendUtf32AsUtf8(renderedImage, pix.symbol);
            }
        }

        renderedImage.append("\033[0m");

        std::cout << renderedImage;
        std::cout.flush();

        this->pImpl->canvSnap = this->pImpl->canvas;
        this->pImpl->dirty = false;
    }
}
