/*
    This class is responsible for keeping track of everything related to traffic,
    such as roads, traffic lights, cars.
    It is also responsible for updating classes that are primarily traffic-related.
    This includes all cars and traffic lights. Furthermore, it also takes responsibility
    for spawning, removoving and overall make the traffic run smooth. 
    Randomness for cars is implemented via the boost library. 
*/

#ifndef TRAFFIC_H
#define TRAFFIC_H

#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>
#include <boost/random.hpp>

#include "Road.h"
#include "Driver.h"
#include "Road.h"
#include "Shader.h"
#include "Terrain.h"

class Traffic
{
public:
    // Constructors/Destructors
    Traffic();
    Traffic(std::shared_ptr <Road_Network*> new_network);
    Traffic(std::shared_ptr <Road_Network*> new_network, int const& nr_cars);
    ~Traffic();

    // Others
    void create_car();
    void render_cars(Object_Shader const& shader) const;
    void spawn_cars(int const& nr_cars);
    void update();
    void assign_new_route(unsigned const& i);
    void assign_first_route();
    void set_all_states(int const& new_state);
    void set_terrain(Terrain* new_terrain);
    std::shared_ptr <Road_Node*> get_new_destination(std::shared_ptr <Road_Node*> node);

    Driver brum = Driver();
    std::vector <Driver> cars;

    

private:
    
    std::shared_ptr <Road_Network*> network;
    Terrain* terrain;
    int get_rand();
};

#endif