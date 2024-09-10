#include "window.h"

intern void
on_frame_buffer_resize(GLFWwindow *handle, int width, int height)
{
   Window *window = (Window *)glfwGetWindowUserPointer(handle);
   ASSERT(window);

   window->width = width;
   window->height = height;
}

void
init_window(Window *window, const char *title)
{
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
   glfwSetFramebufferSizeCallback(window->handle, on_frame_buffer_resize);
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
set_key_callback(Window *window, window_key_callback_fn callback)
{
   glfwSetKeyCallback(window->handle, callback);
}

void
set_char_callback(Window *window, window_char_callback_fn callback)
{
   glfwSetCharCallback(window->handle, callback);
}
