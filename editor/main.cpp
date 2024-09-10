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

struct RenderSize
{
   U32 columns;
   U32 rows;

   GLuint cell_size_loc;
   GLuint grid_size_loc;
};

struct WinEventCtx
{
   RenderSize *render_size;
   OutputTexture *output_texture;
   GFX_Shader compute_shader;
   GlyphMap glyph_map;
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

intern void
init_render_size(RenderSize *rs, GFX_Shader s)
{
   glUseProgram(s.id);

   rs->cell_size_loc = glGetUniformLocation(s.id, "cell_size");
   rs->grid_size_loc = glGetUniformLocation(s.id, "grid_size");

   glUseProgram(0);
}

intern void
update_render_size(RenderSize *rs, GlyphMap gm, GFX_Shader s, U32 window_width, U32 window_height)
{
   GlyphMetrics m = gm.metrics; 

   glUseProgram(s.id);

   U32 cols = (U32)floorf((float)window_width / m.width);
   U32 rows = (U32)floorf((float)window_height / m.height);

   rs->columns = cols;
   rs->rows = rows;

   glUniform2ui(rs->cell_size_loc, m.width, m.height);
   glUniform2ui(rs->grid_size_loc, rs->columns, rs->rows);

   glUseProgram(0);
}

intern OutputTexture
create_output_texture()
{
   OutputTexture tex = {};

   glGenTextures(1, &tex.id);

   return tex;
}

intern void
resize_output_texture(OutputTexture *ot, U32 width, U32 height)
{
   ot->width = width;
   ot->height = height;

   glBindTexture(GL_TEXTURE_2D, ot->id);
   glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
   glBindTexture(GL_TEXTURE_2D, 0);
}

intern void
on_resize(void *_ctx, int width, int height)
{
   WinEventCtx *ctx = (WinEventCtx *) _ctx;

   RenderSize *rs = ctx->render_size;
   OutputTexture *ot = ctx->output_texture;
   GlyphMap gm = ctx->glyph_map;
   GFX_Shader cs = ctx->compute_shader;

   glViewport(0, 0, width, height);

   resize_output_texture(ot, width, height);
   update_render_size(rs, gm, cs, width, height);
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

   OutputTexture output_texture = create_output_texture();

   RenderSize render_size = {};
   init_render_size(&render_size, compute_shader);

   WinEventCtx win_event_ctx = {};
   win_event_ctx.render_size = &render_size;
   win_event_ctx.output_texture = &output_texture;
   win_event_ctx.glyph_map = glyph_map;
   win_event_ctx.compute_shader = compute_shader;

   WindowCallbacks win_callbacks = {};
   win_callbacks.ctx = &win_event_ctx;
   win_callbacks.resize = on_resize;

   set_window_callbacks(&window, win_callbacks);
   on_resize(&win_event_ctx, window.width, window.height);

   U64 fps = 0;
   double last_time_fps = glfwGetTime();

   while (!should_close_window(&window)) {
      glClear(GL_COLOR_BUFFER_BIT);

      render_to_texture(compute_shader, output_texture);
      render_to_screen(renderer, output_texture);

      double now_time_fps = glfwGetTime();
      double delta_time_fps = now_time_fps - last_time_fps;
      if (delta_time_fps >= 1.0) {
         char buf[128];

         double mspf = (delta_time_fps * 1000) / (double)fps;

         sprintf(buf, "Ayed %llu FPS, %f ms/f", fps, mspf);
         glfwSetWindowTitle(window.handle, buf);
         last_time_fps = now_time_fps;
         fps = 0;
      }

      fps++;

      update_window(&window);
   }

   destroy_renderer(renderer);
   unload_shader(compute_shader);
   release_freetype(freetype);
   destroy_window(&window);

   return 0;
}
