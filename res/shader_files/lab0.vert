#version 150

in vec3 in_position;
in vec3 in_normal;
in vec2 in_tex_coord;

uniform mat4 model_matrix;
uniform mat4 world_matrix;
uniform mat4 projection_matrix;

out float shade;

void main(void)
{
	shade = (mat3(world_matrix * model_matrix) * in_normal).z; // Fake shading
	gl_Position = projection_matrix * world_matrix * model_matrix * vec4(in_position, 1.0);
}

