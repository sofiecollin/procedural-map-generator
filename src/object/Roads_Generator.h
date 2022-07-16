#ifndef ROADS_GENERATOR_H
#define ROADS_GENERATOR_H

#include "Terrain.h"
#include "Road.h"
#include "parameters.h"
#include "Pathfinder.h"

#include <vector>
#include <queue>
#include <random>
#include <functional>


class Segment;

struct Compare_Prio_Values
{ 
        bool operator()(Segment const& lhs, Segment const& rhs); 
};

class Roads_Generator
{
public:
    Roads_Generator(Road_Network& network, Terrain& terrain);

private:
    Road_Network& network;
    Terrain& terrain;

    // Parameters

    // Algorithm functions
    void initialize_start_position();
    void generate_cities();
    void generate();
    void generate_highways();
    void generate_road(vec3 from, vec3 to, std::string roadtype);
    bool check_local_constraints(Segment& segment);
    bool check_angle(Road_Node* existing_node, vec3 const& pos) const;
    bool check_and_rotate(Segment& segment, std::function<bool(vec3, vec3)> condition_func);
    bool check_water(Segment& segment);
    bool check_incline(Segment& segment);
    
    bool general_prune(Segment& segment, float max_prune_length, int num_steps, std::function<bool(vec3, vec3)> condition_func);
    bool general_prune(vec3 existing_node_pos, vec3 & new_coord, float max_prune_length, int num_steps, std::function<bool(vec3, vec3)> condition_func);
    
    void create_intersection_node(Segment& segment);
    void add_new_segments_to_queue(Segment const& previous_segment, Road_Node* current_node); 

    // Helper functions
    bool connect_to_road_network(Segment & current_segment);
    bool check_distance_to_node(Segment & segment) const;

    std::priority_queue<Segment, std::vector<Segment>, Compare_Prio_Values> prio_queue;

    // Generate start position (should be in a population density class later)
    const int& kernel_size = 50;
    float highway_thickness = 15;
    float main_road_thickness = 3;
    std::vector<vec3> city_coordinates {};
};

class Segment 
{
public:
    Segment(vec3 const& new_coordinate, Road_Node* existing_node); 

    Road_Node* get_existing_node() const        { return existing_node; }
    void set_existing_node(Road_Node* new_node) { existing_node = new_node; }
    vec3 get_new_coordinate() const             { return new_node->get_position(); }
    void set_new_coordinate(vec3 const& coordinate);
    Road_Node* get_new_node()                   { return new_node; }
    void set_new_node(Road_Node* node)          { new_node = node; }
    int get_speed() const                       { return speed; }
    float get_prio_value() const                { return prio_value; }
    void set_prio_value(float value)            { prio_value = value; }
    
private:

    float prio_value = 0;
    
    int speed = 40; // Speed used to calculate travel_time
    float travel_time;
    
    Road_Node* new_node;      // A new node that is not yet added to the road network
    Road_Node* existing_node; // The node that is already in the road network
};

#endif
