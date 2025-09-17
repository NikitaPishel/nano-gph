#include <iostream>
#include <chrono>
#include "ngph/canvas.h"
#include "ngph/texture.h"

using namespace std;
using namespace gph;

int main() {
    Canvas canv(32, 12);
    canv.updateSize();

    Texture texChess = Texture::Builder(4, 2)
        .fillTexture(' ', "white", "red")
        .setPixel(0, 0, ' ', "white", "black")
        .setPixel(1, 0, ' ', "white", "black")
        .setPixel(2, 1, ' ', "white", "black")
        .setPixel(3, 1, ' ', "white", "black")
        .build();

    // build chessboard pattern
    canv.fillWithTexture(texChess);

    Texture squares = Texture::Builder(10, 5)
    .fillTexture(' ', "white", "b_yellow")
    .addBox(2, 1, 6, 3, ' ', "white", "yellow")
    .build();

    canv.addTexture(2, 1, squares);

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