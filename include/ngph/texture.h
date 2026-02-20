#ifndef CANVAS_TEXTURE_H
#define CANVAS_TEXTURE_H

#include <ngph/colors.h>

namespace gph {
    class GridBuffer;
    class Grid;

    // class Texture is used to draw blocks of pixels on a canvas
    // it is different from as it is muted and isn't internal for gph
    class Texture {
    private:
        class Impl;
        const Impl* pImpl;

        public:
        class Builder {
        private:
            Impl* pImpl;

        public:
            Builder(int xSize = 1, int ySize = 1);
            ~Builder();

            // Size getters
            int getXSize() const;
            int getYSize() const;

            // building methods return Builder& instead of void to make chain method calls

            // add a single pixel
            Builder& setPixel(int xPos, int yPos, char8_t symbol, const Rgb textColor, const Rgb backColor);

            // add pixel by raw values (relative index)
            Builder& setPixelByGridIndex(int index, char8_t symbol, const Rgb textColor, const Rgb backColor);

            // fill a zone of a texture by color Id
            Builder& fillTexture(char8_t symbol, const Rgb textColor, const Rgb backColor);
            Builder& fillRow(int yPos, char8_t symbol, const Rgb textColor, const Rgb backColor);
            Builder& fillCol(int xPos, char8_t symbol, const Rgb textColor, const Rgb backColor);
            Builder& addBox(int xPos, int yPos, int xSize, int ySize, char8_t symbol, const Rgb textColor, const Rgb backColor);

            Builder& addTexture(int xPos, int yPos, const Texture& newTex);
            Builder& fillWithTexture(const Texture& newTex);

            // change size of a texture
            Builder& setSize(int xSize, int ySize);

            // set a specific grid to a texture
            Builder& setGrid(Grid grid);

            // build texture and transfer ownership to it (temporary builder)
            Texture build();

            // create texture and keep ownership (temporary texture interface)
            Texture create();
        };

        // Constructor
        Texture(Impl* pGrid);
        Texture(int xSize = 1, int ySize = 1);
        ~Texture();

        // Get texture dimensions
        int getXSize() const;
        int getYSize() const;

        // getter method for canvas function
        const Grid& getGrid() const;

        // a TexBuffer factory
        GridBuffer newBuffer() const;

        // copy/move constructor
        Texture(const Texture& other);
        Texture(Texture&& other) noexcept;

        // copy/move assignment
        Texture& operator=(const Texture& other);
        Texture& operator=(Texture&& other) noexcept;
    };
}

#endif
