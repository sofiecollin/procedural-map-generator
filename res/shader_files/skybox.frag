#version 330 core

in vec3 out_tex_coords;

uniform samplerCube tex_unit;

out vec4 out_color;

void main()
{
    out_color = texture(tex_unit, out_tex_coords);
}
