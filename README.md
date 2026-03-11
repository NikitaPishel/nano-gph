# **Nano-Gph**

> [Intro](#intro)

> [Installation](#installation)

> [Manual](#short-manual)

> [Contribution](#contribution-)

> [License](/LICENSE.md)

---

## Intro
Nano gph is a minimal C++ CLI graphics interface, that can be used to build a broad variety of CLI applications.

### Features
- Efficient differential rendering
- Automatic screen scaling
- Texture handling
- Saving texture packs

![screenshot](./readmeFiles/screenshot-1.png?width=600)

---

## **Installation**
**Important Notice:** this project supports only linux

cmake:
```cmake
FetchContent_Declare(
    ngph
    GIT_REPOSITORY https://github.com/NikitaPishel/nano-gph
    GIT_TAG <version>
)

FetchContent_MakeAvailable(ngph)
```

---

## **Short Manual**
### Headers:
- `<ngph/canvas.h>`
- `<ngph/texture.h>`
- `<ngph/iotex.h>`
- `<ngph/colors.h>`

![screenshot](./readmeFiles/screenshot-2.png?width=380)

### Key info
Namespace used by this library is `gph`. When building a large project, consider that to avoid integration issues.
Library features main class `Canvas` that handles input into it via textures, accordingly are used as a `Texture` class
It uses `(0, 0)` position as origin (top-left corner)
All method parameters can be found it API (headers)

All interfaces is encapsulated, so there should not appear any issues with accidental triggering of non-API logic

### Canvas
To include canvas, use header `<ngph/canvas>`

#### Size i/o:
```cpp
uint32_t getXSize() const;
uint32_t getYSize() const;
uint32_t getCanvSize() const;
void setSize(int xSize, int ySize);
bool updateSize();
```

- `getXSize()` - returns horizontal size of a canvas
- `getYSize()` - returns vetical size of a canvas
- `getCanvSize()` - returns amount of pixels/formatted characters in a canvas
- `setSize(xSize, ySize)` - sets size of a canvas to the provided dimenions
- `updateSize()` - automatically stretches canvas to fill the entire terminal. When canvas was resized, returns true

#### editing canvas and displaying image:
```cpp
void setPixel(int xPos, int yPos, char32_t symbol, Rgb textColor, Rgb backColor);
void addTexture(int xPos, int yPos, const Texture& newTex);
void iterateTexture(int xPos, int yPos, int xSize, int ySize, const Texture& newTex);
void fillWithTexture(const Texture& newTex);

void render();
```

- `setPixel(...)` - sets a pixel to the provided formatted version: which character is it, its color and background color
- `addTexture(...)` - adds a given texture to the canvas. Pixels that fall outside the canvas boundaries are clipped
- `iterateTexture(...)` - iterates texture a few times. For example, if `xSize = 2` and `ySize = 3`, then this method will add 2 columns of textures in 3 rows
- `fillWithTexture(...)` - automatically fills the whole canvas with the same texture
- `render()` - display canvas on a screen.

###  Texture

#### TexFlag
```cpp
enum class TexFlag : uint8_t {
    OFF = 0,
    HORIZONTAL,
    VERTICAL
};
```

`TexFlag` controls automatic texture scaling in `addText`:
- `OFF` - no scaling; text that exceeds the texture boundaries is clipped
- `HORIZONTAL` - the texture width expands to fit the text without wrapping
- `VERTICAL` - the texture height expands to accommodate wrapped text at the current width

#### Texture::Builder methods
- `Builder(xSize, ySize)` - Constructor: Starts building a texture with the given dimensions.
- `getXSize()` - Returns the current horizontal dimension of the texture being built.
- `getYSize()` - Returns the current vertical dimension of the texture being built.
- `setSize(xSize, ySize)` - Changes the dimensions of the texture being built.
- `setPixel(xPos, yPos, symbol, textColor, backColor)` - Sets a single pixel at a specific coordinate using `Rgb` colors.
- `setPixelByGridIndex(index, symbol, textColor, backColor)` - Same as `setPixel`, but uses a raw grid index instead of coordinates.
- `fillTexture(...)` - Fills the entire texture with the same symbol and colors.
- `fillRow(yPos, ...)` - Fills a single horizontal row (`yPos`) with the specified pixel.
- `fillCol(xPos, ...)` - Fills a single vertical column (`xPos`) with the specified pixel.
- `addBox(xPos, yPos, xSize, ySize, ...)` - Draws a rectangular box starting at `(xPos, yPos)` with the given size and pixel content.
- `addText(xPos, yPos, text, textColor, backColor, flag = TexFlag::OFF)` - Renders a UTF-32 string onto the texture starting at `(xPos, yPos)`. Supports `\n` (moves to the start of the next row), `\t` (advances to the next multiple-of-4 tab stop), and `\r` (moves to the start of the current row). Wide characters (e.g. emoji) occupy two columns. Text that exceeds the texture width wraps to the next row; writing stops when the texture is full. When `flag` is `HORIZONTAL`, the texture auto-expands its width to fit the text. When `flag` is `VERTICAL`, the texture auto-expands its height to accommodate wrapped lines.
- `addTexture(xPos, yPos, newTex)` - Copies another `Texture` (`newTex`) onto the current texture at `(xPos, yPos)`.
- `fillWithTexture(newTex)` - Tiles the `newTex` across the entire current texture.
- `build()` - Finalizes the creation, consumes the `Builder`, and returns the immutable `Texture` object. Note that the builder itself is an ephemeral object, and can't be used once the texture is built.
- `snapshot()` - Creates an independent owned copy of the texture from the current builder state without consuming the `Builder`. Unlike `build()`, the builder remains usable after calling `snapshot()`.

#### Texture methods
- `Texture(int xSize = 1, int ySize = 1)` - Creates a new `Texture` object with the specified dimensions, ready to be used.
- `~Texture()` - Destructor: Cleans up the internal grid implementation if the `Texture` object owns it.
- `getXSize() const` - Returns the horizontal size of the texture.
- `getYSize() const` - Returns the vertical size of the texture.
- `getGrid() const` - Returns a constant reference to the internal `Grid` object (used mainly by the `Canvas` for rendering).

Texture is **immutable**, so it can't be edited after creation. You can only read its size, and buffer it into a serialized binary.

### TexTable
To include the texture table class, use header `<ngph/iotex>`

The `TexTable` class is an utility for managing and persistently storing a collection of `Texture` objects. It allows you to load and save textures to a file and retrieve them by name.

#### TexTable Methods:
```cpp
// basic texture map operations
Texture getTexture(const std::string& texName) const;
const Texture& getTextureRef(const std::string& texName) const;
void setTexture(const std::string& texName, Texture texture);
void delTexture(const std::string& texName);

// Work with binary to save and load textures
void loadTable(const std::string& path);
void saveTable(const std::string& path);
```

- `TexTable()` - Constructor: Creates an empty texture table.
- `getTexture(const std::string& texName) const` - Returns a **copy** of the texture associated with `texName`.
- `getTextureRef(const std::string& texName) const` - Returns a **constant reference** to the texture associated with `texName`. Use this to avoid copying.
- `setTexture(const std::string& texName, Texture texture)` - Stores the provided `texture` under the given `texName` (overwrites if the name exists).
- `delTexture(const std::string& texName)` - Removes the texture associated with `texName` from the table.
- `loadTable(const std::string& path)` - Reads a binary file, deserializes the data, and loads the contained textures into the table.
- `saveTable(const std::string& path)` - Serializes the current textures in the table and writes the binary data to the file specified by `path`.

### Colors
To include the color utility, use header `<ngph/colors>`

The `Rgb` struct is used to represent colors throughout the library. It is passed directly to pixel-setting methods on both `Canvas` and `Texture::Builder`.

#### Rgb
```cpp
struct Rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    Rgb(uint8_t r=0, uint8_t g=0, uint8_t b=0);
};
```

- `Rgb(r, g, b)` - Constructor: Creates a color from red, green, and blue channel values (0–255).

---

## **Contribution**
This project does not accept any contribution.
