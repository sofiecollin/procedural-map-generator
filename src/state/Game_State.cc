#include <GL/gl.h>
#include "GL_utilities.h"
#include "VectorUtils3.h"

#include "Game_State.h"

#include <iostream>
#include <memory>

Game_State::Game_State()
{
    // GL inits
    glClearColor(0.2,0.2,0.5,0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    projection_matrix = perspective(90, 1.0, 0.1, 1000);

    // Set terrain material properites
    terrain.set_material(vec3{0.8, 0.8, 0.8}, vec3{0.7, 0.7, 0.7}, vec3{0.2, 0.2, 0.2}, 1.0, TERRAIN_TEXTURE_PATHS, 0, false);
    
    // Add lightsource to scene
    std::shared_ptr<Dir_Light> Light(new Dir_Light(vec3{0.8, 0.8, 0.8}, vec3{-0.5, -0.5, -0.5}));
    light_container.add_dir_light(Light);

    // Innit traffic with n cars
    traffic = Traffic(std::make_shared <Road_Network*> (&road_network), 50);
    traffic.set_terrain(&terrain);
    
    // Add water (blue square drawn under terrain)
    float water_scale = 64*terrain.get_width_param();
    Object water{"square", vec3{water_scale, -terrain.get_max_height()/3, water_scale}};
    water.set_scale(vec3{water_scale, 1, water_scale});
    objects.push_back(water);


    // Add trees to world
    for (int i = 0; i < 100 * TERRAIN_SCALE_PARAMETER; i++)
    {
        int x {rand() % (int) (TERRAIN_SCALE_PARAMETER * (TERRAIN_RESOLUTION - 1))};
        int z {rand() % (int) (TERRAIN_SCALE_PARAMETER * (TERRAIN_RESOLUTION - 1))};
        if(terrain.get_watermap_value(x, z) != 1)
        {
            objects.push_back(Object{"pine", vec3{(float) x, terrain.get_height(x, z), (float) z}});
        }
    }

    road_network.generate_mesh(terrain);
}

Game_State::~Game_State()
{
}

void Game_State::update(float delta_time)
{
    // Update world objects
    car.drive(&terrain);

    // Update camera
    camera.update(delta_time, car, terrain);
    traffic.update();
}

void Game_State::render() const
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    //  SKYBOX
    skybox_shader.start();
    skybox_shader.load_projection_matrix(projection_matrix);
    skybox_shader.load_world_matrix(camera);
    skybox.render(skybox_shader.get_program_id());
    skybox_shader.stop();

    //  TERRAIN
    
    terrain_shader.start();
    terrain_shader.load_projection_matrix(projection_matrix);
    terrain_shader.load_world_matrix(camera.get_world_matrix());
    terrain_shader.load_camera_position(camera.get_position());
    terrain_shader.load_lights(light_container);

    terrain_shader.load_material_properties(terrain); 
    terrain_shader.load_multiple_textures(terrain);
    terrain_shader.load_model_matrix(terrain.get_model_matrix());
       
    terrain.render(terrain_shader.get_program_id());
    terrain_shader.stop();
    
    //  OBJECTS
    shader.start();
    shader.load_projection_matrix(projection_matrix);
    shader.load_world_matrix(camera.get_world_matrix());
    shader.load_camera_position(camera.get_position());
    shader.load_lights(light_container);

    shader.load_material_properties(car);
    shader.load_model_matrix(car.get_model_matrix());
    car.render(shader.get_program_id());

    // Render traffic cars
    traffic.render_cars(shader);

    for (auto it = objects.begin(); it != objects.end(); it++)
    {
        shader.load_model_matrix(it->get_model_matrix());
        shader.load_material_properties(*it);
        it->render(shader.get_program_id());
    }

    road_network.render(shader);
    
    shader.stop();

    glutSwapBuffers();
}

void Game_State::check_input()
{
    if (glutKeyIsDown(GLUT_KEY_ESC))
    {
        change_state = "exit";
    }
    if (glutKeyIsDown('h'))
    {
      std::cout << camera.get_position() << std::endl;
    }
    if (glutKeyIsDown('p'))
    {
        traffic.set_all_states(1);
    }
    camera.check_input();
}
