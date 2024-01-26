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

napi_async_context async_ctx;
napi_ref resource_ref;

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
            if (!resource_ref)
                break;
            napi_env env = watcher->data;
            napi_handle_scope handle_scope;
            napi_open_handle_scope(env, &handle_scope);

            napi_value resource, cb;
            napi_get_reference_value(env, resource_ref, &resource);
            napi_get_named_property(env, resource, "cb", &cb);

            napi_make_callback(env, async_ctx, resource, cb, 0, NULL, NULL);

            napi_close_handle_scope(env, handle_scope);

            break;
        }
        default:
            break;
        }
    }
}

napi_value register_callback(napi_env env, napi_callback_info info) {
    napi_value resource, name;

    napi_create_object(env, &resource);

    napi_create_string_utf8(env, "Libinput", NAPI_AUTO_LENGTH, &name);
    napi_set_named_property(env, resource, "name", name);

    napi_value argv[1];
    size_t argc = 1;
    napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    napi_set_named_property(env, resource, "cb", argv[0]);

    napi_create_reference(env, resource, 1, &resource_ref);

    napi_async_init(env, resource, name, &async_ctx);

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
