#ifndef APP_CORE_VIEW_H_
#define APP_CORE_VIEW_H_

#include "./app_core_def.h"
#include "./app_core_renderer.h"

#include <cstring>
#include <stdio.h>

class App_View {
public:
    virtual void render(App_Renderer* renderer) = 0;
    virtual uint8_t update(UiKeys* keys, App_Renderer* renderer) = 0;

    virtual bool renderOn(uint8_t event)
    {
        return false;
    }

    virtual void focusView()
    {
    }
};

#endif
