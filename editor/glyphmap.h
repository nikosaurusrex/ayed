#pragma once

struct GlyphMetrics
{
   U32 width;
   U32 height;
   U32 descender;
};

struct GlyphMap
{
   U8 *data;
   U32 width;
   U32 height;
   GlyphMetrics metrics;
};

intern FT_Library init_freetype();
intern void release_freetype(FT_Library lib);
intern GlyphMap load_glyphmap(Arena *arena, const char *font_name, U32 font_size, FT_Library freetype);

intern U32 load_glyph(GlyphMap *gm, U32 codepoint);
