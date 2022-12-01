#ifndef APP_CORE_DISPLAY_H_
#define APP_CORE_DISPLAY_H_

#include "./app_core_renderer.h"
#include "./app_core_util.h"

#ifdef FONT_FILE
#include FONT_FILE
#else
#include "./fonts/fontData5x7.h"
#endif

#ifndef LINE_SPACING
#define LINE_SPACING 2
#endif

// Colors could be set per view...

#define UI_COLOR_BG 0, 0, 0
#define UI_COLOR_PRIMARY 0xFF, 0xFF, 0xFF
#define UI_COLOR_SECONDARY 0xFF, 0xFF, 0xFF
#define UI_COLOR_DARK 100, 100, 100
#define UI_COLOR_MEDIUM 130, 130, 130
#define UI_COLOR_LIGHT 210, 210, 210
#define UI_COLOR_PLAY 122, 255, 0
#define UI_COLOR_HILIGHT 255, 255, 0

// #define UI_COLOR_EQUAL 255, 0, 128
// #define UI_COLOR_EQUAL 255, 0, 255
#define UI_COLOR_EQUAL 0, 255, 255
#define UI_COLOR_CURSOR 0, 128, 255

class App_Display : public App_Renderer {
protected:
    uint16_t xScreen = 0;
    uint16_t yScreen = 0;
    uint8_t row = 0;
    uint8_t col = 0;
    uint8_t currentColor = 0;

    // Could be boolean and use it to draw char process...
    virtual void drawPixel(int16_t x, int16_t y) = 0;
    virtual void drawCursor(int16_t x, int16_t y) = 0;
    virtual void setColor(uint8_t r, uint8_t g, uint8_t b) = 0;

    void setColor(uint8_t color)
    {
        if (color == currentColor) {
            return;
        }
        currentColor = color;
        switch (color) {
        case COLOR_PRIMARY:
            setColor(UI_COLOR_PRIMARY);
            break;
        case COLOR_SECONDARY:
            setColor(UI_COLOR_SECONDARY);
            break;
        case COLOR_LIGHT:
            setColor(UI_COLOR_LIGHT);
            break;
        case COLOR_MEDIUM:
            setColor(UI_COLOR_MEDIUM);
            break;
        case COLOR_DARK:
            setColor(UI_COLOR_DARK);
            break;
        case COLOR_PLAY:
            setColor(UI_COLOR_PLAY);
            break;
        case COLOR_HILIGHT:
            setColor(UI_COLOR_HILIGHT);
            break;
        default:
            setColor(UI_COLOR_PRIMARY);
            break;
        }
    }

    virtual void drawChar(unsigned char chr, uint16_t x, uint16_t y)
    {
        const unsigned char* ptr = fontData + (chr - FONT_ASCII_START) * FONT_H;

        for (int i = 0; i < FONT_H; i++, ptr++) {
            for (int j = 8 - FONT_W; j < 8; j++) { // Support only 8 bits font
                if ((*ptr >> j) & 1) {
                    drawPixel(x + (8 - j), y + i);
                }
            }
        }
    }

    uint16_t levelSpecialChar[6] = {
        *(uint16_t*)("⠠" + 1),
        *(uint16_t*)("⠤" + 1),
        *(uint16_t*)("⠴" + 1),
        *(uint16_t*)("⠶" + 1),
        *(uint16_t*)("⠾" + 1),
        *(uint16_t*)("⠿" + 1),
    };

    virtual void drawSpecialChar(uint16_t val, uint16_t x, uint16_t y)
    {
        if (val == *(uint16_t*)(" " + 1)) {
            return;
        }

        if (val == *(uint16_t*)("⤸" + 1)) {
            // arc
            drawPixel(x + 2, y);
            drawPixel(x + 3, y + 1);
            drawPixel(x + 3, y + 2);
            drawPixel(x + 4, y + 3);
            drawPixel(x + 4, y + 4);
            drawPixel(x + 4, y + 5);
            drawPixel(x + 4, y + 6);
            drawPixel(x + 4, y + 7);
            drawPixel(x + 3, y + 8);
            drawPixel(x + 3, y + 9);
            drawPixel(x + 2, y + 10);
            drawPixel(x + 1, y + 11);
            drawPixel(x, y + 12);

            // arrow
            drawPixel(x + 1, y + 12);
            drawPixel(x + 2, y + 12);
            drawPixel(x + 3, y + 12);

            drawPixel(x, y + 11);
            drawPixel(x, y + 10);
            drawPixel(x, y + 9);
            return;
        }

        uint8_t level = getLevel(val);
        if (level >= 5) {
            drawPixel(x + 4, y);
            drawPixel(x + 5, y);
            drawPixel(x + 4, y + 1);
            drawPixel(x + 5, y + 1);
        }

        if (level >= 4) {
            drawPixel(x + 1, y);
            drawPixel(x + 2, y);
            drawPixel(x + 1, y + 1);
            drawPixel(x + 2, y + 1);
        }

        if (level >= 3) {

            drawPixel(x + 4, y + 4);
            drawPixel(x + 5, y + 4);
            drawPixel(x + 4, y + 3);
            drawPixel(x + 5, y + 3);
        }

        if (level >= 2) {
            drawPixel(x + 1, y + 3);
            drawPixel(x + 2, y + 3);
            drawPixel(x + 1, y + 4);
            drawPixel(x + 2, y + 4);
        }

        if (level >= 1) {
            drawPixel(x + 4, y + 7);
            drawPixel(x + 5, y + 7);
            drawPixel(x + 4, y + 6);
            drawPixel(x + 5, y + 6);
        }

        drawPixel(x + 1, y + 6);
        drawPixel(x + 2, y + 6);
        drawPixel(x + 1, y + 7);
        drawPixel(x + 2, y + 7);
    }

public:
    virtual void clearScreen() = 0;

    void reset(uint8_t color = 0) override
    {
        clearScreen();
        App_Renderer::reset(color);
    }

    virtual void drawText()
    {
        uint16_t x = xScreen;
        uint16_t y = yScreen;
        uint16_t orig_x = x;
        const char* txt = text;
        row = startRow;
        col = 0;

        while (*txt) {
            if (*txt == '\n') {
                x = orig_x;
                y += (FONT_H + LINE_SPACING);
                col = 0;
                row++;
            } else {
                setColor(getColored(row, col));
                if (cursorLen && txt >= cursorPos && txt < cursorPos + cursorLen) {
                    drawCursor(x, y);
                }

                if (*txt == -30) {
                    drawSpecialChar(*(uint16_t*)(txt + 1), x, y);
                    txt += 2;
                } else {
                    drawChar(*txt, x, y);
                }
                x += FONT_W;
                col++;
            }
            txt++;
        }
    }
};

#endif
