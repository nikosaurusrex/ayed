#version 450 core

layout(local_size_x=16, local_size_y=16, local_size_z=1) in;
layout(rgba32f, binding=0) uniform image2D output_texture;

uniform uvec2 cell_size;
uniform uvec2 grid_size;

const uint GLYPH_INVERT = 0x1;
const uint GLYPH_BLINK = 0x2;

struct Cell
{
   uint glyph;
   uint fg;
   uint bg;
};

layout(std430, binding=3) buffer cells_buffer {
   Cell cells[];
};

layout(binding=1) uniform sampler2D glyph_map;

vec3 unpack_color(uint c)
{
   int r = int((c >> 16u) & 0xffu);
   int g = int((c >> 8u) & 0xffu);
   int b = int(c & 0xffu);
   return vec3(r, g, b) / 255.0;
}

void
main()
{
   uvec2 pixel = gl_GlobalInvocationID.xy;

   if (pixel.x >= cell_size.x * grid_size.x || pixel.y >= cell_size.y * grid_size.y) {
      return;
   }

   uvec2 cell_index = pixel / cell_size; 
   uvec2 cell_pos = pixel % cell_size;

   Cell cell = cells[cell_index.x + cell_index.y * grid_size.x];

   uvec2 glyph_pos = uvec2((cell.glyph >> 16) & 0xffffu, cell.glyph & 0xffffu) * cell_size;

   uvec2 pixel_pos = glyph_pos + cell_pos;

   vec4 texel = texelFetch(glyph_map, ivec2(pixel_pos), 0);

   vec3 fg = unpack_color(cell.fg);
   vec3 bg = unpack_color(cell.bg);

   uint flags = (cell.bg >> 24u) & 0xffu;
   vec3 invert = vec3(flags & GLYPH_INVERT);

   fg = abs(invert - fg);
   bg = abs(invert - bg);

   vec3 color = mix(bg, fg, texel.rgb);

   vec3 corrected_color = pow(color, vec3(1.0 / 1.8));

   imageStore(output_texture, ivec2(pixel), vec4(corrected_color, 1.0));
}
