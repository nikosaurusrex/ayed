#pragma once

struct GFX_Shader
{
   GLuint id;
};

intern void init_gfx();

intern GFX_Shader load_gfx_shaders(String8 vert_path, String8 frag_path, Arena *arena);
intern GFX_Shader load_compute_shader(String8 path, Arena *arena);
intern void unload_shader(GFX_Shader shader);
