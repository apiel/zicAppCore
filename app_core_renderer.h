#ifndef APP_CORE_DISPLAY_BASE_H_
#define APP_CORE_DISPLAY_BASE_H_

#include <cstring>
#include <stdint.h>

#define MAX_DISPLAY_COL 30
#define MAX_DISPLAY_ROW 8

// #define MAX_DISPLAY_TEXT 30*8 + 8 // 30*8 + 8\n
// However, special char take more place, so let's keep it large
#define MAX_DISPLAY_TEXT 512
#define COLORED_SIZE 5

enum {
    COLOR_PRIMARY,
    COLOR_SECONDARY,
    COLOR_LIGHT,
    COLOR_MEDIUM,
    COLOR_DARK,
    COLOR_PLAY,
    COLOR_HILIGHT,
};

class App_Renderer {
public:
    char text[MAX_DISPLAY_TEXT] = "";
    char* cursorPos = NULL;
    uint8_t cursorLen = 0;
    bool firstLetter = false;
    uint8_t colored[MAX_DISPLAY_ROW][MAX_DISPLAY_COL];

    uint8_t startRow = 0;

    virtual void reset()
    {
        firstLetter = false;
        cursorPos = NULL;
        cursorLen = 0;
        // for (int i = 0; i < COLORED_SIZE; i++) {
        //     colored[i][0] = 0;
        // }
        for (int i = 0; i < MAX_DISPLAY_ROW; i++) {
            for (int j = 0; j < MAX_DISPLAY_COL; j++) {
                colored[i][j] = 0;
            }
        }
    }

    virtual bool ready()
    {
        return true;
    }

    void setCursor(uint8_t len, int8_t start = 0)
    {
        cursorPos = text + strlen(text) + start;
        cursorLen = len;
    }

    bool isColored(uint8_t row, uint8_t col)
    {
        return getColored(row, col) != 255;
    }

    uint8_t getColored(uint8_t row, uint8_t col)
    {
        return colored[row][col];
    }

    void useColor(uint8_t row, uint8_t col, uint8_t color = COLOR_MEDIUM)
    {
        colored[row][col] = color;
    }

    void useColor(uint8_t fromRow, uint8_t tillRow, uint8_t fromCol, uint8_t tillCol, uint8_t color = COLOR_MEDIUM)
    {
        for (int i = fromRow; i < MAX_DISPLAY_ROW && i < tillRow; i++) {
            for (int j = 0; j < MAX_DISPLAY_COL && j < tillCol; j++) {
                useColor(i, j, color);
            }
        }
    }

    void useColoredRow(int8_t row = 0)
    {
        useColor(row, row + 1, 0, 255, COLOR_LIGHT);
    }

    void useFirstLetterHilighted()
    {
        firstLetter = true;
    }
};

#endif
