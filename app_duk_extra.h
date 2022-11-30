#ifndef APP_DUK_EXTRA_H_
#define APP_DUK_EXTRA_H_

#include "./app_core_def.h"

#include <duktape.h>
#include <stdio.h>

#define DUK_STRING_PUSH_SAFE (1 << 0) /* no error if file does not exist */

/*
 *  duk_dump_context_{stdout,stderr}()
 */

void duk_dump_context_stdout(duk_context* ctx)
{
    duk_push_context_dump(ctx);
    fprintf(stdout, "%s\n", duk_safe_to_string(ctx, -1));
    duk_pop(ctx);
}

void duk_dump_context_stderr(duk_context* ctx)
{
    duk_push_context_dump(ctx);
    fprintf(stderr, "%s\n", duk_safe_to_string(ctx, -1));
    duk_pop(ctx);
}

/*
 *  duk_push_string_file() and duk_push_string_file_raw()
 */

// TODO use SDL file stuff?
const char* duk_push_string_file_raw(duk_context* ctx, const char* path, duk_uint_t flags)
{
    FILE* f = NULL;
    char* buf;
    long sz; /* ANSI C typing */

    if (!path) {
        goto fail;
    }
    f = fopen(path, "rb");
    if (!f) {
        goto fail;
    }
    if (fseek(f, 0, SEEK_END) < 0) {
        goto fail;
    }
    sz = ftell(f);
    if (sz < 0) {
        goto fail;
    }
    if (fseek(f, 0, SEEK_SET) < 0) {
        goto fail;
    }
    buf = (char*)duk_push_fixed_buffer(ctx, (duk_size_t)sz);
    if ((size_t)fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        duk_pop(ctx);
        goto fail;
    }
    (void)fclose(f); /* ignore fclose() error */
    return duk_buffer_to_string(ctx, -1);

fail:
    if (f) {
        (void)fclose(f); /* ignore fclose() error */
    }

    if (flags & DUK_STRING_PUSH_SAFE) {
        duk_push_undefined(ctx);
    } else {
        (void)duk_type_error(ctx, "read file error");
    }
    return NULL;
}

duk_int_t duk_compile_file(duk_context* ctx, const char* path)
{
    duk_int_t rc;
    duk_push_string_file_raw(ctx, path, DUK_STRING_PUSH_SAFE);
    duk_push_string(ctx, path);

    if ((rc = duk_pcompile(ctx, DUK_COMPILE_EVAL)) != 0) {
        APP_LOG("compile failed: %s\n", duk_safe_to_string(ctx, -1));
    }
    return rc;
}

duk_int_t duk_eval_file(duk_context* ctx, const char* path)
{
    duk_int_t rc = duk_compile_file(ctx, path);
    if (rc != 0) {
        return rc;
    }
    duk_push_global_object(ctx); /* 'this' binding */
    return duk_pcall_method(ctx, 0);
}

static duk_ret_t duk_fn_log(duk_context* ctx)
{
    duk_push_string(ctx, " ");
    duk_insert(ctx, 0);
    duk_join(ctx, duk_get_top(ctx) - 1);
    printf("%s\n", duk_safe_to_string(ctx, -1));
    return 0;
}

static void duk_register_fn_log(duk_context* ctx)
{
    duk_push_c_function(ctx, duk_fn_log, DUK_VARARGS);
    duk_put_global_string(ctx, "log");
}

duk_int_t duk_eval_file_extra(duk_context* ctx, const char* path)
{
    duk_int_t rc = duk_compile_file(ctx, path);
    if (rc != 0) {
        return rc;
    }
    duk_register_fn_log(ctx);
    duk_push_global_object(ctx); /* 'this' binding */
    return duk_pcall_method(ctx, 0);
}

#endif