#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>

#include <GL/gl.h>
#include "VectorUtils3.h"

#include "Light.h"
#include "Object.h"
#include "Terrain.h"

class Shader
{
public:
    Shader();
    Shader(std::string const& vertex_file, std::string const& fragment_file);


    void start() const;
    void stop() const;

    GLuint get_program_id() const { return program_id; }

protected:
    int get_uniform_location(std::string const& uniform_name) const;

    void load_int  (int location, int         value) const;
    void load_float(int location, float       value) const;
    void load_bool (int location, bool        value) const;
    void load_mat4 (int location, mat4 const& value) const;
    void load_vec3 (int location, vec3 const& value) const;
    void load_vec4 (int location, vec4 const& value) const;
    
    void load_vec3_arr (int location, std::vector<vec3>    const& value) const;
    void load_vec2_arr (int location, std::vector<vec2>    const& value) const;
    void load_int_arr  (int location, std::vector<int>     const& value) const;
    void load_bool_arr (int location, std::vector<int>     const& value) const;
    void load_float_arr(int location, std::vector<GLfloat> const& value) const;

    void load_texture_unit(int location, int texture_unit) const;

    GLuint program_id;
};

class Object_Shader : public Shader
{
public:
    Object_Shader();
    Object_Shader(std::string const& vertex_file, std::string const& fragment_file);
    Object_Shader(std::string const& vertex_file, std::string const& fragment_file, int nr_of_textures); //if multiple textures
   
    void load_projection_matrix(mat4 const& projection_matrix) const;
    void load_world_matrix     (mat4 const&      world_matrix) const;
    void load_model_matrix     (mat4 const&      model_matrix) const;
    void load_camera_position  (vec3 const&        camera_pos) const;
    void load_light_color      (vec3 const&         light_pos) const;
  
    void load_lights(Light_Container const& light_container) const;
    void load_material_properties(Object const& obj) const;
    
    void load_number_of_lights(int num) const;
    void load_texture(int location, uint const& texture, int texture_unit) const;
    void load_multiple_textures(Terrain const& terrain) const; //only works for terrain object at this time


private:
    int location_projection_matrix;
    int location_world_matrix;
    int location_model_matrix;
    int location_lightning_params;
    int location_camera_pos;
    int location_light_pos_dir;
    int location_light_attenuation_params;
    int location_light_spot_direction;
    int location_light_cutoff;
    int location_light_type;
    int location_light_color;
    int location_num_of_lights;
    int location_specularity_map;
    int location_use_specularity_map;
    int location_ka;
    int location_kd;
    int location_ks;
    int location_a;
    int location_object_texture; //for main texture
    std::vector<int> location_multiple_object_textures; //for all textures except main texture (if multiple textures are to be used)
};

#endif

