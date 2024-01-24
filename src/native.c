#include <assert.h>
#include <fcntl.h>
#include <libinput.h>
#include <node_api.h>
#include <stdio.h>
#include <unistd.h>
#include <uv.h>

struct uv_loop_s *loop;
uv_poll_t li_watcher;
struct libinput *li;
napi_ref js_callback_ref;

static int open_restricted(const char *path, int flags, void *user_data) {
    (void)user_data;
    int fd = open(path, flags);
    return fd < 0 ? -errno : fd;
}

static void close_restricted(int fd, void *user_data) {
    (void)user_data;
    close(fd);
}

static const struct libinput_interface interface = {
    .open_restricted = open_restricted,
    .close_restricted = close_restricted,
};

void on_li_readable(uv_poll_t *watcher, int status, int events) {
    (void)status, (void)events;

    if (libinput_dispatch(li) < 0)
        return;

    struct libinput_event *ev;
    while ((ev = libinput_get_event(li))) {
        enum libinput_event_type event_type = libinput_event_get_type(ev);
        switch (event_type) {
        case LIBINPUT_EVENT_POINTER_SCROLL_FINGER: {
            if (!js_callback_ref)
                break;

            napi_env env = watcher->data;

            napi_handle_scope handle_scope;
            napi_open_handle_scope(env, &handle_scope);

            napi_value resource_name, resource_obj;
            napi_create_string_utf8(env, "XXX", NAPI_AUTO_LENGTH,
                                    &resource_name);
            napi_create_object(env, &resource_obj);

            napi_async_context async_ctx;
            napi_async_init(env, resource_obj, resource_name, &async_ctx);

            napi_callback_scope callback_scope;
            napi_open_callback_scope(env, resource_obj, async_ctx,
                                     &callback_scope);

            napi_value js_callback;
            napi_get_reference_value(env, js_callback_ref, &js_callback);

            napi_value undefined, result;
            napi_get_undefined(env, &undefined);
            napi_call_function(env, undefined, js_callback, 0, NULL, &result);

            napi_close_callback_scope(env, callback_scope);
            napi_close_handle_scope(env, handle_scope);

            break;
        }
        default:
            break;
        }
    }
}

napi_value register_callback(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    size_t argc = 1;

    napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    napi_create_reference(env, argv[0], 1, &js_callback_ref);
    puts("callback referenced");

    napi_value undefined, result;
    napi_get_undefined(env, &undefined);
    napi_call_function(env, undefined, argv[0], 0, NULL, &result);
    puts("callback is called for testing");

    return NULL;
}

napi_value init_all(napi_env env, napi_value exports) {
    napi_value fn;
    napi_create_function(env, NULL, 0, register_callback, NULL, &fn);
    napi_set_named_property(env, exports, "registerCallback", fn);

    struct udev *udev = udev_new();
    li = libinput_udev_create_context(&interface, NULL, udev);
    libinput_udev_assign_seat(li, "seat0");

    napi_get_uv_event_loop(env, &loop);

    li_watcher.data = env;
    uv_poll_init(loop, &li_watcher, libinput_get_fd(li));
    uv_poll_start(&li_watcher, UV_READABLE, on_li_readable);

    return exports;
}

NAPI_MODULE("", init_all)