/*
    This class creates a driveable car. 
    It inherits from object, allows for moving the model
    with wasd and follows the terrain heightwise
*/

#ifndef DRIVEABLE_H
#define DRIVEABLE_H

#include "Object.h"
#include "Terrain.h"

class Driveable: public Object
{
    public:
        Driveable();
        ~Driveable();

    public:
        void drive(Terrain *terrain);
        void set_height(float const& new_height);
};

#endif