#include "Shader.h"

#include "GL/gl.h"
#include "GL_utilities.h"
#include "LoadTGA.h"

#include "Light.h"

Shader::Shader()
{
    program_id = loadShaders("res/shader_files/phong_mult.vert", "res/shader_files/phong_mult.frag");
}

Shader::Shader(std::string const& vertex_file, std::string const& fragment_file)
{
    program_id = loadShaders(vertex_file.c_str(), fragment_file.c_str());
}

void Shader::start() const
{
    glUseProgram(program_id);
}

void Shader::stop() const
{
  glUseProgram(0);
}

int Shader::get_uniform_location(std::string const& uniform_name) const
{
    return glGetUniformLocation(program_id, uniform_name.c_str());
}

void Shader::load_int(int location, int value) const
{
    glUniform1i(location, value);
}

void Shader::load_float(int location, float value) const
{
    glUniform1f(location, value);
}

void Shader::load_bool(int location, bool value) const 
{
    glUniform1i (location, value ? 1 : 0);
}

void Shader::load_mat4(int location, mat4 const& value) const
{
    glUniformMatrix4fv(location, 1, GL_TRUE, value.m);
}

void Shader::load_vec3(int location, vec3 const& value) const
{
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::load_vec4(int location, vec4 const& value) const
{
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::load_texture_unit(int location, int texture_unit) const
{
    glUniform1i(location, texture_unit);
}

void Shader::load_int_arr(int location, std::vector<int> const& value) const
{
    glUniform1iv(location, value.size(), reinterpret_cast<const GLint *>(value.data()));
}

void Shader::load_bool_arr(int location, std::vector<int> const& value) const
{
    glUniform1fv(location, value.size(), reinterpret_cast<const GLfloat *>(value.data()));
}

void Shader::load_float_arr(int location, std::vector<GLfloat> const& value) const
{
    glUniform1fv(location, value.size(), value.data());
}

void Shader::load_vec2_arr(int location, std::vector<vec2> const& value) const
{
    glUniform2fv(location, value.size(), reinterpret_cast<const GLfloat *>(value.data()));
}

void Shader::load_vec3_arr(int location, std::vector<vec3> const& value) const
{
    glUniform3fv(location, value.size(), reinterpret_cast<const GLfloat *>(value.data()));
}

//=================================
//======== Object_Shader ==========
//=================================

Object_Shader::Object_Shader()
    : Shader()
{
    location_projection_matrix = get_uniform_location("projection_matrix");
    location_world_matrix = get_uniform_location("world_matrix");
    location_model_matrix = get_uniform_location("model_matrix");
    location_lightning_params = get_uniform_location("lightning_params");
    location_camera_pos = get_uniform_location("camera_pos");
    location_light_pos_dir = get_uniform_location("light_pos_dir");
    location_light_color = get_uniform_location("light_color");
    location_light_attenuation_params = get_uniform_location("light_attenuation_params");
    location_light_cutoff = get_uniform_location("light_cutoff");
    location_light_spot_direction = get_uniform_location("light_spot_dir");
    location_light_type = get_uniform_location("light_type");
    location_num_of_lights = get_uniform_location("num_of_lights");
    location_object_texture = get_uniform_location("object_texture");
    location_specularity_map = get_uniform_location("specularity_map");
    location_use_specularity_map = get_uniform_location("use_specularity_map");
    location_ka = get_uniform_location("ka");
    location_kd = get_uniform_location("kd");
    location_ks = get_uniform_location("ks");
    location_a = get_uniform_location("a");

}

Object_Shader::Object_Shader(std::string const& vertex_file, std::string const& fragment_file)
    : Shader(vertex_file,fragment_file)
{
    location_projection_matrix = get_uniform_location("projection_matrix");
    location_world_matrix = get_uniform_location("world_matrix");
    location_model_matrix = get_uniform_location("model_matrix");
    location_lightning_params = get_uniform_location("lightning_params");
    location_camera_pos = get_uniform_location("camera_pos");
    location_light_pos_dir = get_uniform_location("light_pos_dir");
    location_light_color = get_uniform_location("light_color");
    location_light_attenuation_params = get_uniform_location("light_attenuation_params");
    location_light_cutoff = get_uniform_location("light_cutoff");
    location_light_spot_direction = get_uniform_location("light_spot_dir");
    location_light_type = get_uniform_location("light_type");
    location_num_of_lights = get_uniform_location("num_of_lights");
    location_object_texture = get_uniform_location("object_texture");
    location_specularity_map = get_uniform_location("specularity_map");
    location_use_specularity_map = get_uniform_location("use_specularity_map");
    location_ka = get_uniform_location("ka");
    location_kd = get_uniform_location("kd");
    location_ks = get_uniform_location("ks");
    location_a = get_uniform_location("a");

}

Object_Shader::Object_Shader(std::string const& vertex_file, std::string const& fragment_file, int nr_of_textures)
    : Shader(vertex_file,fragment_file)
{
    location_projection_matrix = get_uniform_location("projection_matrix");
    location_world_matrix = get_uniform_location("world_matrix");
    location_model_matrix = get_uniform_location("model_matrix");
    location_lightning_params = get_uniform_location("lightning_params");
    location_camera_pos = get_uniform_location("camera_pos");
    location_light_pos_dir = get_uniform_location("light_pos_dir");
    location_light_color = get_uniform_location("light_color");
    location_light_attenuation_params = get_uniform_location("light_attenuation_params");
    location_light_cutoff = get_uniform_location("light_cutoff");
    location_light_spot_direction = get_uniform_location("light_spot_dir");
    location_light_type = get_uniform_location("light_type");
    location_num_of_lights = get_uniform_location("num_of_lights");
    location_specularity_map = get_uniform_location("specularity_map");
    location_use_specularity_map = get_uniform_location("use_specularity_map");
    location_ka = get_uniform_location("ka");
    location_kd = get_uniform_location("kd");
    location_ks = get_uniform_location("ks");
    location_a = get_uniform_location("a");
    location_object_texture = get_uniform_location("object_texture"); //for main textures

    for(int i=0; i<nr_of_textures-1; i++)
    {  
        //for all textures except main texture 
        int location = get_uniform_location("object_texture" + std::to_string(i+1));
        location_multiple_object_textures.push_back(location);  
    }
        

}

void Object_Shader::load_projection_matrix(mat4 const& projection_matrix) const
{
    load_mat4(location_projection_matrix, projection_matrix);
}

void Object_Shader::load_world_matrix(mat4 const& world_matrix) const
{
    load_mat4(location_world_matrix, world_matrix);
}

void Object_Shader::load_model_matrix(mat4 const& model_matrix) const
{
    load_mat4(location_model_matrix, model_matrix);
}

void Object_Shader::load_camera_position(vec3 const& camera_pos) const
{
    load_vec3(location_camera_pos, camera_pos);
}

void Object_Shader::load_lights(Light_Container const& light_container) const
{
    load_vec3_arr(location_light_pos_dir, light_container.get_light_pos_dir_data());
    load_vec3_arr(location_light_color, light_container.get_light_color_data());
    load_vec3_arr(location_light_attenuation_params, light_container.get_light_attenuation_data());
    load_vec3_arr(location_light_spot_direction, light_container.get_light_spot_direction_data());
    load_vec2_arr(location_light_cutoff, light_container.get_light_cutoff_data());
    load_int_arr(location_light_type, light_container.get_light_type_data());
    load_int(location_num_of_lights, light_container.get_number_of_active_lights());
}

void Object_Shader::load_material_properties(Object const& obj) const
{
    auto mat = obj.get_material();
    load_vec3(location_ka, mat.ka);
    load_vec3(location_kd, mat.kd);
    load_vec3(location_ks, mat.ks);
    load_float(location_a, mat.a);
    load_texture(location_object_texture, mat.texture_id, 0);
    if(mat.use_specularity_map)
    {
        load_texture(location_specularity_map, mat.specularity_map_id, 1);
    }
    load_bool(location_use_specularity_map, mat.use_specularity_map);
}

void Object_Shader::load_texture(int location, uint const& texture, int texture_unit) const
{
    glActiveTexture(GL_TEXTURE0 + texture_unit);    
    glBindTexture(GL_TEXTURE_2D, texture);
    load_texture_unit(location, texture_unit);
}

void Object_Shader::load_multiple_textures(Terrain const& terrain) const
{
    std::vector<GLuint> texture_ids = terrain.get_texture_ids();
    if(texture_ids.size() > 1)
    {
        for(uint i = 0; i < texture_ids.size() - 1; i++)
        {
            load_texture(location_multiple_object_textures[i], texture_ids[i+1], i+1);
        }
    }
    
}
