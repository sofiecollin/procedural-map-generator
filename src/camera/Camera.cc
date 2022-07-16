#include "Camera.h"

void Camera::update(float delta_time, Object const& obj, Terrain & terrain)
{
    // Initiatlization

    if (!free_mode)
    {
        follow_car(obj, terrain);
    }
    else
    {
        translate();
    }
}

void Camera::follow_car(Object const& obj, Terrain & terrain)
{
    vec3 pos = obj.get_position();
    mat4 rot = obj.get_rotation();
    look_at = pos + rot * vec3{0.0, 0.0, -10.0};
    position = (pos + vec3{0.0, 5.0, 0.0}) - rot * vec3{0.0, 0.0, -6.0};
    float terrain_height = terrain.get_interpol_height(position.x, position.z, 0.0);
    if (position.y < terrain_height + 5.0)
    {
        position.y = terrain_height + 5.0;
    }
}

void Camera::check_input()
{

    if (mouse_rotation)
    {
        glutHideCursor();

        int delta_x = last_x - glutGet(GLUT_MOUSE_POSITION_X);
        int delta_y = last_y - glutGet(GLUT_MOUSE_POSITION_Y);

        vec3 direction {look_at - position};
        direction = R(delta_x / 10, up_vector) * direction;
        vec3 bi_dir = cross(direction, up_vector);
        direction = R(delta_y / 10, bi_dir) * direction;

        look_at = position + direction;

        glutWarpPointer(last_x, last_y);

    }
    else 
    {
        glutShowCursor();

        vec3 direction {look_at - position};
        vec3 bi_dir {cross(direction, up_vector)};
        if (glutKeyIsDown(GLUT_KEY_LEFT))
        {
            direction = R(5, up_vector) * direction;
        }
        if (glutKeyIsDown(GLUT_KEY_RIGHT))
        {
            direction = R(-5, up_vector) * direction;
        }
        if (glutKeyIsDown(GLUT_KEY_DOWN))
        {
            direction = R(-5, bi_dir) * direction;
        }
        if (glutKeyIsDown(GLUT_KEY_UP))
        {
            direction = R(5, bi_dir) * direction;
        }
        look_at = position + direction;
    }
    
    //Translation
    velosity = vec3 {0, 0, 0};
    vec3 fly_dir {normalize(look_at - position)};
    fly_dir.y = 0;
    vec3 bi_dir {cross(up_vector, fly_dir)};

    if (glutKeyIsDown('w'))
    {
        velosity += fly_dir;
    }
    if (glutKeyIsDown('a'))
    {
        velosity += bi_dir;
    }
    if (glutKeyIsDown('s'))
    {
        velosity -= fly_dir;
    }
    if (glutKeyIsDown('d'))
    {
        velosity -= bi_dir;
    }
    if (glutKeyIsDown('c'))
    {
        velosity -= up_vector;
    }
    if (glutKeyIsDown('v'))
    {
        velosity += up_vector;
    }
    if (norm(velosity) != 0)
    {
        velosity = normalize(velosity);
    }
    
    if (glutKeyIsDown('+') && !key_down)
    {
        inc_translationspeed();
        key_down = true;
    }
    if (glutKeyIsDown('-') && !key_down)
    {
        deq_translationspeed();
        key_down = true;
    }
    if (glutKeyIsDown('m') && !key_down)
    {
        mouse_rotation = !mouse_rotation;
        free_mode = true;
        key_down = true;
    }
    if (glutKeyIsDown('f') && !key_down)
    {
        free_mode = !free_mode;
        key_down = true;
    }
    if (!glutKeyIsDown('-')  && !glutKeyIsDown('-') && !glutKeyIsDown('m') && !glutKeyIsDown('f'))
    {
        key_down = false;
    }
}

mat4 Camera::get_world_matrix() const
{
    return lookAtv(position, look_at, up_vector);
}

void Camera::translate() 
{
    position += velosity * translation_speed;
    look_at += velosity * translation_speed;
}

