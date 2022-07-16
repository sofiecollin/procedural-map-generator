#version 150

in vec3 in_position;
in vec3 in_normal;
in vec2 in_tex_coord;

out vec3 out_normal;
out vec2 out_tex_coord;
out vec3 out_surface_position; // Ingemar calls surface, some call fragment position

uniform mat4 model_matrix;
uniform mat4 world_matrix;
uniform mat4 projection_matrix;

void main(void)
{
	mat3 normal_matrix = mat3(model_matrix);
	out_normal = normal_matrix * in_normal;
	out_tex_coord = in_tex_coord;
	out_surface_position = vec3(model_matrix * vec4(in_position, 1.0));
	gl_Position = projection_matrix * world_matrix * model_matrix * vec4(in_position, 1.0);
}