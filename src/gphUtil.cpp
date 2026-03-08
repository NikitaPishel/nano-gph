#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include "gphUtil.h"

// graphics utility namespace
namespace gph {
    winsize getWinSize() {
        winsize window;

        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window) == -1) {
            std::cerr << ("Failed to get window size");

            window.ws_col = 1;
            window.ws_row = 1;
            window.ws_xpixel = 1;
            window.ws_ypixel = 1;
        }

        return window;
    };

    bool isWideChar(char32_t cp) {
        // Sorted ranges of East Asian Wide / Fullwidth codepoints (Unicode 15.x)
        static const struct { char32_t lo, hi; } ranges[] = {
            {0x1100,  0x115F},  // Hangul Jamo
            {0x2E80,  0x303E},  // CJK Radicals Supplement – CJK Symbols & Punctuation
            {0x3041,  0x33BF},  // Hiragana … CJK Compatibility
            {0x3400,  0x4DBF},  // CJK Unified Ideographs Extension A
            {0x4E00,  0x9FFF},  // CJK Unified Ideographs
            {0xA000,  0xA4CF},  // Yi Syllables & Radicals
            {0xA960,  0xA97F},  // Hangul Jamo Extended-A
            {0xAC00,  0xD7AF},  // Hangul Syllables
            {0xF900,  0xFAFF},  // CJK Compatibility Ideographs
            {0xFE10,  0xFE19},  // Vertical Forms
            {0xFE30,  0xFE6F},  // CJK Compatibility Forms & Small Form Variants
            {0xFF01,  0xFF60},  // Fullwidth Latin & Halfwidth Katakana
            {0xFFE0,  0xFFE6},  // Fullwidth Currency Signs
            {0x1B000, 0x1B12F}, // Kana Supplement & Extended-A
            {0x1F004, 0x1F004}, // Mahjong Tile Red Dragon
            {0x1F0CF, 0x1F0CF}, // Playing Card Black Joker
            {0x1F200, 0x1F2FF}, // Enclosed CJK Letters & Months
            {0x1F300, 0x1F64F}, // Miscellaneous Symbols & Emoticons
            {0x1F680, 0x1F6FF}, // Transport & Map Symbols
            {0x1F900, 0x1FAFF}, // Supplemental Symbols & Pictographs
            {0x20000, 0x2A6DF}, // CJK Unified Ideographs Extension B
            {0x2A700, 0x2CEAF}, // CJK Extensions C – E
            {0x2CEB0, 0x2EBEF}, // CJK Extension F
            {0x2F800, 0x2FA1F}, // CJK Compatibility Ideographs Supplement
            {0x30000, 0x3134F}, // CJK Extension G
        };

        for (const auto& r : ranges) {
            if (cp < r.lo) break; // ranges are sorted — no match possible further
            if (cp <= r.hi) return true;
        }
        return false;
    }
};
