#ifndef LIGHT_H
#define LIGHT_H

#include <vector>
#include <memory>

#include "VectorUtils3.h"

class Light
{
public:
    Light(vec3 color);

    vec3 get_color() const;
    void set_color(vec3 new_color);

protected:
    vec3 color;
};

class Dir_Light : public Light
{
public:
    Dir_Light(vec3 color, vec3 direction);
    
    vec3 get_direction() const;
    void set_direction(vec3 dir);
  
private:
    vec3 direction;
};

class Pos_Light : public Light
{
public:
    Pos_Light(vec3 color, vec3 position, vec3 attenuation_params);
    
    vec3 get_position() const;
    vec3 get_attenuation_params() const;
    void set_position(vec3 pos);
    void set_attenuation_parameters(vec3 params);

private:
    vec3 position;
    vec3 attenuation_params;
};

class Spot_Light : public Pos_Light
{
 public:
    Spot_Light(vec3 color, vec3 position, vec3 attenuation_params, vec3 direction, GLfloat theta);
    Spot_Light(vec3 color, vec3 position, vec3 attenuation_params, vec3 direction, vec2 cutoff);
    
    vec2 get_cutoff() const;
    void set_cutoff(vec2 cutoff);
    vec3 get_direction() const;
    void set_direction(vec3 dir);

 private:
    vec3 direction;
    vec2 cutoff; /* vec2(inner cutoff, outer cutoff)
		            Angle is given as degrees but is converted to cos.
		            This is because it is more efficient for the shader
		            to not have to do that calculation*/
};

class Light_Container
{
public:
    Light_Container();

    void add_dir_light    (std::shared_ptr<Dir_Light>  light);
    void add_pos_light    (std::shared_ptr<Pos_Light>  light);
    void add_spot_light   (std::shared_ptr<Spot_Light> light);
    void remove_dir_light (std::shared_ptr<Dir_Light>  const& light);
    void remove_pos_light (std::shared_ptr<Pos_Light>  const& light);
    void remove_spot_light(std::shared_ptr<Spot_Light> const& light);
  
    const std::vector<vec3>& get_light_color_data()          const;
    const std::vector<vec3>& get_light_pos_dir_data()        const;
    const std::vector<vec3>& get_light_attenuation_data()    const;
    const std::vector<vec3>& get_light_spot_direction_data() const;
    const std::vector<vec2>& get_light_cutoff_data()         const;
    const std::vector<int> & get_light_type_data()           const;
  
    void update_shader_data();
    void update_active_shader_data(vec3 camera_pos, float frustum_depth, float attenuation_cutoff);
    int get_number_of_lights() const;
    int get_number_of_active_lights() const;

private:
    void add_dir_light_data (std::shared_ptr<Dir_Light>  const& light);
    void add_pos_light_data (std::shared_ptr<Pos_Light>  const& light);
    void add_spot_light_data(std::shared_ptr<Spot_Light> const& light);
    
    std::vector<std::shared_ptr<Dir_Light>>  dir_light_vec;
    std::vector<std::shared_ptr<Pos_Light>>  pos_light_vec;
    std::vector<std::shared_ptr<Spot_Light>> spot_light_vec;
    
    std::vector<vec3> light_color_data;
    std::vector<vec3> light_pos_dir_data;
    std::vector<vec3> light_attenuation_data;
    std::vector<vec2> light_cutoff_data;
    std::vector<vec3> light_spot_direction_data;
    std::vector<int>  light_type_data; /* This has to be int instead of bool in
					  order for .data() to work in load_bool_arr() */
};

#endif
