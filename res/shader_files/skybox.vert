#version 330 core

in vec3 in_position;
in vec3 tex_coord;

uniform mat4 world_matrix;
uniform mat4 projection_matrix;

out vec3 out_tex_coords;

void main()
{
    out_tex_coords = in_position;
    gl_Position = projection_matrix * world_matrix * vec4(in_position, 1.0);
}
