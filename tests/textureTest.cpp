#include <gtest/gtest.h>

#include "grid.h"
#include "ngph/colors.h"
#include <ngph/texture.h>

using namespace gph;

namespace {

TEST(TextureTest, TestGetSize) {
    Grid grid(2, 2);
    Texture tex = Texture::Builder(2, 2)
        .setGrid(grid)
        .build();

    EXPECT_EQ(grid.getXSize(), tex.getXSize());
    EXPECT_EQ(grid.getYSize(), tex.getYSize());
}

TEST(TextureTest, TestBuildSetPixel) {
    Texture tex = Texture::Builder(4, 4)
        .setPixel(0, 0, ' ', Rgb(0,0,0), Rgb(0,0,0))
        .setPixel(3, 2, 'a', Rgb(255,255,255), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();

    EXPECT_EQ(grid.getPixel(0, 0).symbol, ' ');
    EXPECT_EQ(grid.getPixel(3, 2).symbol, 'a');
}

TEST(TextureTest, TestBuildFillTexture) {
    Texture tex = Texture::Builder(4, 4)
        .fillTexture('a', Rgb(0,0,0), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();

    for (int i = 0; i < grid.getGridSize(); i++) {
        EXPECT_EQ(grid.getPixelByIndex(i).symbol, 'a');
    }
}

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

TEST(TextureTest, GetSymbol) {
    Texture tex = Texture::Builder(3, 3)
        .setPixel(1, 2, U'Z', Rgb(0,0,0), Rgb(0,0,0))
        .build();

    EXPECT_EQ(tex.getSymbol(1, 2), U'Z');
    EXPECT_EQ(tex.getSymbol(0, 0), U' ');
}

TEST(TextureTest, GetFgColor) {
    Texture tex = Texture::Builder(3, 3)
        .setPixel(0, 0, U'A', Rgb(10, 20, 30), Rgb(0,0,0))
        .build();

    Rgb fg = tex.getFgColor(0, 0);
    EXPECT_EQ(fg.r, 10);
    EXPECT_EQ(fg.g, 20);
    EXPECT_EQ(fg.b, 30);
}

TEST(TextureTest, GetBgColor) {
    Texture tex = Texture::Builder(3, 3)
        .setPixel(2, 1, U'B', Rgb(0,0,0), Rgb(50, 100, 150))
        .build();

    Rgb bg = tex.getBgColor(2, 1);
    EXPECT_EQ(bg.r, 50);
    EXPECT_EQ(bg.g, 100);
    EXPECT_EQ(bg.b, 150);
}

TEST(TextureTest, PixelGettersOutOfRange) {
    Texture tex = Texture::Builder(3, 3).build();

    EXPECT_THROW(tex.getSymbol(3, 0),  std::out_of_range);
    EXPECT_THROW(tex.getSymbol(0, 3),  std::out_of_range);
    EXPECT_THROW(tex.getSymbol(-1, 0), std::out_of_range);
    EXPECT_THROW(tex.getFgColor(3, 0), std::out_of_range);
    EXPECT_THROW(tex.getBgColor(0, 3), std::out_of_range);
}

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

TEST(TextureTest, AddTextTab) {
    Texture tex = Texture::Builder(10, 2)
        .addText(0, 0, U"\tA", Rgb(255,255,255), Rgb(0,0,0))
        .build();
    EXPECT_EQ(tex.getGrid().getPixel(4, 0).symbol, U'A');

    Texture tex2 = Texture::Builder(10, 2)
        .addText(0, 0, U"AB\tC", Rgb(255,255,255), Rgb(0,0,0))
        .build();
    EXPECT_EQ(tex2.getGrid().getPixel(4, 0).symbol, U'C');
}

TEST(TextureTest, AddTextWrapOnOverflow) {
    Texture tex = Texture::Builder(3, 2)
        .addText(0, 0, U"ABCDE", Rgb(255,255,255), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();
    EXPECT_EQ(grid.getPixel(0, 0).symbol, U'A');
    EXPECT_EQ(grid.getPixel(2, 0).symbol, U'C');
    EXPECT_EQ(grid.getPixel(0, 1).symbol, U'D');
    EXPECT_EQ(grid.getPixel(1, 1).symbol, U'E');
}

TEST(TextureTest, AddTextStopsWhenFull) {
    Texture tex = Texture::Builder(3, 2)
        .addText(0, 0, U"ABCDEFG", Rgb(255,255,255), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();
    EXPECT_EQ(grid.getPixel(0, 0).symbol, U'A');
    EXPECT_EQ(grid.getPixel(2, 1).symbol, U'F');
    EXPECT_EQ(grid.getPixel(0, 0).symbol, U'A');
}

TEST(TextureTest, AddTextWithOffset) {
    Texture tex = Texture::Builder(5, 3)
        .addText(2, 1, U"XY", Rgb(255,255,255), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();
    EXPECT_EQ(grid.getPixel(2, 1).symbol, U'X');
    EXPECT_EQ(grid.getPixel(3, 1).symbol, U'Y');
    EXPECT_EQ(grid.getPixel(0, 0).symbol, U' ');
}

TEST(TextureTest, AddTextUnicode) {
    Texture tex = Texture::Builder(6, 2)
        .addText(0, 0, U"Hi\U0001F525!", Rgb(255,255,255), Rgb(0,0,0))
        .build();

    Grid grid = tex.getGrid();
    EXPECT_EQ(grid.getPixel(0, 0).symbol, U'H');
    EXPECT_EQ(grid.getPixel(1, 0).symbol, U'i');
    EXPECT_EQ(grid.getPixel(2, 0).symbol, U'\U0001F525');
    EXPECT_EQ(grid.getPixel(3, 0).symbol, U' ');
    EXPECT_EQ(grid.getPixel(4, 0).symbol, U'!');
}

TEST(TextureTest, AddTextFlagOff) {
    Texture tex = Texture::Builder(3, 1)
        .addText(0, 0, U"ABCDE", Rgb(255,255,255), Rgb(0,0,0), TexFlag::OFF)
        .build();

    EXPECT_EQ(tex.getXSize(), 3);
    EXPECT_EQ(tex.getYSize(), 1);
    EXPECT_EQ(tex.getSymbol(0, 0), U'A');
    EXPECT_EQ(tex.getSymbol(2, 0), U'C');
}

TEST(TextureTest, AddTextFlagHorizontalExpands) {
    Texture tex = Texture::Builder(3, 1)
        .addText(0, 0, U"Hello", Rgb(255,255,255), Rgb(0,0,0), TexFlag::HORIZONTAL)
        .build();

    EXPECT_EQ(tex.getXSize(), 5);
    EXPECT_EQ(tex.getYSize(), 1);
    EXPECT_EQ(tex.getSymbol(0, 0), U'H');
    EXPECT_EQ(tex.getSymbol(4, 0), U'o');
}

TEST(TextureTest, AddTextFlagHorizontalNoShrink) {
    Texture tex = Texture::Builder(10, 2)
        .addText(0, 0, U"Hi", Rgb(255,255,255), Rgb(0,0,0), TexFlag::HORIZONTAL)
        .build();

    EXPECT_EQ(tex.getXSize(), 10);
    EXPECT_EQ(tex.getYSize(), 2);
}

TEST(TextureTest, AddTextFlagHorizontalMultiline) {
    Texture tex = Texture::Builder(2, 2)
        .addText(0, 0, U"Hi\nBye!", Rgb(255,255,255), Rgb(0,0,0), TexFlag::HORIZONTAL)
        .build();

    EXPECT_EQ(tex.getXSize(), 4);
    EXPECT_EQ(tex.getSymbol(0, 1), U'B');
    EXPECT_EQ(tex.getSymbol(3, 1), U'!');
}

TEST(TextureTest, AddTextFlagHorizontalWideChar) {
    Texture tex = Texture::Builder(1, 1)
        .addText(0, 0, U"\U0001F525", Rgb(255,255,255), Rgb(0,0,0), TexFlag::HORIZONTAL)
        .build();

    EXPECT_EQ(tex.getXSize(), 2);
    EXPECT_EQ(tex.getSymbol(0, 0), U'\U0001F525');
}

TEST(TextureTest, AddTextFlagVerticalExpands) {
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

TEST(TextureTest, AddTextFlagVerticalNoShrink) {
    Texture tex = Texture::Builder(5, 10)
        .addText(0, 0, U"Hi", Rgb(255,255,255), Rgb(0,0,0), TexFlag::VERTICAL)
        .build();

    EXPECT_EQ(tex.getXSize(), 5);
    EXPECT_EQ(tex.getYSize(), 10);
}

TEST(TextureTest, AddTextFlagVerticalNewlines) {
    Texture tex = Texture::Builder(5, 1)
        .addText(0, 0, U"A\nB\nC", Rgb(255,255,255), Rgb(0,0,0), TexFlag::VERTICAL)
        .build();

    EXPECT_EQ(tex.getYSize(), 3);
    EXPECT_EQ(tex.getSymbol(0, 0), U'A');
    EXPECT_EQ(tex.getSymbol(0, 1), U'B');
    EXPECT_EQ(tex.getSymbol(0, 2), U'C');
}

TEST(TextureTest, CreateReadsBuilderData) {
    Texture::Builder builder(3, 3);
    builder.fillTexture('X', Rgb(10, 20, 30), Rgb(40, 50, 60));

    Texture tex = builder.create();

    EXPECT_EQ(tex.getXSize(), 3);
    EXPECT_EQ(tex.getYSize(), 3);
    EXPECT_EQ(tex.getSymbol(0, 0), U'X');
    EXPECT_EQ(tex.getFgColor(1, 1).r, 10);
    EXPECT_EQ(tex.getBgColor(2, 2).b, 60);
}

TEST(TextureTest, CreateBuilderStillUsable) {
    Texture::Builder builder(3, 3);
    builder.fillTexture('A', Rgb(0, 0, 0), Rgb(0, 0, 0));

    {
        Texture view = builder.create();
        EXPECT_EQ(view.getSymbol(0, 0), U'A');
    }

    builder.setPixel(1, 1, 'B', Rgb(1, 1, 1), Rgb(1, 1, 1));

    {
        Texture view2 = builder.create();
        EXPECT_EQ(view2.getSymbol(0, 0), U'A');
        EXPECT_EQ(view2.getSymbol(1, 1), U'B');
    }
}

TEST(TextureTest, CreateSharesData) {
    Texture::Builder builder(2, 2);
    builder.fillTexture('A', Rgb(0, 0, 0), Rgb(0, 0, 0));

    Texture view = builder.create();
    EXPECT_EQ(view.getSymbol(0, 0), U'A');

    builder.setPixel(0, 0, 'Z', Rgb(0, 0, 0), Rgb(0, 0, 0));
    EXPECT_EQ(view.getSymbol(0, 0), U'Z');
}

TEST(TextureTest, CreateMultipleCalls) {
    Texture::Builder builder(2, 2);
    builder.fillTexture('A', Rgb(0, 0, 0), Rgb(0, 0, 0));

    Texture view1 = builder.create();
    Texture view2 = builder.create();

    builder.setPixel(0, 0, 'Q', Rgb(0, 0, 0), Rgb(0, 0, 0));

    EXPECT_EQ(view1.getSymbol(0, 0), U'Q');
    EXPECT_EQ(view2.getSymbol(0, 0), U'Q');
}

TEST(TextureTest, SnapshotReadsBuilderData) {
    Texture::Builder builder(3, 3);
    builder.fillTexture('X', Rgb(10, 20, 30), Rgb(40, 50, 60));

    Texture tex = builder.snapshot();

    EXPECT_EQ(tex.getXSize(), 3);
    EXPECT_EQ(tex.getYSize(), 3);
    EXPECT_EQ(tex.getSymbol(0, 0), U'X');
    EXPECT_EQ(tex.getFgColor(1, 1).r, 10);
    EXPECT_EQ(tex.getBgColor(2, 2).b, 60);
}

TEST(TextureTest, SnapshotBuilderStillUsable) {
    Texture::Builder builder(3, 3);
    builder.fillTexture('A', Rgb(0, 0, 0), Rgb(0, 0, 0));

    Texture snap = builder.snapshot();
    EXPECT_EQ(snap.getSymbol(0, 0), U'A');

    builder.setPixel(1, 1, 'B', Rgb(1, 1, 1), Rgb(1, 1, 1));
    Texture final_tex = builder.build();

    EXPECT_EQ(final_tex.getSymbol(0, 0), U'A');
    EXPECT_EQ(final_tex.getSymbol(1, 1), U'B');
}

TEST(TextureTest, SnapshotIsIndependent) {
    Texture::Builder builder(2, 2);
    builder.fillTexture('A', Rgb(0, 0, 0), Rgb(0, 0, 0));

    Texture snap = builder.snapshot();
    EXPECT_EQ(snap.getSymbol(0, 0), U'A');

    builder.setPixel(0, 0, 'Z', Rgb(0, 0, 0), Rgb(0, 0, 0));
    EXPECT_EQ(snap.getSymbol(0, 0), U'A');
}

TEST(TextureTest, SnapshotMultipleCalls) {
    Texture::Builder builder(2, 2);
    builder.fillTexture('A', Rgb(0, 0, 0), Rgb(0, 0, 0));

    Texture snap1 = builder.snapshot();

    builder.setPixel(0, 0, 'B', Rgb(0, 0, 0), Rgb(0, 0, 0));
    Texture snap2 = builder.snapshot();

    builder.setPixel(0, 0, 'C', Rgb(0, 0, 0), Rgb(0, 0, 0));

    EXPECT_EQ(snap1.getSymbol(0, 0), U'A');
    EXPECT_EQ(snap2.getSymbol(0, 0), U'B');
}

}
