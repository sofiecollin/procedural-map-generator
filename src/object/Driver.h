/*
    This is a class for driving vehicles. It consists of the following:
    - An object: a car
    - A state machine
    - Tools for driving
    The state machine represents the state of the car: driving, idle, done.
    When the car is done, it recieves instructions from the traffic class
    of where to go, then the car makes necessary calculations, goes into driving
    and drives.
*/

#ifndef DRIVERH
#define DRIVER_H

#include "Object.h"
#include "VectorUtils3.h"
#include "Road.h"
#include "Terrain.h"
#include "parameters.h"

class Driver{

public:
    Driver();
    ~Driver();

    // Overall functions
    void update(Terrain* terrain);
    void new_route(vec3 const& new_goal);
    void new_route(std::shared_ptr<Road_Node> node_ptr); 

    // Setters
    void set_goal(std::shared_ptr<Road_Node> node_ptr);
    void set_goal(vec3 const& new_goal);
    void set_state(int const& new_state);
    void set_offset(float const& new_offset);
    void set_speed(float const& new_speed);
    void set_position(float const& x, float const& y, float const& z);
    void set_position(vec3 const& new_pos);
    void set_start_node(std::shared_ptr <Road_Node*> new_start);
    void set_goal_node(std::shared_ptr <Road_Node*> new_goal);
    void rot_towards_goal();

    // Getters
    int get_state();
    std::shared_ptr <Road_Node*> get_goal_node();
    std::shared_ptr <Road_Node*> get_start_node();
    Object get_car() const;
    Object get_car();


private:
    bool goal_reached();                        // Has goal been reached?
    void calc_route();                          // Calculates a new route (form one node to adjacent node)
    vec3 get_position();                        
    mat4 get_rotation();
    std::shared_ptr <Road_Node*> get_start();

    void drive(Terrain* terrain);

    float offset{0};    // Offset on road.
    float speed{0};     // Speed is currently hard coded in the .cc file
    int state{0};       // 0 - done, 1 - driving, 2 - idle

    
    vec3 goal;
    vec3 drive_dir;     // Always normalized!
    std::shared_ptr <Road_Node*> start_node;
    std::shared_ptr <Road_Node*> goal_node;
    
public:
    Object car = Object("low_poly_car");

};

#endif