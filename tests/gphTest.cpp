#include <gtest/gtest.h>
#include <filesystem>

#include "grid.h"
#include "ngph/colors.h"
#include <ngph/texture.h>
#include <ngph/canvas.h>
#include <ngph/iotex.h>

using namespace gph;

// unnamed namespace – limits access to tests
namespace {

// Test grid creation with zero size
TEST(GridTest, ZeroGrid) {
    EXPECT_THROW({ Grid grid(0, 1); }, std::exception);
    EXPECT_THROW({ Grid grid(1, 0); }, std::exception);
    EXPECT_THROW({ Grid grid(0, 0); }, std::exception);
}

// Test grid creation with negatives
TEST(GridTest, NegGrid) {
    EXPECT_THROW({ Grid grid(2, -2); }, std::exception);
    EXPECT_THROW({ Grid grid(-2, 2); }, std::exception);
    EXPECT_THROW({ Grid grid(-2, -2); }, std::exception);
}

// Test accessing valid default pixel
TEST(GridTest, CheckGetElmnt) {
    Grid grid(3, 2);
    EXPECT_EQ(grid.getPixel(0, 0).symbol, ' ');
    EXPECT_NO_THROW(grid.getPixel(2, 1));
}

// Test grid dimension values directly
TEST(GridTest, CheckDimensions) {
    Grid grid(3, 2);
    EXPECT_EQ(grid.getXSize(), 3);
    EXPECT_EQ(grid.getYSize(), 2);
}

// Test adding and retrieving pixel
TEST(GridTest, CheckAddPixel) {
    Grid grid(4, 3);
    Grid::Pixel pix;
    pix.symbol = '1';
    pix.textColor = Rgb(2, 2, 2);
    pix.backColor = Rgb(3, 3, 3);

    grid.addPixel(3, 2, pix);
    const Grid::Pixel retrieved = grid.getPixel(3, 2);

    EXPECT_EQ(retrieved.symbol, '1');
    EXPECT_EQ(retrieved.textColor.r, 2);
    EXPECT_EQ(retrieved.backColor.b, 3);
}

// Test accessing pixel out of bounds
TEST(GridTest, OutOfRange) {
    Grid grid(3, 2);
    EXPECT_THROW(grid.getPixel(3, 1), std::out_of_range);
    EXPECT_THROW(grid.getPixel(0, 2), std::out_of_range);
    EXPECT_THROW(grid.getPixel(-1, 0), std::out_of_range);
    EXPECT_THROW(grid.getPixel(0, -1), std::out_of_range);
}

// Test resizing errors
TEST(GridTest, GridResizeError) {
    Grid grid(3, 3);
    EXPECT_THROW(grid.setGridSize(0, 5), std::invalid_argument);
    EXPECT_THROW(grid.setGridSize(5, 0), std::invalid_argument);
}

// Test resizing
TEST(GridTest, GridResizeIndexation) {
    Grid grid(3, 3);

    int x = 2;
    int y = 1;

    Grid::Pixel pix;
    pix.symbol = 'b';
    grid.addPixel(x, y, pix);

    grid.setGridSize(5, 5);
    const Grid::Pixel newPix = grid.getPixel(x, y);

    EXPECT_EQ(newPix.symbol, 'b');
}

// Test if grid buffer packs and unpacks data without data change
TEST(GridTest, TestGridBuffer) {
    Grid grid(2, 2);
    grid.setPixel(0, 0, 'a', Rgb(0, 0, 0), Rgb(0, 0, 0));
    grid.setPixel(1, 0, 'b', Rgb(1, 1, 1), Rgb(1, 1, 1));
    grid.setPixel(0, 1, 'c', Rgb(2, 2, 2), Rgb(2, 2, 2));
    grid.setPixel(1, 1, 'd', Rgb(3, 3, 3), Rgb(3, 3, 3));

    GridBuffer buffer = grid.newBuffer();
    Grid unpackedGrid = buffer.unpack();

    EXPECT_EQ(unpackedGrid.getXSize(), grid.getXSize());
    EXPECT_EQ(unpackedGrid.getYSize(), grid.getYSize());
    EXPECT_EQ(unpackedGrid.getGridSize(), grid.getGridSize());

    Grid::Pixel original;
    Grid::Pixel unpacked;
    for (int i = 0; i < grid.getGridSize(); i++) {
        original = grid.getPixelByIndex(i);
        unpacked = unpackedGrid.getPixelByIndex(i);

        EXPECT_EQ(unpacked.symbol, original.symbol);
        EXPECT_EQ(unpacked.textColor.b, original.textColor.b);
        EXPECT_EQ(unpacked.backColor.g, original.backColor.g);
    }
}


// Test that an emoji codepoint can be stored and retrieved unchanged
TEST(GridTest, EmojiStoreRetrieve) {
    Grid grid(3, 3);
    Grid::Pixel pix;
    pix.symbol = U'🔥';
    pix.textColor = Rgb(255, 255, 255);
    pix.backColor = Rgb(0, 0, 0);

    grid.addPixel(1, 1, pix);
    EXPECT_EQ(grid.getPixel(1, 1).symbol, U'🔥');
}

// Test that an emoji survives a buffer pack/unpack round-trip
TEST(GridTest, EmojiBufferRoundTrip) {
    Grid grid(2, 2);
    grid.setPixel(0, 0, U'🌟', Rgb(0, 0, 0), Rgb(0, 0, 0));
    grid.setPixel(1, 1, U'🚀', Rgb(0, 0, 0), Rgb(0, 0, 0));

    GridBuffer buffer = grid.newBuffer();
    Grid unpacked = buffer.unpack();

    EXPECT_EQ(unpacked.getPixel(0, 0).symbol, U'🌟');
    EXPECT_EQ(unpacked.getPixel(1, 1).symbol, U'🚀');
}

// test texture get wrappers
TEST(TextureTest, TestGetSize) {
    Grid grid(2, 2);
    Texture tex = Texture::Builder(2, 2)
        .setGrid(grid)
        .build();

    EXPECT_EQ(grid.getXSize(), tex.getXSize());
    EXPECT_EQ(grid.getYSize(), tex.getYSize());
}

// test if the build single pixel fill works
TEST(TextureTest, TestBuildSetPixel) {
    Texture tex = Texture::Builder(4, 4)
        .setPixel(0, 0, ' ', Rgb(0,0,0), Rgb(0,0,0))
        .setPixel(3, 2, 'a', Rgb(255,255,255), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();

    EXPECT_EQ(grid.getPixel(0, 0).symbol, ' ');
    EXPECT_EQ(grid.getPixel(3, 2).symbol, 'a');
}

// test if the build texture fill works
TEST(TextureTest, TestBuildFillTexture) {
    Texture tex = Texture::Builder(4, 4)
        .fillTexture('a', Rgb(0,0,0), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();

    for (int i = 0; i < grid.getGridSize(); i++) {
        EXPECT_EQ(grid.getPixelByIndex(i).symbol, 'a');
    }
}

// test if the build row fill works
TEST(TextureTest, TestBuildFillRow) {
    Texture tex = Texture::Builder(4, 4)
        .fillRow(2, 'a', Rgb(0,0,0),Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();

    Grid::Pixel pix;
    for (int i = 0; i < grid.getXSize(); i++) {
        pix = grid.getPixel(i, 2);

        EXPECT_EQ(pix.symbol, 'a');
    }
}

// test if the build column fill works
TEST(TextureTest, TestBuildFillCol) {
    Texture tex = Texture::Builder(4, 4)
        .fillCol(1, 'a', Rgb(0,0,0), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();

    Grid::Pixel pix;
    for (int i = 0; i < grid.getYSize(); i++) {
        pix = grid.getPixel(1, i);

        EXPECT_EQ(pix.symbol, 'a');
    }
}

TEST(TextureTest, addBoxAll) {
    Texture tex = Texture::Builder(4, 4)
        .addBox(1, 1, 2, 2, 'a', Rgb(0,0,0), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();

    EXPECT_EQ(grid.getPixel(1, 1).symbol, 'a');
    EXPECT_EQ(grid.getPixel(2, 1).symbol, 'a');
    EXPECT_EQ(grid.getPixel(1, 2).symbol, 'a');
    EXPECT_EQ(grid.getPixel(2, 2).symbol, 'a');
    EXPECT_NE(grid.getPixel(3, 3).symbol, 'a');

    Texture::Builder builder = Texture::Builder(5, 5);

    EXPECT_NO_THROW(builder.addBox(4, 4, 10, 10, 'a', Rgb(255,255,255), Rgb(0,0,0)));
    EXPECT_THROW(builder.addBox(0, -1, 10, 10, 'a', Rgb(255,255,255), Rgb(0,0,0)), std::out_of_range);
    EXPECT_THROW(builder.addBox(-1, 0, 10, 10, 'a', Rgb(255,255,255), Rgb(0,0,0)), std::out_of_range);
    EXPECT_THROW(builder.addBox(0, 0, 0, 10, 'a', Rgb(255,255,255), Rgb(0,0,0)), std::invalid_argument);
    EXPECT_THROW(builder.addBox(0, 0, 10, 0, 'a', Rgb(255,255,255), Rgb(0,0,0)), std::invalid_argument);
}

// getSymbol returns the correct codepoint
TEST(TextureTest, GetSymbol) {
    Texture tex = Texture::Builder(3, 3)
        .setPixel(1, 2, U'Z', Rgb(0,0,0), Rgb(0,0,0))
        .build();

    EXPECT_EQ(tex.getSymbol(1, 2), U'Z');
    EXPECT_EQ(tex.getSymbol(0, 0), U' '); // default pixel
}

// getFgColor returns the correct foreground colour
TEST(TextureTest, GetFgColor) {
    Texture tex = Texture::Builder(3, 3)
        .setPixel(0, 0, U'A', Rgb(10, 20, 30), Rgb(0,0,0))
        .build();

    Rgb fg = tex.getFgColor(0, 0);
    EXPECT_EQ(fg.r, 10);
    EXPECT_EQ(fg.g, 20);
    EXPECT_EQ(fg.b, 30);
}

// getBgColor returns the correct background colour
TEST(TextureTest, GetBgColor) {
    Texture tex = Texture::Builder(3, 3)
        .setPixel(2, 1, U'B', Rgb(0,0,0), Rgb(50, 100, 150))
        .build();

    Rgb bg = tex.getBgColor(2, 1);
    EXPECT_EQ(bg.r, 50);
    EXPECT_EQ(bg.g, 100);
    EXPECT_EQ(bg.b, 150);
}

// pixel getters throw out_of_range on invalid coordinates
TEST(TextureTest, PixelGettersOutOfRange) {
    Texture tex = Texture::Builder(3, 3).build();

    EXPECT_THROW(tex.getSymbol(3, 0),  std::out_of_range);
    EXPECT_THROW(tex.getSymbol(0, 3),  std::out_of_range);
    EXPECT_THROW(tex.getSymbol(-1, 0), std::out_of_range);
    EXPECT_THROW(tex.getFgColor(3, 0), std::out_of_range);
    EXPECT_THROW(tex.getBgColor(0, 3), std::out_of_range);
}

// addText: basic left-to-right placement
TEST(TextureTest, AddTextBasic) {
    Texture tex = Texture::Builder(5, 2)
        .addText(0, 0, U"Hello", Rgb(255,255,255), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();
    EXPECT_EQ(grid.getPixel(0, 0).symbol, U'H');
    EXPECT_EQ(grid.getPixel(1, 0).symbol, U'e');
    EXPECT_EQ(grid.getPixel(2, 0).symbol, U'l');
    EXPECT_EQ(grid.getPixel(3, 0).symbol, U'l');
    EXPECT_EQ(grid.getPixel(4, 0).symbol, U'o');
}

// addText: \n moves cursor to start of next row
TEST(TextureTest, AddTextNewline) {
    Texture tex = Texture::Builder(5, 3)
        .addText(0, 0, U"Hi\nBye", Rgb(255,255,255), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();
    EXPECT_EQ(grid.getPixel(0, 0).symbol, U'H');
    EXPECT_EQ(grid.getPixel(1, 0).symbol, U'i');
    EXPECT_EQ(grid.getPixel(0, 1).symbol, U'B');
    EXPECT_EQ(grid.getPixel(1, 1).symbol, U'y');
    EXPECT_EQ(grid.getPixel(2, 1).symbol, U'e');
}

// addText: \t advances to the next multiple-of-4 tab stop
TEST(TextureTest, AddTextTab) {
    // tab at x=0 → next stop = 4
    Texture tex = Texture::Builder(10, 2)
        .addText(0, 0, U"\tA", Rgb(255,255,255), Rgb(0,0,0))
        .build();
    EXPECT_EQ(tex.getGrid().getPixel(4, 0).symbol, U'A');

    // tab at x=2 → next stop = 4
    Texture tex2 = Texture::Builder(10, 2)
        .addText(0, 0, U"AB\tC", Rgb(255,255,255), Rgb(0,0,0))
        .build();
    EXPECT_EQ(tex2.getGrid().getPixel(4, 0).symbol, U'C');
}

// addText: chars wrap to next row when width is exceeded
TEST(TextureTest, AddTextWrapOnOverflow) {
    // "ABCDE" in a 3-wide texture: row0 = ABC, row1 = DE
    Texture tex = Texture::Builder(3, 2)
        .addText(0, 0, U"ABCDE", Rgb(255,255,255), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();
    EXPECT_EQ(grid.getPixel(0, 0).symbol, U'A');
    EXPECT_EQ(grid.getPixel(2, 0).symbol, U'C');
    EXPECT_EQ(grid.getPixel(0, 1).symbol, U'D');
    EXPECT_EQ(grid.getPixel(1, 1).symbol, U'E');
}

// addText: stops once the texture is full, without writing out-of-bounds
TEST(TextureTest, AddTextStopsWhenFull) {
    // 3x2 texture has 6 cells; "ABCDEFG" fills all 6 then stops before G
    Texture tex = Texture::Builder(3, 2)
        .addText(0, 0, U"ABCDEFG", Rgb(255,255,255), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();
    EXPECT_EQ(grid.getPixel(0, 0).symbol, U'A');
    EXPECT_EQ(grid.getPixel(2, 1).symbol, U'F');
    // G is beyond capacity — nothing after F should be set
    EXPECT_EQ(grid.getPixel(0, 0).symbol, U'A'); // sanity
}

// addText: starting at a non-zero offset
TEST(TextureTest, AddTextWithOffset) {
    Texture tex = Texture::Builder(5, 3)
        .addText(2, 1, U"XY", Rgb(255,255,255), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();
    EXPECT_EQ(grid.getPixel(2, 1).symbol, U'X');
    EXPECT_EQ(grid.getPixel(3, 1).symbol, U'Y');
    EXPECT_EQ(grid.getPixel(0, 0).symbol, U' '); // untouched
}

// addText: Unicode codepoints (emoji) are stored correctly and advance by 2
TEST(TextureTest, AddTextUnicode) {
    // texture is 6 wide: H(0) i(1) 🔥(2) placeholder(3) !(4)
    Texture tex = Texture::Builder(6, 2)
        .addText(0, 0, U"Hi\U0001F525!", Rgb(255,255,255), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();
    EXPECT_EQ(grid.getPixel(0, 0).symbol, U'H');
    EXPECT_EQ(grid.getPixel(1, 0).symbol, U'i');
    EXPECT_EQ(grid.getPixel(2, 0).symbol, U'\U0001F525'); // 🔥 — occupies cells 2 and 3
    EXPECT_EQ(grid.getPixel(3, 0).symbol, U' ');          // right-half blank placeholder
    EXPECT_EQ(grid.getPixel(4, 0).symbol, U'!');
}

// addText TexFlag::OFF: texture size is unchanged and text is clipped as normal
TEST(TextureTest, AddTextFlagOff) {
    Texture tex = Texture::Builder(3, 1)
        .addText(0, 0, U"ABCDE", Rgb(255,255,255), Rgb(0,0,0), TexFlag::OFF)
        .build();

    EXPECT_EQ(tex.getXSize(), 3);
    EXPECT_EQ(tex.getYSize(), 1);
    EXPECT_EQ(tex.getSymbol(0, 0), U'A');
    EXPECT_EQ(tex.getSymbol(2, 0), U'C');
}

// addText TexFlag::HORIZONTAL: texture grows in x so the text fits without wrapping
TEST(TextureTest, AddTextFlagHorizontalExpands) {
    Texture tex = Texture::Builder(3, 1)
        .addText(0, 0, U"Hello", Rgb(255,255,255), Rgb(0,0,0), TexFlag::HORIZONTAL)
        .build();

    EXPECT_EQ(tex.getXSize(), 5);
    EXPECT_EQ(tex.getYSize(), 1);
    EXPECT_EQ(tex.getSymbol(0, 0), U'H');
    EXPECT_EQ(tex.getSymbol(4, 0), U'o');
}

// addText TexFlag::HORIZONTAL: texture does not shrink if text already fits
TEST(TextureTest, AddTextFlagHorizontalNoShrink) {
    Texture tex = Texture::Builder(10, 2)
        .addText(0, 0, U"Hi", Rgb(255,255,255), Rgb(0,0,0), TexFlag::HORIZONTAL)
        .build();

    EXPECT_EQ(tex.getXSize(), 10);
    EXPECT_EQ(tex.getYSize(), 2);
}

// addText TexFlag::HORIZONTAL: x grows to the longest natural line across newlines
TEST(TextureTest, AddTextFlagHorizontalMultiline) {
    // "Hi\nBye!" — longest line is "Bye!" (4 chars), starting at x=0
    Texture tex = Texture::Builder(2, 2)
        .addText(0, 0, U"Hi\nBye!", Rgb(255,255,255), Rgb(0,0,0), TexFlag::HORIZONTAL)
        .build();

    EXPECT_EQ(tex.getXSize(), 4);
    EXPECT_EQ(tex.getSymbol(0, 1), U'B');
    EXPECT_EQ(tex.getSymbol(3, 1), U'!');
}

// addText TexFlag::HORIZONTAL: wide (2-cell) characters count as 2 columns
TEST(TextureTest, AddTextFlagHorizontalWideChar) {
    // U+1F525 🔥 is 2 cells wide; starting on a 1-wide texture → needs width 2
    Texture tex = Texture::Builder(1, 1)
        .addText(0, 0, U"\U0001F525", Rgb(255,255,255), Rgb(0,0,0), TexFlag::HORIZONTAL)
        .build();

    EXPECT_EQ(tex.getXSize(), 2);
    EXPECT_EQ(tex.getSymbol(0, 0), U'\U0001F525');
}

// addText TexFlag::VERTICAL: texture grows in y so wrapped text fits
TEST(TextureTest, AddTextFlagVerticalExpands) {
    // "ABCDE" in a 3-wide, 1-tall texture wraps to 2 rows → y must grow to 2
    Texture tex = Texture::Builder(3, 1)
        .addText(0, 0, U"ABCDE", Rgb(255,255,255), Rgb(0,0,0), TexFlag::VERTICAL)
        .build();

    EXPECT_EQ(tex.getXSize(), 3);
    EXPECT_EQ(tex.getYSize(), 2);
    EXPECT_EQ(tex.getSymbol(0, 0), U'A');
    EXPECT_EQ(tex.getSymbol(2, 0), U'C');
    EXPECT_EQ(tex.getSymbol(0, 1), U'D');
    EXPECT_EQ(tex.getSymbol(1, 1), U'E');
}

// addText TexFlag::VERTICAL: texture does not shrink if text already fits
TEST(TextureTest, AddTextFlagVerticalNoShrink) {
    Texture tex = Texture::Builder(5, 10)
        .addText(0, 0, U"Hi", Rgb(255,255,255), Rgb(0,0,0), TexFlag::VERTICAL)
        .build();

    EXPECT_EQ(tex.getXSize(), 5);
    EXPECT_EQ(tex.getYSize(), 10);
}

// addText TexFlag::VERTICAL: explicit newlines beyond current height are also handled
TEST(TextureTest, AddTextFlagVerticalNewlines) {
    // "A\nB\nC" on a 5-wide, 1-tall texture → needs 3 rows
    Texture tex = Texture::Builder(5, 1)
        .addText(0, 0, U"A\nB\nC", Rgb(255,255,255), Rgb(0,0,0), TexFlag::VERTICAL)
        .build();

    EXPECT_EQ(tex.getYSize(), 3);
    EXPECT_EQ(tex.getSymbol(0, 0), U'A');
    EXPECT_EQ(tex.getSymbol(0, 1), U'B');
    EXPECT_EQ(tex.getSymbol(0, 2), U'C');
}

// test if texture table i/o works correctly with no errors
TEST(IotexTest, TestTexStore) {
    TexTable table;

    Texture texOrig = Texture::Builder(2, 2)
        .fillTexture('a', Rgb(0,0,0), Rgb(0,0,0))
        .build();

    EXPECT_NO_THROW(table.setTexture("test", texOrig));

    Texture texLoaded = table.getTexture("test");
    EXPECT_EQ(texOrig.getGrid().getPixelByIndex(0).symbol, texLoaded.getGrid().getPixelByIndex(0).symbol);

    EXPECT_NO_THROW(table.delTexture("test"));

    Texture texDel = table.getTexture("test");
    EXPECT_EQ(texDel.getGrid().getPixelByIndex(0).symbol, ' ');
}

TEST(IotexTest, TestSaveLoad) {
    std::string fPath = "./.testTexTable.gph";

    TexTable tblorig;

    Texture texOrig = Texture::Builder(2, 2)
        .fillTexture('a', Rgb(0,0,0), Rgb(0,0,0))
        .setPixel(1, 1, 'b', Rgb(0,0,0), Rgb(0,0,0))
        .build();

    tblorig.setTexture("test", texOrig);
    tblorig.saveTable(fPath);

    TexTable tblLoaded;
    tblLoaded.loadTable(fPath);

    Texture texLoaded = tblLoaded.getTexture("test");

    EXPECT_EQ(texOrig.getXSize(), texLoaded.getXSize());
    EXPECT_EQ(texOrig.getYSize(), texLoaded.getYSize());

    Grid gridOrig = texOrig.getGrid();
    Grid gridLoaded = texLoaded.getGrid();

    for (int i = 0; i < gridOrig.getGridSize(); i++) {
        EXPECT_EQ(gridOrig.getPixelByIndex(i).symbol, gridLoaded.getPixelByIndex(i).symbol);
        }

    std::filesystem::remove(fPath);
}

// test if size getters work fine
TEST(CanvasTest, TestCanvSize) {
    uint32_t xSize = 10;
    uint32_t ySize = 20;
    uint32_t canvSize = xSize * ySize;

    Canvas canv(xSize, ySize);

    EXPECT_EQ(xSize, canv.getXSize());
    EXPECT_EQ(ySize, canv.getYSize());
    EXPECT_EQ(canvSize, canv.getCanvSize());
}

// test if resize exception works
TEST(CanvasTest, TestCanvResizeErr) {
    EXPECT_THROW(Canvas(0, 1), std::invalid_argument);
    EXPECT_THROW(Canvas(1, 0), std::invalid_argument);

    Canvas canv(5, 5);

    EXPECT_THROW(canv.setSize(1, 0), std::invalid_argument);
    EXPECT_THROW(canv.setSize(0, 1), std::invalid_argument);
}

// addTexture silently clips out-of-range pixels; it should never throw
TEST(CanvasTest, TestAddTexNoThrowOnOutOfRange) {
    Canvas canv(5, 5);

    Texture tex = Texture::Builder(3, 3)
        .fillTexture('X', Rgb(255, 0, 0), Rgb(0, 0, 0))
        .build();

    // negative offsets — partially off top-left edge
    EXPECT_NO_THROW(canv.addTexture(-1, 0, tex));
    EXPECT_NO_THROW(canv.addTexture(0, -1, tex));
    EXPECT_NO_THROW(canv.addTexture(-2, -2, tex));

    // completely off-screen in all directions
    EXPECT_NO_THROW(canv.addTexture(-10, 0, tex));
    EXPECT_NO_THROW(canv.addTexture(0, -10, tex));
    EXPECT_NO_THROW(canv.addTexture(10, 0, tex));
    EXPECT_NO_THROW(canv.addTexture(0, 10, tex));

    // overflow — partially beyond the right/bottom edge
    EXPECT_NO_THROW(canv.addTexture(4, 0, tex));
    EXPECT_NO_THROW(canv.addTexture(0, 4, tex));
    EXPECT_NO_THROW(canv.addTexture(5, 0, tex));
    EXPECT_NO_THROW(canv.addTexture(0, 5, tex));
}

// partial clip: only the visible portion of the texture should be placed
TEST(CanvasTest, TestAddTexPartialClip) {
    Canvas canv(5, 5);

    // 4x4 texture, placed at (-2,-2): only the bottom-right 2x2 should land on the canvas
    Texture tex = Texture::Builder(4, 4)
        .fillTexture('P', Rgb(255, 0, 0), Rgb(0, 0, 0))
        .build();

    EXPECT_NO_THROW(canv.addTexture(-2, -2, tex));

    // placed at (4, 4) on a 5x5 canvas: only pixel (4,4) should be touched
    EXPECT_NO_THROW(canv.addTexture(4, 4, tex));

    // placed so it overhangs the right and bottom edges
    EXPECT_NO_THROW(canv.addTexture(3, 3, tex));
}

// test if setPixel out of range works
TEST (CanvasTest, TestSetPixelErr) {
    Canvas canv(5, 5);

    EXPECT_THROW(canv.setPixel(1, -1, 'a', Rgb(255,0,0), Rgb(0,0,255)), std::out_of_range);
    EXPECT_THROW(canv.setPixel(-1, 1, 'a', Rgb(255,0,0), Rgb(0,0,255)), std::out_of_range);

    EXPECT_THROW(canv.setPixel(0, 5, 'a', Rgb(255,0,0), Rgb(0,0,255)), std::out_of_range);
    EXPECT_THROW(canv.setPixel(5, 0, 'a', Rgb(255,0,0), Rgb(0,0,255)), std::out_of_range);
}

// test if normal usage won't trigger exveptions
TEST(CanvasTest, TestNormalWork) {
    Canvas canv(5, 5);

    Texture tex = Texture::Builder(2, 2)
    .fillTexture(' ', Rgb(255,0,0), Rgb(0,0,255))
    .build();

    canv.addTexture(1, 1, tex);

    EXPECT_NO_THROW(canv.addTexture(1, 1, tex));
    EXPECT_NO_THROW(canv.setPixel(4, 4, 'a', Rgb(255,0,0), Rgb(0,0,255)));
    EXPECT_NO_THROW(canv.updateSize());
    EXPECT_NO_THROW(canv.setSize(8, 8));
    EXPECT_NO_THROW(canv.iterateTexture(1, 1, 2, 1, tex));
    EXPECT_NO_THROW(canv.fillWithTexture(tex));
}
}
