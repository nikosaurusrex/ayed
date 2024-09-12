#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "vendor/stb/stb_image_write.h"

#include "base/base_inc.h"

#include "glyphmap.h"

enum
{
   GM_COUNT_X = 32,
   GM_COUNT_Y = 16,
};

intern GlyphMetrics
calculate_font_metrics(FT_Face face)
{
   GlyphMetrics metrics = {};

   FT_UInt glyph_index = FT_Get_Char_Index(face, 'M');
   FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

   metrics.descender = -face->size->metrics.descender >> 6;
   metrics.width = face->glyph->advance.x >> 6;
   metrics.height = (face->size->metrics.ascender >> 6) + metrics.descender;

   return metrics;
}

FT_Library
init_freetype()
{
   FT_Library lib = {};

   FT_Error err = FT_Init_FreeType(&lib);
   if (err != 0) {
      log_fatal("Failed to initialize FreeType\n");
   }

   FT_Library_SetLcdFilter(lib, FT_LCD_FILTER_DEFAULT);

   return lib;
}

void
release_freetype(FT_Library lib)
{
   FT_Done_FreeType(lib);
}

GlyphMap
load_glyphmap(Arena *arena, const char *font_name, U32 font_size, FT_Library freetype)
{
   GlyphMap gm = {};

   FT_Error err;

   FT_Face face;
   err = FT_New_Face(freetype, font_name, 0, &face);
   if (err != 0) {
      log_fatal("Failed to load face\n");
   }

   err = FT_Set_Pixel_Sizes(face, 0, font_size);
   if (err != 0) {
      log_fatal("Failed to set pixel size\n");
   }

   gm.metrics = calculate_font_metrics(face);

   gm.width = gm.metrics.width * GM_COUNT_X * 3;
   gm.height = gm.metrics.height * GM_COUNT_Y;
   gm.data = push_array(arena, U8, gm.width * gm.height);

   // Load ascii chars
   U32 ascii_chars_count = ('~' - ' ') + 1;

   for (U32 i = 0; i < ascii_chars_count; ++i) {
      U32 codepoint = (U32)(' ' + i);

      FT_UInt glyph_index = FT_Get_Char_Index(face, codepoint);
      FT_Int32 load_flags = FT_LOAD_TARGET_LCD;
      err = FT_Load_Glyph(face, glyph_index, load_flags);
      if (err != 0) {
         log_fatal("Failed to load glyph\n");
      }

      FT_Render_Mode render_flags = FT_RENDER_MODE_LCD;
      err = FT_Render_Glyph(face->glyph, render_flags);
      if (err != 0) {
         log_fatal("Failed to render glyph\n");
      }
   
      FT_GlyphSlot g = face->glyph;
      FT_Bitmap bm = face->glyph->bitmap;

      U32 l = g->bitmap_left;
      S32 t_signed = (S32)(gm.metrics.height - gm.metrics.descender) - g->bitmap_top;
      t_signed = CLAMP_BOT(t_signed, 0);
      U32 t = (U32)t_signed;

      U32 tx = i % GM_COUNT_X;
      U32 ty = i / GM_COUNT_X;

      U32 sx = tx * gm.metrics.width * 3 + l * 3;
      U32 sy = ty * gm.metrics.height + t;

      for (U32 py = 0; py < bm.rows; ++py) {
         for (U32 px = 0; px < bm.width; px += 3) {
            U32 row = py * bm.pitch;
            U8 r = bm.buffer[px + 0 + row];
            U8 g = bm.buffer[px + 1 + row];
            U8 b = bm.buffer[px + 2 + row];

            U32 x = sx + px;
            U32 y = sy + py;

            U32 drow = y * gm.width;
            gm.data[x + 0 + drow] = r;
            gm.data[x + 1 + drow] = g;
            gm.data[x + 2 + drow] = b;
         }
      }
   }

   stbi_write_jpg("out.jpg", gm.width / 3, gm.height, 3, gm.data, 100);

   return gm;
}
