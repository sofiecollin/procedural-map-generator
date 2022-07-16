#include <iostream>

#include <GL/gl.h>
#include "stb_image.h"

#include "Skybox.h"

Skybox::Skybox()
{
    model = *LoadModelPlus("res/objects/skybox.obj");

    load_cube_map(grimmnight);
}

Skybox::~Skybox() 
{

}

void Skybox::render(GLuint shader_program) const
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    DrawModel(model, shader_program, "in_position", NULL, NULL);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void Skybox::load_cube_map(std::vector<std::string> texture_files)
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    for (unsigned i = 0; i < texture_files.size(); i++)
    {
        int width, height, nrChannels;
	    stbi_set_flip_vertically_on_load(false); // tell stb to flip loaded texture's on the y-axis.
	    unsigned char *data = stbi_load(texture_files[i].c_str(), &width, &height, &nrChannels, STBI_rgb);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else
        {
            std::cout << "Failed to load texture:" << texture_files[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}