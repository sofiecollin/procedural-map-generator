#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>
#include <vector>
#include <memory>

#include <GL/gl.h>
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

#include "State.h"
#include "Object.h"
#include "Terrain.h"
#include "Shader.h"
#include "Camera.h"
#include "Skybox.h"
#include "Skybox_Shader.h"
#include "Light.h"
#include "Driveable.h"
#include "Pathfinder.h"
#include "Road.h"
#include "Roads_Generator.h"
#include "Traffic.h"
#include "parameters.h"

class Game_State : public State
{
public:
    Game_State();
    ~Game_State();

    Game_State& operator=(Game_State const& rhs) = default;
    Game_State& operator=(Game_State && rhs) = default;
    
    virtual void update(float delta_time);
    virtual void render() const;
    virtual void check_input();
    virtual std::string name() const
    {
	return "game";
    }

private:

    //constants
    const int initWidth=512, initHeight=512;

    // Projection matrix, set by a call to perspective().
    mat4 projection_matrix;

    Driveable car {};
    Camera camera {};
    Skybox skybox {};
    Object_Shader shader {};
    Skybox_Shader skybox_shader {};
    Object_Shader terrain_shader = Object_Shader("res/shader_files/terrain.vert", "res/shader_files/terrain.frag", TERRAIN_NUM_TEXTURES); 
    Light_Container light_container {};
    
    //Terrain terrain = Terrain(TERRAIN_MAX_HEIGHT, TERRAIN_SCALE_PARAMETER, "res/textures/heightmap.tga"); //terrain from heightmap
    //Terrain terrain = Terrain(TERRAIN_RESOLUTION, TERRAIN_MAX_HEIGHT, TERRAIN_SCALE_PARAMETER,
    //                           TERRAIN_NUM_OCTAVES, TERRAIN_SMOOTHNESS, SEED); //terrain from parameters
    
    // Terrain below can be used to test water based things. It also loads a watermap
    //Terrain terrain = Terrain(TERRAIN_MAX_HEIGHT, TERRAIN_SCALE_PARAMETER,"res/textures/heightmap.tga", "res/textures/watermap.tga");
    Terrain terrain = Terrain(TERRAIN_RESOLUTION, TERRAIN_MAX_HEIGHT, TERRAIN_SCALE_PARAMETER,
                                  TERRAIN_NUM_OCTAVES, TERRAIN_SMOOTHNESS, SEED, "res/textures/watermap.tga");
    

    Road_Network road_network = Road_Network();
    Roads_Generator roads_generator = Roads_Generator(road_network, terrain);
    Traffic traffic = Traffic();

    std::vector<Object> objects {};
};

#endif