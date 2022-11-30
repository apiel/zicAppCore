#ifndef APP_CORE_VIEW_JS_H_
#define APP_CORE_VIEW_JS_H_

#include "./app_core_renderer.h"
#include "./app_core_view.h"

#include <duktape.h>
#include <app_duk_extra.h>

class App_View_JS : public App_View {
public:
duk_context* ctx;

    App_View_JS()
    {
        ctx = duk_create_heap_default();
    }

    ~App_View_JS()
    {
        duk_destroy_heap(ctx);
    }

    virtual void initDisplay(App_Renderer* renderer)
    {
        strcpy(renderer->text, "");
    }

    void render(App_Renderer* renderer)
    {
        initDisplay(renderer);
    }

    virtual uint8_t update(UiKeys* keys, App_Renderer* renderer)
    {
        uint8_t res = VIEW_CHANGED;

        if (res != VIEW_NONE) {
            render(renderer);
        }
        return res;
    }
};

#endif