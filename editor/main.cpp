#define GLEW_STATIC
#include <GL/glew.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H

#include "base/base_inc.h"
#include "window.h"
#include "gfx.h"
#include "glyphmap.h"

#include "base/base_inc.cpp"
#include "window.cpp"
#include "gfx.cpp"
#include "glyphmap.cpp"

struct Renderer
{
   GLuint vao;
   GLuint vbo;
   GLuint ebo;
   GFX_Shader shader;
};

struct OutputTexture
{
   GLuint id;
   U32 width;
   U32 height;
};

intern Renderer
create_renderer(Arena *arena)
{
   static const float vertices[] = {
      // 3d pos + 2d tex coord
      1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
      -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
      -1.0f, 1.0f, 1.0f, 0.0f, 1.0f
   };

   static const U32 indices[] = {
      0, 1, 3,
      1, 2, 3
   };

   Renderer r = {};

   glGenVertexArrays(1, &r.vao);
   glGenBuffers(1, &r.vbo);
   glGenBuffers(1, &r.ebo);

   glBindVertexArray(r.vao);
   
   glBindBuffer(GL_ARRAY_BUFFER, r.vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r.ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);

   glBindVertexArray(0);

   r.shader = load_gfx_shaders(str8_from_cstr("assets/vert_shader.glsl"), str8_from_cstr("assets/frag_shader.glsl"), arena);

   glUseProgram(r.shader.id);
   glUniform1i(glGetUniformLocation(r.shader.id, "tex_text"), 0);
   glUseProgram(0);

   return r;
}

intern void
destroy_renderer(Renderer r)
{
   unload_shader(r.shader);

   glDeleteVertexArrays(1, &r.vao);
   glDeleteBuffers(1, &r.vbo);
   glDeleteBuffers(1, &r.ebo);
}

intern void
render_to_screen(Renderer r, OutputTexture tex)
{
   glUseProgram(r.shader.id);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, tex.id);

   glBindVertexArray(r.vao);

   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

   glBindVertexArray(0);
   glUseProgram(0);
}

intern void
render_to_texture(GFX_Shader compute_shader, OutputTexture tex)
{
   glUseProgram(compute_shader.id);

   glBindImageTexture(0, tex.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

   U32 dx = (tex.width + 15) / 16;
   U32 dy = (tex.height + 15) / 16;
   glDispatchCompute(dx, dy, 1);

   glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

   glUseProgram(0);
}

intern OutputTexture
create_output_texture(GlyphMap gm, U32 window_width, U32 window_height)
{
   OutputTexture tex = {};

   U32 w = window_width;
   U32 h = window_height;

   U32 gx = 8;
   U32 gy = 8;

   tex.width = gx * gm.metrics.width;
   tex.height = gy * gm.metrics.height;

   glGenTextures(1, &tex.id);
   glBindTexture(GL_TEXTURE_2D, tex.id);
   glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, tex.width, tex.height);
   glBindTexture(GL_TEXTURE_2D, 0);

   return tex;
}

int
main(int argc, char **argv)
{
   Arena arena = {};
   init_arena(&arena, GIGA_BYTES(1));
   
   FT_Library freetype = init_freetype();
   GlyphMap glyph_map = load_glyphmap(&arena, "assets/consolas.ttf", freetype);

   Window window = {};
   init_window(&window, "Ayed");

   init_gfx();

   GFX_Shader compute_shader = load_compute_shader(str8_from_cstr("assets/compute_shader.glsl"), &arena);
   Renderer renderer = create_renderer(&arena);

   // TODO: handle resize
   OutputTexture output_texture = create_output_texture(glyph_map, window.width, window.height);

   while (!should_close_window(&window)) {
      glClear(GL_COLOR_BUFFER_BIT);

      render_to_texture(compute_shader, output_texture);
      render_to_screen(renderer, output_texture);

      update_window(&window);
   }

   destroy_renderer(renderer);
   unload_shader(compute_shader);
   release_freetype(freetype);
   destroy_window(&window);

   return 0;
}
