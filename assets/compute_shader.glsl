#version 450 core

layout(local_size_x=16, local_size_y=16) in;
layout(rgba32f, binding=0) uniform image2D output_texture;

void
main()
{
   uvec2 pixel = gl_GlobalInvocationID.xy;
}
