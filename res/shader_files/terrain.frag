#version 150

in vec3 out_normal;
in vec2 out_tex_coord;
in vec3 out_surface_position;

out vec4 out_color;

// Material
uniform sampler2D object_texture;
uniform sampler2D object_texture1;
uniform sampler2D object_texture2;
uniform sampler2D specularity_map;
uniform bool use_specularity_map;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float a;

// Light
uniform vec3 camera_pos;
uniform int num_of_lights;
uniform vec3 light_pos_dir[30]; // Max num. of lights is currently 30 (arbitrary limit)
uniform vec3 light_color[30];
uniform vec3 light_attenuation_params[30];
uniform vec3 light_spot_dir[30]; // Direction needed for spotlights
uniform int light_type[30];
uniform vec2 light_cutoff[30];
uniform vec4 lightning_params;

vec3 light_dir;
vec3 specular_light;
float attenuation;

void main(void)
{
	vec3 temp_color = vec3(0.0, 0.0, 0.0);
	vec3 normal = normalize(out_normal);

	for(int i = 0; i < num_of_lights; i++)
	{
		if(light_type[i]==0)
		{
			light_dir = -normalize(light_pos_dir[i]);
			attenuation = 1.0;
		}
		else if(light_type[i]==1 || light_type[i]==2)
		{
			light_dir = normalize(light_pos_dir[i]-out_surface_position);
			float constant = light_attenuation_params[i].x;
 			float linear = light_attenuation_params[i].y;	
 			float quadratic = light_attenuation_params[i].z;

			// Calculate attenuation
			float distance = distance(light_pos_dir[i], out_surface_position);
			attenuation = 1.0/(constant+linear*distance+quadratic*pow(distance,2));

			// To my understanding glsl evaluates everything so we loose no performance
			// by having this if-clause here instead of higher up and restructuring the
			// program
			if(light_type[i]==2)
			{
				float theta = dot(light_dir, normalize(-light_spot_dir[i]));
				float epsilon = light_cutoff[i].x - light_cutoff[i].y;
				attenuation = clamp(((1-light_cutoff[i].y)-theta)/epsilon,0.0,1.0);
			}
		}

		

		// Ambient
		vec3 ambient_light = ka / 5 * light_color[i];

		// Diffuse
		float diffuse = max(dot(normal, light_dir), 0.0);
		vec3 diffuse_light = kd * diffuse * light_color[i];

		// Specular
	   	vec3 view_dir = normalize(camera_pos - out_surface_position);
	    	vec3 reflection = reflect(-light_dir, normal);
		float specular = max(dot(reflection, view_dir), 0.0);

		if(use_specularity_map)
		{
			vec3 spec_map = texture(specularity_map, out_tex_coord).rgb;
			specular_light = ks * pow(specular, a) * spec_map * light_color[i];
		}
		else
		{
			specular_light = ks * pow(specular, a) * light_color[i];
		}
		
		temp_color = temp_color + (ambient_light + diffuse_light + specular_light) * attenuation;
	}
	
	vec4 tex1;
	vec4 tex2;
	vec4 tot_tex;
	float blend_factor;
	float slope = 1.0 - normal.y;
	float lower_bound = 0.2;
	float upper_bound = 0.7;

	if(slope < lower_bound)
	{
		blend_factor = clamp(slope / lower_bound, 0, 1);
		tex1 = texture(object_texture, out_tex_coord);
		tex2 = texture(object_texture1, out_tex_coord);
		tot_tex = mix(tex1, tex2, blend_factor);		
	}
	else if(slope >= lower_bound && slope < upper_bound)
	{
		blend_factor = clamp((slope- lower_bound) * (1.0/ (upper_bound - lower_bound)), 0, 1);
		tex1 = texture(object_texture1, out_tex_coord);
		tex2 = texture(object_texture2, out_tex_coord);
		tot_tex = mix(tex1, tex2, blend_factor);
	}
	else
	{
		tot_tex = texture(object_texture2, out_tex_coord);
	}

	out_color = vec4(temp_color, 1.0) * tot_tex;

}


