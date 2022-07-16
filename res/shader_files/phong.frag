#version 150

in vec3 out_normal;
in vec2 out_tex_coord;
in vec3 out_surface_position;

out vec4 out_color;

uniform vec3 light_pos;
uniform vec3 camera_pos;
uniform vec3 light_color;
uniform vec4 lightning_params;

void main(void)
{
	float ka = lightning_params.x;
	float kd = lightning_params.y;
	float ks = lightning_params.z;
	float a = lightning_params.w;
				
	vec3 normal = normalize(out_normal);
	vec3 light_dir = -normalize(light_pos);
	
	// Diffuse
	float diffuse = max(dot(normal, light_dir), 0.0);

	// Specular
	vec3 view_dir = normalize(camera_pos - out_surface_position);
	vec3 reflection = reflect(-light_dir, normal);
	float specular = max(dot(reflection, view_dir), 0.0);

	float shade = ka+kd*diffuse+ks*pow(specular, a);
	out_color = vec4(shade*light_color, 1.0);
}