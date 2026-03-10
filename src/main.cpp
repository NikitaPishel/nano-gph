#include <cstdlib>
#include <iostream>
#include <chrono>
#include "ngph/canvas.h"
#include "ngph/colors.h"
#include "ngph/texture.h"

using namespace std;
using namespace gph;

int main() {
    Canvas canv(32, 12);
    canv.updateSize();

    int width = canv.getXSize();
    int height = canv.getYSize();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Calculate a ratio (0.0 to 1.0) based on distance from top-left
            // Using (x + y) / (max_x + max_y) gives a diagonal flow
            float factor = static_cast<float>(x + y) / (width + height - 2);

            // Interpolate from Blue (top-left) to Gold (bottom-right)
            int r = static_cast<int>(0   + factor * 255);
            int g = static_cast<int>(50  + factor * 150);
            int b = static_cast<int>(200 - factor * 200);

            // Apply the color as the background of a space character
            canv.setPixel(x, y, U' ', Rgb(255, 255, 255), Rgb(r, g, b));
        }
    }

    // Overlay a text label using addText
    Texture labelTex = Texture::Builder(22, 3)
        .addText(0, 0, U"Hello, World!\n\tTabbed line\n🚀Row 3 here", Rgb(255, 255, 0), Rgb(0, 0, 80))
        .build();
    canv.addTexture(1, 1, labelTex);

    // Demo: texture placed partially off the bottom-right corner — visible portion is clipped in
    Texture cornerTex = Texture::Builder(10, 4)
        .fillTexture(' ', Rgb(0, 0, 0), Rgb(180, 60, 200))
        .addText(0, 0, U"[clipped]", Rgb(255, 255, 255), Rgb(180, 60, 200))
        .build();
    canv.addTexture(width - 5, height - 2, cornerTex);
    canv.addTexture(width/2, -2, cornerTex);

    // Alternate emoji then ASCII char across the middle row
    const char32_t emojis[] = {U'🌟', U'🔥', U'💎', U'🚀', U'🎮', U'🌈', U'⚡', U'🎯'};
    const char32_t chars[]  = {U'A',  U'B',  U'C',  U'D',  U'E',  U'F',  U'G',  U'H' };
    const int emojiCount = sizeof(emojis) / sizeof(emojis[0]);
    int midY = height / 2;
    for (int x = 0; x < width/2; ++x) {
        int idx = x % emojiCount;
        canv.setPixel(x, midY, emojis[idx], Rgb(255, 255, 255), Rgb(0, 0, 0));
        canv.setPixel(x+2, midY, chars[idx], Rgb(255, 255, 0), Rgb(0, 0, 0));
        x+=2;
    }

    // Benchmarking Render Loop
    constexpr int numRenders = 100;
    double totalTime = 0.0;

    for (int f = 0; f < numRenders; f++) {
        auto start = chrono::high_resolution_clock::now();

        canv.render();
        // If the window resized, we'd ideally re-calculate the gradient here
        canv.updateSize();

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        totalTime += elapsed.count();
    }

    double avgFrameTime = totalTime / numRenders;
    double avgFPS = 1.0 / avgFrameTime;

    // Clear screen and print stats
    std::cout << "\nAverage FPS over " << numRenders << " renders: " << avgFPS << " FPS\n";

    return 0;
}
