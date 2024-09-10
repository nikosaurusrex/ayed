#pragma once

#include <GLFW/glfw3.h>

struct Window
{
   GLFWwindow *handle;
   int width;
   int height;
};

typedef void (*window_key_callback_fn)(GLFWwindow *window, int key, int scancode, int action, int mods);
typedef void (*window_char_callback_fn)(GLFWwindow *window, unsigned int codepoint);

intern void init_window(Window *window, const char *title);
intern void destroy_window(Window *window);

intern B32 should_close_window(Window *window);
intern void update_window(Window *window);

intern void set_key_callback(Window *window, window_key_callback_fn callback);
intern void set_char_callback(Window *window, window_char_callback_fn callback);
