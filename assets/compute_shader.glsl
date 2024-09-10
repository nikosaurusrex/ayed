#version 450 core

layout(local_size_x=16, local_size_y=16) in;
layout(rgba32f, binding=0) uniform image2D output_texture;

void
main()
{
   ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);

   vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
   imageStore(output_texture, pixel, color);
}
