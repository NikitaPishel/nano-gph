#include <gtest/gtest.h>
#include <filesystem>

#include "grid.h"
#include "ngph/colors.h"
#include <ngph/texture.h>
#include <ngph/iotex.h>

using namespace gph;

namespace {

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

}
