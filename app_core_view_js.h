#ifndef APP_CORE_VIEW_JS_H_
#define APP_CORE_VIEW_JS_H_

#include "./app_core_renderer.h"
#include "./app_core_view.h"

#include <app_duk_extra.h>
#include <duktape.h>

class App_View_JS : public App_View {
public:
    duk_context* ctx;
    static App_Renderer* display;

    App_View_JS()
    {
        ctx = duk_create_heap_default();
        duk_push_c_function(ctx, App_View_JS::duk_render, DUK_VARARGS);
        duk_put_global_string(ctx, "render");
    }

    ~App_View_JS()
    {
        duk_destroy_heap(ctx);
    }

    void render(App_Renderer* renderer)
    {
        App_View_JS::display = renderer;
        strcpy(renderer->text, "");
        duk_get_global_string(ctx, "renderer");
        duk_call(ctx, 0);
    }

    virtual uint8_t update(UiKeys* keys, App_Renderer* renderer)
    {
        App_View_JS::display = renderer;
        uint8_t res = VIEW_CHANGED;

        if (res != VIEW_NONE) {
            render(renderer);
        }
        return res;
    }

    static duk_ret_t duk_render(duk_context* ctx)
    {
        duk_push_string(ctx, " ");
        duk_insert(ctx, 0);
        duk_join(ctx, duk_get_top(ctx) - 1);
        strcpy(App_View_JS::display->text, duk_safe_to_string(ctx, -1));
        return 0;
    }
};

App_Renderer* App_View_JS::display = NULL;

#endif