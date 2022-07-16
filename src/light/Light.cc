#include "Light.h"

// ========================
// ===| Help functions |===
// ========================

float vec3_dist(vec3 a, vec3 b)
{
    return pow(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2), 0.5);
}

float calc_attenuation(vec3 camera_pos, vec3 light_pos, float view_radius, vec3 attenuation_params)
{
    float distance = vec3_dist(camera_pos, light_pos);
    distance = std::max(distance-view_radius, 0.0f);
    return 1.0 / (attenuation_params.x + attenuation_params.y * distance + attenuation_params.z * pow(distance, 2));
}

// ===============
// ===| Light |===
// ===============

Light::Light(vec3 color)
    : color{color}
{}

vec3 Light::get_color() const
{
    return color;
}

void Light::set_color(vec3 new_color)
{
    color = new_color;
}


// ===================
// ===| Dir light |===
// ===================

Dir_Light::Dir_Light(vec3 color, vec3 direction)
    : Light(color), direction{direction}
{}

vec3 Dir_Light::get_direction() const
{
    return direction;
}

void Dir_Light::set_direction(vec3 dir)
{
    direction = dir;
}


// ===================
// ===| Pos Light |===
// ===================

Pos_Light::Pos_Light(vec3 color, vec3 position, vec3 attenuation_params)
    : Light(color), position{position}, attenuation_params{attenuation_params}
{}

vec3 Pos_Light::get_position() const
{
    return position;
}

vec3 Pos_Light::get_attenuation_params() const
{
    return attenuation_params;
}

void Pos_Light::set_position(vec3 pos)
{
    position = pos;
}

void Pos_Light::set_attenuation_parameters(vec3 params)
{
    attenuation_params = params;
}


// ====================
// ===| Spot light |===
// ====================

Spot_Light::Spot_Light(vec3 color, vec3 position, vec3 attenuation_params, vec3 direction, GLfloat theta)
    : Pos_Light(color, position, attenuation_params), direction{direction},
      cutoff{vec2{cos(theta), cos(theta + 8 * (float)M_PI / 180)}}
{}

Spot_Light::Spot_Light(vec3 color, vec3 position, vec3 attenuation_params, vec3 direction, vec2 cutoff)
    : Pos_Light(color, position, attenuation_params), direction{direction},
      cutoff{vec2{cos(cutoff.x), cos(cutoff.y)}}
{}

vec2 Spot_Light::get_cutoff() const
{
    return vec2{acos(cutoff.x), acos(cutoff.y)};
}

void Spot_Light::set_cutoff(vec2 cutoff)
{
    cutoff = vec2{cos(cutoff.x), cos(cutoff.y)};
}

vec3 Spot_Light::get_direction() const
{
    return direction;
}

void Spot_Light::set_direction(vec3 dir)
{
    direction = dir;
}


// =========================
// ===| Light Container |===
// =========================

Light_Container::Light_Container()
{}

void Light_Container::add_dir_light(std::shared_ptr<Dir_Light> light)
{
    dir_light_vec.push_back(light);
    add_dir_light_data(light);
}

void Light_Container::add_pos_light(std::shared_ptr<Pos_Light> light)
{
    pos_light_vec.push_back(light);
    add_pos_light_data(light);
}

void Light_Container::add_spot_light(std::shared_ptr<Spot_Light> light)
{
    spot_light_vec.push_back(light);
    add_spot_light_data(light);
}

void Light_Container::add_dir_light_data(std::shared_ptr<Dir_Light> const& light)
{
    light_color_data.push_back(light->get_color());
    light_pos_dir_data.push_back(light->get_direction());
    light_attenuation_data.push_back(vec3 {0.0, 0.0, 0.0});
    light_spot_direction_data.push_back(vec3 {0.0, 0.0, 0.0});
    light_cutoff_data.push_back(vec2 {0.0, 0.0});
    light_type_data.push_back(0);
}

void Light_Container::add_pos_light_data(std::shared_ptr<Pos_Light> const& light)
{
    light_color_data.push_back(light->get_color());
    light_pos_dir_data.push_back(light->get_position());
    light_attenuation_data.push_back(light->get_attenuation_params());
    light_spot_direction_data.push_back(vec3 {0.0, 0.0, 0.0});
    light_cutoff_data.push_back(vec2 {0.0, 0.0});
    light_type_data.push_back(1); 
}

void Light_Container::add_spot_light_data(std::shared_ptr<Spot_Light> const& light)
{
    light_color_data.push_back(light->get_color());
    light_pos_dir_data.push_back(light->get_position());
    light_attenuation_data.push_back(light->get_attenuation_params());
    light_spot_direction_data.push_back(light->get_direction());
    light_cutoff_data.push_back(light->get_cutoff());
    light_type_data.push_back(2);
}

void Light_Container::remove_dir_light(std::shared_ptr<Dir_Light> const& light)
{
    for(auto it = dir_light_vec.begin(); it != dir_light_vec.end(); )
	{
	    if((*it).get() == light.get())
		{
		    dir_light_vec.erase(it);
		    it = dir_light_vec.end();
		}
        else
        {
		    it++;
	    }	
	}
    update_shader_data();
}

void Light_Container::remove_pos_light(std::shared_ptr<Pos_Light> const& light)
{
    for(auto it = pos_light_vec.begin(); it != pos_light_vec.end(); )
	{
	    if((*it).get() == light.get())
		{
		    pos_light_vec.erase(it);
		    it = pos_light_vec.end();
		}
        else
        {
		    it++;
	    }	
	}
    update_shader_data();
}

void Light_Container::remove_spot_light(std::shared_ptr<Spot_Light> const& light)
{
    for(auto it = spot_light_vec.begin(); it != spot_light_vec.end(); )
	{
	    if((*it).get() == light.get())
		{
		    spot_light_vec.erase(it);
		    it = spot_light_vec.end();
		}
        else
        {
		    it++;
	    }	
	}
    update_shader_data();
}

// Updates all arrays of data that will be used in shader
void Light_Container::update_shader_data()
{
    light_color_data.clear();
    light_pos_dir_data.clear();
    light_attenuation_data.clear();
    light_spot_direction_data.clear();
    light_cutoff_data.clear();
    light_type_data.clear();
  
    for(uint i = 0; i < dir_light_vec.size(); i++)
	{
	    add_dir_light_data(dir_light_vec[i]);
	}
    for(uint i = 0; i < pos_light_vec.size(); i++)
	{
	    add_pos_light_data(pos_light_vec[i]);
	}
    for(uint i = 0; i < spot_light_vec.size(); i++)
	{
	    add_spot_light_data(spot_light_vec[i]);
	}
}

/// Updates all arrays of data that will be used in shader, like above.
// This however excludes all lights that do not fullfill the condition put up
// by frustum_depth and attenuation_cutoff (i.e the lights that are too far away)
void Light_Container::update_active_shader_data(vec3 camera_pos, float frustum_depth, float attenuation_cutoff)
{
    light_color_data.clear();
    light_pos_dir_data.clear();
    light_attenuation_data.clear();
    light_spot_direction_data.clear();
    light_cutoff_data.clear();
    light_type_data.clear();
  
    for(uint i = 0; i < dir_light_vec.size(); i++)
	{
	    add_dir_light_data(dir_light_vec[i]);
	}

    for(uint i = 0; i < pos_light_vec.size(); i++)
	{
	    float attenuation = calc_attenuation(camera_pos, pos_light_vec[i]->get_position(),
						    frustum_depth, pos_light_vec[i]->get_attenuation_params());

        // An if-clause determining wether we are close enough to the light source for it to make impact
		// on the world
	    if(attenuation > attenuation_cutoff)
		{
		    add_pos_light_data(pos_light_vec[i]);
		}
	}

    for(uint i = 0; i < spot_light_vec.size(); i++)
	{
	    float attenuation = calc_attenuation(camera_pos, spot_light_vec[i]->get_position(),
						    frustum_depth, spot_light_vec[i]->get_attenuation_params());

	    if(attenuation > attenuation_cutoff)
		{
		    add_spot_light_data(spot_light_vec[i]);
		}
	}
}

int Light_Container::get_number_of_lights() const
{
    return dir_light_vec.size() + pos_light_vec.size() + spot_light_vec.size();
}

int Light_Container::get_number_of_active_lights() const
{
    return light_pos_dir_data.size();
}

const std::vector<vec3>& Light_Container::get_light_color_data() const
{
    return light_color_data;
}

const std::vector<vec3>& Light_Container::get_light_pos_dir_data() const
{
    return light_pos_dir_data;
}

const std::vector<vec3>& Light_Container::get_light_attenuation_data() const
{
    return light_attenuation_data;
}

const std::vector<vec3>& Light_Container::get_light_spot_direction_data() const
{
    return light_spot_direction_data;
}

const std::vector<vec2>& Light_Container::get_light_cutoff_data() const
{
    return light_cutoff_data;
}

const std::vector<int>& Light_Container::get_light_type_data() const
{
    return light_type_data;
}
