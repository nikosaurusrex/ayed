#include "window.h"

intern void
window_on_frame_buffer_resize(GLFWwindow *handle, int width, int height)
{
   Window *window = (Window *)glfwGetWindowUserPointer(handle);
   ASSERT(window);

   window->width = width;
   window->height = height;

   WindowCallbacks callbacks = window->callbacks;
   WindowResizeCallbackFn callback = callbacks.resize;

   if (callback != 0) {
      callback(callbacks.ctx, width, height);
   }
}

intern void
window_on_key_event(GLFWwindow *handle, int key, int scancode, int action, int mods)
{
   Window *window = (Window *)glfwGetWindowUserPointer(handle);
   ASSERT(window);

   WindowCallbacks callbacks = window->callbacks;
   WindowKeyCallbackFn callback = callbacks.key;

   if (callback != 0) {
      callback(callbacks.ctx, key, scancode, action, mods);
   }
}

intern void
window_on_char_event(GLFWwindow *handle, unsigned int codepoint)
{
   Window *window = (Window *)glfwGetWindowUserPointer(handle);
   ASSERT(window);

   WindowCallbacks callbacks = window->callbacks;
   WindowCharCallbackFn callback = callbacks.text;

   if (callback != 0) {
      callback(callbacks.ctx, codepoint);
   }
}

void
init_window(Window *window, const char *title)
{
   window->callbacks = {};

   if (!glfwInit()) {
      log_fatal("Failed to initialize GLFW");
   }

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

   const int width = 1280;
   const int height = 720;

   window->handle = glfwCreateWindow(width, height, title, 0, 0);
   if (!window->handle) {
      log_fatal("Failed to create window");
   }

   glfwMakeContextCurrent(window->handle);

   const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
   glfwSetWindowPos(window->handle, (mode->width - width) / 2, (mode->height - height) / 2);

   glfwSetWindowUserPointer(window->handle, window);

   glfwGetFramebufferSize(window->handle, &window->width, &window->height);

   glfwSetFramebufferSizeCallback(window->handle, window_on_frame_buffer_resize);
   glfwSetKeyCallback(window->handle, window_on_key_event);
   glfwSetCharCallback(window->handle, window_on_char_event);
}

void
destroy_window(Window *window)
{
   glfwDestroyWindow(window->handle);
   glfwTerminate();
}

B32
should_close_window(Window *window)
{
   return glfwWindowShouldClose(window->handle);
}

void
update_window(Window *window)
{
   glfwPollEvents();
   glfwSwapBuffers(window->handle);
}

void
set_window_callbacks(Window *window, WindowCallbacks callbacks)
{
   window->callbacks = callbacks;
}
