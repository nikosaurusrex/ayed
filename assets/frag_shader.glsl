#version 450 core

in vec2 p_uv;

out vec4 o_color;

uniform sampler2D tex_text;

void
main()
{
   o_color = texture(tex_text, p_uv);
}
