#ifndef APP_CORE_DISPLAY_BASE_H_
#define APP_CORE_DISPLAY_BASE_H_

#include <cstring>
#include <stdint.h>

#define MAX_DISPLAY_COL 30
#define MAX_DISPLAY_ROW 10

// #define MAX_DISPLAY_TEXT MAX_DISPLAY_COL*(MAX_DISPLAY_ROW + 1) // +1 for \n
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
    COLOR_MARKER,
};

class App_Renderer {
public:
    char text[MAX_DISPLAY_TEXT] = "";
    char* cursorPos = NULL;
    uint8_t cursorLen = 0;
    uint8_t colored[MAX_DISPLAY_ROW][MAX_DISPLAY_COL];

    uint8_t startRow = 0;

    virtual void reset(uint8_t color = 0)
    {
        cursorPos = NULL;
        cursorLen = 0;
        setDefaultColor(color);
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

    uint8_t getColored(uint8_t row, uint8_t col)
    {
        return colored[row][col];
    }

    void setDefaultColor(uint8_t color)
    {
        for (int i = 0; i < MAX_DISPLAY_ROW; i++) {
            for (int j = 0; j < MAX_DISPLAY_COL; j++) {
                colored[i][j] = color;
            }
        }
    }

    void useColor(uint8_t row, uint8_t col, uint8_t color)
    {
        colored[row % MAX_DISPLAY_ROW][col % MAX_DISPLAY_COL] = color;
    }

    void useColor(uint8_t row, uint8_t col, uint8_t len, uint8_t color)
    {
        for (int i = 0; i < len; i++) {
            colored[row % MAX_DISPLAY_ROW][(col + i) % MAX_DISPLAY_COL] = color;
        }
    }

    void useColor(uint8_t fromRow, uint8_t tillRow, uint8_t fromCol, uint8_t tillCol, uint8_t color)
    {
        for (int i = fromRow; i < MAX_DISPLAY_ROW && i < tillRow; i++) {
            for (int j = 0; j < MAX_DISPLAY_COL && j < tillCol; j++) {
                useColor(i, j, color);
            }
        }
    }

    void useColoredRow(int8_t row = 0)
    {
        useColor(row, row + 1, 0, 255, COLOR_DARK);
    }
};

#endif
