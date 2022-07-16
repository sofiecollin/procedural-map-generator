#include "Driveable.h"
#include "MicroGlut.h"

Driveable::Driveable()
{
    model_data.material.ks = (vec3){0.0, 0.0, 0.0};
    model_data.material.ka = (vec3){1.5, 1.5, 1.5};
    model_data.material.kd = (vec3){1.0, 1.0, 1.0};
}

Driveable::~Driveable()
{

}

void Driveable::drive(Terrain *terrain)
{
    if (glutKeyIsDown('w'))
    {   
        move(MultVec3(rotation, vec3{0.0, 0.0, -0.25}));
        set_height(terrain->get_interpol_height(position.x, position.z, 0.0));
    }
    if (glutKeyIsDown('s'))
    {   
        move(MultVec3(rotation, vec3{0.0, 0.0, 0.25}));
        set_height(terrain->get_interpol_height(position.x, position.z, 0.0));
    }
    if (glutKeyIsDown('d'))
    {   
        rotate(Ry(-0.05));
    }
    if (glutKeyIsDown('a'))
    {   
        rotate(Ry(0.05));
    }
    return;
}

void Driveable::set_height(float const& new_height)
{
    position.y = new_height;
}