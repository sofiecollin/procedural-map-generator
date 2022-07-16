#ifndef CAMERA_H
#define CAMERA_H

#include "VectorUtils3.h"
#include "MicroGlut.h"
#include <string>
#include <vector>
#include <algorithm>

#include "Object.h"
#include "Terrain.h"
#include "Roads_Generator.h"

class Camera
{
public:
    Camera() {};
    void update(float delta_time, Object const& obj, Terrain & terrain);
    void check_input();
    mat4 get_world_matrix() const;
    vec3 get_position() const { return position; }
    vec3 get_look_dir() const { return look_at - position; }
    void follow_car(Object const& obj, Terrain & terrain);
    
private:
    vec3 position = {0, 5, 10};
    vec3 look_at {16, 0, 16};
    vec3 up_vector {0, 1, 0};

    vec3 velosity {0, 0, 0};

    float translation_speed = 0.5;

    int last_x {256};
    int last_y {256};

    bool key_down = false;
    bool mouse_rotation = false;
    bool free_mode = true;
    
    mat4 worldMatrix = get_world_matrix();
    void translate();
    void rotate();
    void inc_translationspeed() {translation_speed *= 1.1;};
    void deq_translationspeed() {translation_speed *= 0.8;};
};

#endif
