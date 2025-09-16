#ifndef CANVAS_H
#define CANVAS_H
#include <string>
#include <memory>
#include "ngph/texture.h"

namespace gph {
    class Canvas {
    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;

    public:
        // Constructor
        Canvas(int xSize = 1, int ySize = 1);
        ~Canvas();

        // Canvas size
        uint32_t getXSize() const;
        uint32_t getYSize() const;
        uint32_t getCanvSize() const;
        void setSize(int xSize, int ySize);
        void updateSize();

        // Adding pixels to the canvas
        void setPixel(int xPos = 1, int yPos = 1, char symbol = ' ', std::string textColor = "white", std::string backColor = "black");
        void addTexture(int xPos, int yPos, const Texture& newTex);

        // render and display data
        void render();
    };
};
#endif
