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
    for (int i; i < grid.getGridSize(); i++) {
        original = grid.getPixelByIndex(i);
        unpacked = unpackedGrid.getPixelByIndex(i);

        EXPECT_EQ(unpacked.symbol, original.symbol);
        EXPECT_EQ(unpacked.textColor.b, original.textColor.b);
        EXPECT_EQ(unpacked.backColor.g, original.backColor.g);
    }
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

// test if texture out of range works
TEST(CanvasTest, TestAddTexErr) {
    Canvas canv(5, 5);

    Texture tex = Texture::Builder()
    .fillTexture(' ', Rgb(255,0,0), Rgb(0,0,0))
    .build();

    EXPECT_THROW(canv.addTexture(-1, 1, tex), std::out_of_range);
    EXPECT_THROW(canv.addTexture(1, -1, tex), std::out_of_range);

    EXPECT_THROW(canv.addTexture(0, 5, tex), std::out_of_range);
    EXPECT_THROW(canv.addTexture(0, 5, tex), std::out_of_range);
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
