#include "gfx.h"

intern void
check_opengl_error()
{
   GLenum error = glGetError();
   if (error != GL_NO_ERROR) {
      log_fatal("OpenGL error: %s", glewGetErrorString(error));
   }
}

intern void
print_shader_log(GLuint shader)
{
   int   length = 0, chars_written = 0;
   char *log;
   glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

   if (length <= 0) {
     return;
   }

   log = (char *)malloc(length);
   glGetShaderInfoLog(shader, length, &chars_written, log);
   log_fatal("Shader Info Log: %s", log);

   free(log);
}

intern void
print_program_log(GLuint program)
{
   int   length = 0, chars_written = 0;
   char *log;
   glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

   if (length <= 0) {
     return;
   }

   log = (char *)malloc(length);
   glGetProgramInfoLog(program, length, &chars_written, log);
   log_fatal("Program Info Log: %s", log);

   free(log);
}

intern GLuint
compile_shader(String8 src, GLenum type, const char *stage_name)
{
   char *c_str = cstr_from_str8(src);

   GLuint shader = glCreateShader(type);
   glShaderSource(shader, 1, (const GLchar *const *)&c_str, 0);

   GLint success = 0;

   glCompileShader(shader);
   check_opengl_error();
   glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
   if (success != 1) {
      print_shader_log(shader);
      log_fatal("Failed to compile %s shader", stage_name);
   }

   free(c_str);

   return shader;
}

void
init_gfx()
{
   if (glewInit() != GLEW_OK) {
      log_fatal("Failed to initialize GLEW");
   }
}

GFX_Shader
load_gfx_shaders(String8 vert_path, String8 frag_path, Arena *arena)
{
   String8 vert_src = os_read_file(vert_path, arena);
   String8 frag_src = os_read_file(frag_path, arena);

   if (!vert_src.ptr) {
      log_fatal("File not found: %.*s", (int)vert_path.len, vert_path.ptr);
   }

   if (!frag_src.ptr) {
      log_fatal("File not found: %.*s", (int)frag_path.len, frag_path.ptr);
   }

   GLuint vert_shader = compile_shader(vert_src, GL_VERTEX_SHADER, "vertex");
   GLuint frag_shader = compile_shader(frag_src, GL_FRAGMENT_SHADER, "fragment");

   GLuint program = glCreateProgram();

   glAttachShader(program, vert_shader);
   glAttachShader(program, frag_shader);

   glLinkProgram(program);
   check_opengl_error();

   GLint success = 0;
   glGetProgramiv(program, GL_LINK_STATUS, &success);
   if (success != 1) {
      print_program_log(program);
      log_fatal("Failed to link shader program");
   }

   glDeleteShader(vert_shader);
   glDeleteShader(frag_shader);

   return GFX_Shader(program);
}

GFX_Shader
load_compute_shader(String8 path, Arena *arena)
{
   String8 src = os_read_file(path, arena);

   if (!src.ptr) {
      log_fatal("File not found: %.*s", (int)path.len, path.ptr);
   }

   GLuint shader = compile_shader(src, GL_COMPUTE_SHADER, "compute");

   GLuint program = glCreateProgram();

   glAttachShader(program, shader);

   glLinkProgram(program);
   check_opengl_error();

   GLint success = 0;
   glGetProgramiv(program, GL_LINK_STATUS, &success);
   if (success != 1) {
      print_program_log(program);
      log_fatal("Failed to link shader program");
   }

   glDeleteShader(shader);

   return GFX_Shader(program);
}

void
unload_shader(GFX_Shader shader)
{
   glDeleteProgram(shader.id);
}


