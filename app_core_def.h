#ifndef APP_CORE_DEF_H_
#define APP_CORE_DEF_H_

#include <stdint.h>

enum {
    VIEW_NONE,
    VIEW_CHANGED,
};

enum {
    UI_KEY_UP,
    UI_KEY_DOWN,
    UI_KEY_LEFT,
    UI_KEY_RIGHT,
    UI_KEY_EDIT,
    UI_KEY_MENU,
    UI_KEY_ACTION,
    UI_KEY_COUNT
};

class UiKeys {
public:
    bool Up = false;
    bool Down = false;
    bool Right = false;
    bool Left = false;
    bool Edit = false;
    bool Menu = false;
    bool Action = false;

    int8_t getDirection()
    {
        if (Right) {
            return 1;
        } else if (Up) {
            return 10;
        } else if (Left) {
            return -1;
        } else if (Down) {
            return -10;
        }
        return 0;
    }

    float getDirection(float step)
    {
        return getDirection() * step;
    }
};

#endif