#version 450 core

layout(local_size_x=16, local_size_y=16) in;
layout(rgba32f, binding=0) uniform image2D output_texture;

uniform uvec2 cell_size;
uniform uvec2 grid_size;

void
main()
{
   ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
   pixel.y = imageSize(output_texture).y - 1 - pixel.y;

   if (pixel.x >= cell_size.x * grid_size.x || pixel.y < 0) {
      return;
   }

   vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
   imageStore(output_texture, pixel, color);
}
