#ifndef CANVAS_TEXTURE_H
#define CANVAS_TEXTURE_H

#include <string>
#include <cstdint>
#include <vector>

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


            // building methods return Builder& instead of void to make chain method calls
            
            // add a single pixel
            Builder& setPixel(int xPos, int yPos, char symbol = ' ', const std::string& textColor = "white", const std::string& backColor = "black");
            
            // fill a zone of a texture
            Builder& fillTexture(char symbol = ' ', const std::string& textColor = "white", const std::string& backColor = "black");
            Builder& fillRow(int yPos, char symbol = ' ', const std::string& textColor = "white", const std::string& backColor = "black");
            Builder& fillCol(int xPos, char symbol = ' ', const std::string& textColor = "white", const std::string& backColor = "black");
            Builder& addBox(int xPos, int yPos, int xSize, int ySize, char symbol = ' ', const std::string& textColor = "white", const std::string& backColor = "black");
            
            // add a single pixel by color Id
            Builder& setPixelById(int xPos, int yPos, char symbol = ' ', const std::string& textColor = "white", const std::string& backColor = "black");
            
            // fill a zone of a texture by color Id
            Builder& fillTextureById(char symbol = ' ', const std::string& textColor = "7", const std::string& backColor = "0");
            Builder& fillRowById(int yPos, char symbol = ' ', const std::string& textColor = "7", const std::string& backColor = "0");
            Builder& fillColById(int xPos, char symbol = ' ', const std::string& textColor = "7", const std::string& backColor = "0");
            Builder& addBoxById(int xPos, int yPos, int xSize, int ySize, char symbol = ' ', const std::string& textColor = "7", const std::string& backColor = "0");

            // change size of a texture
            Builder& setSize(int xSize, int ySize);
            
            // set a specific grid to a texture
            Builder& setGrid(Grid grid);

            Texture build();
        };

        // Constructor
        Texture(Impl* pGrid);
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