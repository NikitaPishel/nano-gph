#include <gtest/gtest.h>

#include "ngph/colors.h"
#include <ngph/texture.h>
#include <ngph/canvas.h>

using namespace gph;

namespace {

TEST(CanvasTest, TestCanvSize) {
    uint32_t xSize = 10;
    uint32_t ySize = 20;
    uint32_t canvSize = xSize * ySize;

    Canvas canv(xSize, ySize);

    EXPECT_EQ(xSize, canv.getXSize());
    EXPECT_EQ(ySize, canv.getYSize());
    EXPECT_EQ(canvSize, canv.getCanvSize());
}

TEST(CanvasTest, TestCanvResizeErr) {
    EXPECT_THROW(Canvas(0, 1), std::invalid_argument);
    EXPECT_THROW(Canvas(1, 0), std::invalid_argument);

    Canvas canv(5, 5);

    EXPECT_THROW(canv.setSize(1, 0), std::invalid_argument);
    EXPECT_THROW(canv.setSize(0, 1), std::invalid_argument);
}

TEST(CanvasTest, TestAddTexNoThrowOnOutOfRange) {
    Canvas canv(5, 5);

    Texture tex = Texture::Builder(3, 3)
        .fillTexture('X', Rgb(255, 0, 0), Rgb(0, 0, 0))
        .build();

    EXPECT_NO_THROW(canv.addTexture(-1, 0, tex));
    EXPECT_NO_THROW(canv.addTexture(0, -1, tex));
    EXPECT_NO_THROW(canv.addTexture(-2, -2, tex));

    EXPECT_NO_THROW(canv.addTexture(-10, 0, tex));
    EXPECT_NO_THROW(canv.addTexture(0, -10, tex));
    EXPECT_NO_THROW(canv.addTexture(10, 0, tex));
    EXPECT_NO_THROW(canv.addTexture(0, 10, tex));

    EXPECT_NO_THROW(canv.addTexture(4, 0, tex));
    EXPECT_NO_THROW(canv.addTexture(0, 4, tex));
    EXPECT_NO_THROW(canv.addTexture(5, 0, tex));
    EXPECT_NO_THROW(canv.addTexture(0, 5, tex));
}

TEST(CanvasTest, TestAddTexPartialClip) {
    Canvas canv(5, 5);

    Texture tex = Texture::Builder(4, 4)
        .fillTexture('P', Rgb(255, 0, 0), Rgb(0, 0, 0))
        .build();

    EXPECT_NO_THROW(canv.addTexture(-2, -2, tex));

    EXPECT_NO_THROW(canv.addTexture(4, 4, tex));

    EXPECT_NO_THROW(canv.addTexture(3, 3, tex));
}

TEST (CanvasTest, TestSetPixelErr) {
    Canvas canv(5, 5);

    EXPECT_THROW(canv.setPixel(1, -1, 'a', Rgb(255,0,0), Rgb(0,0,255)), std::out_of_range);
    EXPECT_THROW(canv.setPixel(-1, 1, 'a', Rgb(255,0,0), Rgb(0,0,255)), std::out_of_range);

    EXPECT_THROW(canv.setPixel(0, 5, 'a', Rgb(255,0,0), Rgb(0,0,255)), std::out_of_range);
    EXPECT_THROW(canv.setPixel(5, 0, 'a', Rgb(255,0,0), Rgb(0,0,255)), std::out_of_range);
}

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
