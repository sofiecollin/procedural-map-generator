#include "Skybox_Shader.h"
#include "Camera.h"

Skybox_Shader::Skybox_Shader()
    : Shader("res/shader_files/skybox.vert", "res/shader_files/skybox.frag")
{
    location_projection_matrix = get_uniform_location("projection_matrix");
    location_world_matrix = get_uniform_location("world_matrix");

}

void Skybox_Shader::load_projection_matrix(mat4 const& projection_matrix) const
{
    load_mat4(location_projection_matrix, projection_matrix);
}

void Skybox_Shader::load_world_matrix(Camera const& camera) const
{
    mat4 world_matrix = camera.get_world_matrix();

    world_matrix.m[3] = 0;
    world_matrix.m[7] = 0;
    world_matrix.m[11] = 0;

    load_mat4(location_world_matrix, world_matrix);
}