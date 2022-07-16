#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>

#include <GL/gl.h>
#include "loadobj.h"

class Skybox
{
public:
    Skybox();
    ~Skybox();

    virtual void render(GLuint shader_program) const;

private:
    Model model;

    GLuint texture_id;
    void load_cube_map(std::vector<std::string> texture_files);

    std::vector<std::string> grimmnight {
        "res/skybox/grimmnight_rt.tga",
        "res/skybox/grimmnight_lf.tga",
        "res/skybox/grimmnight_up.tga",
        "res/skybox/grimmnight_dn.tga",
        "res/skybox/grimmnight_bk.tga",
        "res/skybox/grimmnight_ft.tga"
    };
};

#endif