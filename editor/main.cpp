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

int
main(int argc, char **argv)
{
   Arena arena = {};
   init_arena(&arena, GIGA_BYTES(1));
   
   FT_Library freetype = init_freetype();
   load_glyphmap(&arena, "assets\\consolas.ttf", freetype);

   /*
   Window window = {};
   init_window(&window, "Ayed");

   init_gfx();

   while (!should_close_window(&window)) {
      update_window(&window);
   }

   destroy_window(&window);*/
   release_freetype(freetype);

   return 0;
}
