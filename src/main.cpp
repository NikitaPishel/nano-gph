#include <iostream>
#include <chrono>
#include "ngph/canvas.h"
#include "ngph/texture.h"

using namespace std;
using namespace gph;

int main() {
    Canvas canv(32, 12);
    canv.updateSize();

    Texture texWhite = Texture::Builder(2, 1)
        .fillTexture(' ', "white", "red")
        .build();

    Texture texBlack = Texture::Builder(2, 1)
        .fillTexture(' ', "white", "black")
        .build();

    // build chessboard pattern
    for (int j = 0; j < canv.getYSize(); j++) {
        for (int i = 0; i < canv.getXSize() / 2; i++) {
            bool isWhite = ((i + j) % 2 == 0);
            const auto& tex = isWhite ? texWhite : texBlack;
            canv.addTexture(i * 2, j, tex);
        }
    }

    constexpr int numRenders = 500;
    double totalTime = 0.0;

    for (int f = 0; f < numRenders; f++) {
        auto start = chrono::high_resolution_clock::now();

        canv.render();

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        totalTime += elapsed.count();
    }

    double avgFrameTime = totalTime / numRenders;
    double avgFPS = 1.0 / avgFrameTime;

    std::cout << "\nAverage FPS over " << numRenders << " renders: " << avgFPS << " FPS\n";

    return 0;
}