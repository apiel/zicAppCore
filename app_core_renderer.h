#ifndef APP_CORE_DISPLAY_BASE_H_
#define APP_CORE_DISPLAY_BASE_H_

#include <cstring>
#include <stdint.h>

// #define MAX_DISPLAY_TEXT 176 // 21*8 + 8\n
//                              // but should actually be 20*8 to fit console

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
    int8_t coloredRow[2] = { -1, -1 };
    bool firstLetter = false;
    uint8_t colored[COLORED_SIZE][5] = { { 0, 0, 0, 0, 0 } };

    uint8_t startRow = 0;

    virtual void reset()
    {
        firstLetter = false;
        cursorPos = NULL;
        cursorLen = 0;
        useColoredRow(-1, -1);
        for (int i = 0; i < COLORED_SIZE; i++) {
            colored[i][0] = 0;
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
        for (int i = 0; i < COLORED_SIZE; i++) {
            if (colored[i][0] == 1
                && row >= colored[i][1] && row < colored[i][2]
                && col >= colored[i][3] && col < colored[i][4]) {
                return true;
            }
        }
        return false;
    }

    // TODO could even think about specifying the color rgb or with a color id like primary, secondary, etc
    void useColor(uint8_t fromRow, uint8_t toRow, uint8_t fromCol, uint8_t toCol)
    {
        for (int i = 0; i < COLORED_SIZE; i++) {
            if (colored[i][0] == 0) {
                colored[i][0] = 1;
                colored[i][1] = fromRow;
                colored[i][2] = toRow;
                colored[i][3] = fromCol;
                colored[i][4] = toCol;
                break;
            }
        }
    }

    void useColoredRow()
    {
        useColoredRow(0, -1);
    }

    void useColoredRow(int8_t row)
    {
        useColoredRow(row, -1);
    }

    void useColoredRow(int8_t row1, int8_t row2)
    {
        coloredRow[0] = row1;
        coloredRow[1] = row2;
    }

    void useFirstLetterHilighted()
    {
        firstLetter = true;
    }
};

#endif
