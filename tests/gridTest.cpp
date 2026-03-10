#include <gtest/gtest.h>

#include "grid.h"
#include "ngph/colors.h"

using namespace gph;

namespace {

TEST(GridTest, ZeroGrid) {
    EXPECT_THROW({ Grid grid(0, 1); }, std::exception);
    EXPECT_THROW({ Grid grid(1, 0); }, std::exception);
    EXPECT_THROW({ Grid grid(0, 0); }, std::exception);
}

TEST(GridTest, NegGrid) {
    EXPECT_THROW({ Grid grid(2, -2); }, std::exception);
    EXPECT_THROW({ Grid grid(-2, 2); }, std::exception);
    EXPECT_THROW({ Grid grid(-2, -2); }, std::exception);
}

TEST(GridTest, CheckGetElmnt) {
    Grid grid(3, 2);
    EXPECT_EQ(grid.getPixel(0, 0).symbol, ' ');
    EXPECT_NO_THROW(grid.getPixel(2, 1));
}

TEST(GridTest, CheckDimensions) {
    Grid grid(3, 2);
    EXPECT_EQ(grid.getXSize(), 3);
    EXPECT_EQ(grid.getYSize(), 2);
}

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

TEST(GridTest, OutOfRange) {
    Grid grid(3, 2);
    EXPECT_THROW(grid.getPixel(3, 1), std::out_of_range);
    EXPECT_THROW(grid.getPixel(0, 2), std::out_of_range);
    EXPECT_THROW(grid.getPixel(-1, 0), std::out_of_range);
    EXPECT_THROW(grid.getPixel(0, -1), std::out_of_range);
}

TEST(GridTest, GridResizeError) {
    Grid grid(3, 3);
    EXPECT_THROW(grid.setGridSize(0, 5), std::invalid_argument);
    EXPECT_THROW(grid.setGridSize(5, 0), std::invalid_argument);
}

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

TEST(GridTest, EmojiStoreRetrieve) {
    Grid grid(3, 3);
    Grid::Pixel pix;
    pix.symbol = U'🔥';
    pix.textColor = Rgb(255, 255, 255);
    pix.backColor = Rgb(0, 0, 0);

    grid.addPixel(1, 1, pix);
    EXPECT_EQ(grid.getPixel(1, 1).symbol, U'🔥');
}

TEST(GridTest, EmojiBufferRoundTrip) {
    Grid grid(2, 2);
    grid.setPixel(0, 0, U'🌟', Rgb(0, 0, 0), Rgb(0, 0, 0));
    grid.setPixel(1, 1, U'🚀', Rgb(0, 0, 0), Rgb(0, 0, 0));

    GridBuffer buffer = grid.newBuffer();
    Grid unpacked = buffer.unpack();

    EXPECT_EQ(unpacked.getPixel(0, 0).symbol, U'🌟');
    EXPECT_EQ(unpacked.getPixel(1, 1).symbol, U'🚀');
}

}
