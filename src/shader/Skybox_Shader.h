#ifndef SKYBOX_SHADER_H
#define SKYBOX_SHADER_H

#include "Shader.h"
#include "Camera.h"

class Skybox_Shader : public Shader
{
public:
    Skybox_Shader();

    void load_projection_matrix(mat4 const& projection_matrix) const;
    void load_world_matrix(Camera const& camera) const;

private:
    int location_projection_matrix;
    int location_world_matrix;



};

#endif