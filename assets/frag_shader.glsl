#version 450 core

in vec2 p_uv;

out vec4 o_color;

uniform sampler2D tex_text;

void
main()
{
   vec2 uv = vec2(p_uv.x, 1.0 - p_uv.y);
   o_color = texture(tex_text, uv);
}
