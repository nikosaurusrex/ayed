#pragma once

#include <GLFW/glfw3.h>

typedef void (*WindowKeyCallbackFn)(void *ctx, int key, int scancode, int action, int mods);
typedef void (*WindowCharCallbackFn)(void *ctx, unsigned int codepoint);
typedef void (*WindowResizeCallbackFn)(void *ctx, int width, int height);

struct WindowCallbacks
{
   void *ctx;
   WindowKeyCallbackFn key;
   WindowCharCallbackFn chr;
   WindowResizeCallbackFn resize;
};

struct Window
{
   GLFWwindow *handle;
   WindowCallbacks callbacks;
   int width;
   int height;
};

intern void init_window(Window *window, const char *title);
intern void destroy_window(Window *window);

intern B32 should_close_window(Window *window);
intern void update_window(Window *window);

intern void set_window_callbacks(Window *window, WindowCallbacks callbacks);
