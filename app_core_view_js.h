#ifndef APP_CORE_VIEW_JS_H_
#define APP_CORE_VIEW_JS_H_

#include "./app_core_renderer.h"
#include "./app_core_view.h"

#include <app_duk_extra.h>
#include <duktape.h>

#define APP_VIEW_JS_TOP_ROW 1

class App_View_JS : public App_View {
public:
    duk_context* ctx;
    static App_Renderer* display;

    App_View_JS()
    {
        ctx = duk_create_heap_default();
        duk_push_c_function(ctx, App_View_JS::duk_render, 1);
        duk_put_global_string(ctx, "render");
        duk_push_c_function(ctx, App_View_JS::duk_useColor, 4);
        duk_put_global_string(ctx, "useColor");

        duk_push_int(ctx, COLOR_PRIMARY);
        duk_put_global_string(ctx, "COLOR_PRIMARY");
        duk_push_int(ctx, COLOR_SECONDARY);
        duk_put_global_string(ctx, "COLOR_SECONDARY");
        duk_push_int(ctx, COLOR_HIDE);
        duk_put_global_string(ctx, "COLOR_HIDE");
        duk_push_int(ctx, COLOR_LIGHT);
        duk_put_global_string(ctx, "COLOR_LIGHT");
        duk_push_int(ctx, COLOR_MEDIUM);
        duk_put_global_string(ctx, "COLOR_MEDIUM");
        duk_push_int(ctx, COLOR_DARK);
        duk_put_global_string(ctx, "COLOR_DARK");
        duk_push_int(ctx, COLOR_HILIGHT);
        duk_put_global_string(ctx, "COLOR_HILIGHT");
        duk_push_int(ctx, COLOR_PLAY);
        duk_put_global_string(ctx, "COLOR_PLAY");
        duk_push_int(ctx, COLOR_CURSOR);
        duk_put_global_string(ctx, "COLOR_CURSOR");
    }

    ~App_View_JS()
    {
        duk_destroy_heap(ctx);
    }

    virtual void preRender(App_Renderer* renderer)
    {
        strcpy(renderer->text, "");
    }

    void render(App_Renderer* renderer)
    {
        preRender(renderer);
        App_View_JS::display = renderer;
        if (duk_get_global_string(ctx, "renderer")) {
            duk_call(ctx, 0);
            duk_pop(ctx);
        }
    }

    virtual uint8_t update(UiKeys* keys, App_Renderer* renderer)
    {
        App_View_JS::display = renderer;
        uint8_t res = VIEW_CHANGED;

        if (duk_get_global_string(ctx, "update")) {
            duk_idx_t obj_idx = duk_push_object(ctx);
            duk_push_boolean(ctx, keys->Up);
            duk_put_prop_string(ctx, obj_idx, "Up");
            duk_push_boolean(ctx, keys->Down);
            duk_put_prop_string(ctx, obj_idx, "Down");
            duk_push_boolean(ctx, keys->Right);
            duk_put_prop_string(ctx, obj_idx, "Right");
            duk_push_boolean(ctx, keys->Left);
            duk_put_prop_string(ctx, obj_idx, "Left");
            duk_push_boolean(ctx, keys->Menu);
            duk_put_prop_string(ctx, obj_idx, "Menu");
            duk_push_boolean(ctx, keys->Edit);
            duk_put_prop_string(ctx, obj_idx, "Edit");
            duk_push_boolean(ctx, keys->Action);
            duk_put_prop_string(ctx, obj_idx, "Action");
            // TODO might want to add functions to obj
            duk_call(ctx, 1);
            res = (int)duk_get_int(ctx, -1);
            duk_pop(ctx);
        }

        if (res != VIEW_NONE) {
            render(renderer);
        }
        return res;
    }

    static duk_ret_t duk_render(duk_context* ctx)
    {
        strcat(App_View_JS::display->text, duk_safe_to_string(ctx, 0));
        return 0;
    }

    static duk_ret_t duk_useColor(duk_context* ctx)
    {
        uint8_t row = duk_get_int(ctx, 0);
        uint8_t col = duk_get_int(ctx, 1);
        uint8_t color = duk_get_int(ctx, 2);
        if (duk_is_undefined(ctx, 3)) {
            App_View_JS::display->useColor(row + APP_VIEW_JS_TOP_ROW, col, color);
        } else {
            uint8_t len = duk_get_int(ctx, 3);
            App_View_JS::display->useColor(row + APP_VIEW_JS_TOP_ROW, col, color, len);
        }

        return 0;
    }
};

App_Renderer* App_View_JS::display = NULL;

#endif