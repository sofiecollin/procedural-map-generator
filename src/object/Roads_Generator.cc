#include "Roads_Generator.h"

#include <iostream>
#include <chrono>

// Define to print debug messages 
//#define DEBUG_TEXT
#define TIMED_GENERATION
//#define LOCAL_CONSTRAINTS_DEBUG_TEXT

#define pi 3.1415

bool Compare_Prio_Values::operator()(Segment const& lhs, Segment const& rhs)
{
    return lhs.get_prio_value() > rhs.get_prio_value();
}

Roads_Generator::Roads_Generator(Road_Network& network, Terrain& terrain)
    : network{network}, terrain{terrain}
{   
 
    highway_thickness = HIGHWAY_WIDTH;
    if (NUMBER_OF_CITIES == 1)
    {
        initialize_start_position();
        vec3 city_pos = city_coordinates.front();
        network.add_node(city_pos);
    }
    else
    {
        generate_highways();
    }
    
    generate_cities();

    // Generation done without y-height. Setting node heights according to terrain
    std::vector<Road_Node*> nodeList = network.get_nodes();
    for(auto it = nodeList.begin(); it != nodeList.end(); it++)
    {
        vec3 pos = (*it)->get_position();
        (*it)->set_position(vec3{pos.x, terrain.get_interpol_height(pos.x, pos.z, 0.0), pos.z});
    }
}

/*
    Generates road from two coordinates with A* algoritm, STEEPNESS_PENALTY from parameters will
    affect how the road is constructed   
*/

void Roads_Generator::generate_road(vec3 first_city, vec3 next_populated_area_vector, std::string roadtype)
{
    Pathfinder finder(&terrain, first_city.x, first_city.z, next_populated_area_vector.x, next_populated_area_vector.z);
    std::vector<vec2> list = finder.find_path();
    std::vector<vec3> new_list {};
    std::transform(list.begin(), list.end(), std::back_inserter(new_list), [&terrain = terrain](vec2 v) -> vec3 {
        return vec3{v.x, terrain.get_interpol_height(v.x, v.y, 0), v.y};
        });
    if (roadtype == "H")
    {
        network.add_nodes(new_list, highway_thickness);
    }
    else
    {
        network.add_nodes(new_list, main_road_thickness);
    }
    
}

/*
    Generates the number of cites specified in parameters.
*/

void Roads_Generator::generate_cities()
{
    for (auto it = city_coordinates.begin(); it != city_coordinates.end(); ++it)
    {
        prio_queue = std::priority_queue<Segment, std::vector<Segment>, Compare_Prio_Values> {};
        float min_ang;
        float rand_ang;
        float x;
        float z;
        vec3 pos;
        min_ang = LC_MIN_ANGLE*pi/180;
        rand_ang = min_ang + (rand() % 100)*(pi - min_ang)/100;

        x = (*it).x + DEFAULT_SEGMENT_LENGTH*cos(rand_ang);
        z = (*it).z + DEFAULT_SEGMENT_LENGTH*sin(rand_ang);
        prio_queue.push(Segment{vec3{x, 0, z}, network.get_node(*it)});

        x = (*it).x + DEFAULT_SEGMENT_LENGTH*cos(rand_ang);
        z = (*it).z - DEFAULT_SEGMENT_LENGTH*sin(rand_ang);
        prio_queue.push(Segment{vec3{x, 0, z}, network.get_node(*it)});

        x = (*it).x - DEFAULT_SEGMENT_LENGTH*cos(rand_ang);
        z = (*it).z + DEFAULT_SEGMENT_LENGTH*sin(rand_ang);
        prio_queue.push(Segment{vec3{x, 0, z}, network.get_node(*it)});

        x = (*it).x - DEFAULT_SEGMENT_LENGTH*cos(rand_ang);
        z = (*it).z - DEFAULT_SEGMENT_LENGTH*sin(rand_ang);
        prio_queue.push(Segment{vec3{x, 0, z}, network.get_node(*it)});

        generate();
    }
}

/*
    If only one city is created this function creates a random position
*/

void Roads_Generator::initialize_start_position()
{
    float xpos_to;
    float zpos_to;
    int min_pos = 5;
    int max = TERRAIN_RESOLUTION - min_pos;

    do
    {
    xpos_to = TERRAIN_SCALE_PARAMETER*(min_pos + (rand() % max));
    zpos_to = TERRAIN_SCALE_PARAMETER*(min_pos + (rand() % max));

    } while(terrain.get_watermap_value((int) xpos_to, (int) zpos_to) == 1);
    city_coordinates.push_back(vec3{xpos_to, 0, zpos_to});
}

/*
    Generates NUMBER_OF_CITIES - 1 highways between the cities that are closest to each other
*/

void Roads_Generator::generate_highways()
{
    float xpos_from;
    float zpos_from;
    float xpos_to;
    float zpos_to;
    float from_height;
    float to_height;
    vec3 from;
    vec3 to;
    int min_pos = 5;
    int max = TERRAIN_RESOLUTION - 2*min_pos;
    int displacement = 10;
    bool invalid_position = true;
    bool too_close = false;

    for (int i = 0; i < NUMBER_OF_CITIES - 1; ++i)
    {
        while (invalid_position)
        {
            xpos_to = TERRAIN_SCALE_PARAMETER*(min_pos + (rand() % max));
            zpos_to = TERRAIN_SCALE_PARAMETER*(min_pos + (rand() % max));
            to_height = terrain.get_height(xpos_to, zpos_to);
            to = vec3{xpos_to, to_height, zpos_to};


            for (auto it = city_coordinates.begin(); it != city_coordinates.end(); ++it)
            {
                if (norm((*it) - to) < displacement)
                {
                    too_close = true;
                }
            }

            if (!(terrain.get_watermap_value(to.x, to.z) == 1) && !too_close)
            {
                invalid_position = false;
            }
        }
        invalid_position = true;
        too_close = true;
        bool water = true;

        if (city_coordinates.empty())
        {
            while (water)
            {
                xpos_from = TERRAIN_SCALE_PARAMETER*(min_pos + (rand() % max));
                zpos_from = TERRAIN_SCALE_PARAMETER*(min_pos + (rand() % max));
                from_height = terrain.get_height(xpos_from, zpos_from);
                from = vec3{xpos_from, from_height, zpos_from};

                if (!(terrain.get_watermap_value(from.x, from.z) == 1))
                {
                    water = false;
                }
            }
            water = true;
            
            city_coordinates.push_back(from);

        }
        else
        {
            from = city_coordinates[0];
            for (auto it = city_coordinates.begin(); it != city_coordinates.end(); ++it)
            {
                if (norm(*it -  to) < norm(from - to))
                {
                    from = *it;
                }
            }
        }
        too_close = false;
        city_coordinates.push_back(to);
        generate_road(from, to, "H");
    }
    
}

void Roads_Generator::generate()
{
    #ifdef DEBUG_TEXT
        auto start = std::chrono::system_clock::now();
    #endif

    int max_num_segments = SEGMENTS_PER_CITY;
    int num_segments = 1;

    // If prio queue empty, stop. Otherwise generate next
    while(!prio_queue.empty() && num_segments < max_num_segments)
    {
        #ifdef DEBUG_TEXT
            std::cout << "Next segment: " << num_segments << std::endl;
        #endif
        
        Segment curr_segment = prio_queue.top();
        prio_queue.pop();

        if(check_local_constraints(curr_segment))
        {
            auto new_node = network.try_add_node(curr_segment.get_new_node(), {curr_segment.get_existing_node()});  

            add_new_segments_to_queue(curr_segment, new_node);
        }

        num_segments++;
    }
    #ifdef DEBUG_TEXT
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        std::cout << "Road network generated in " << elapsed_seconds.count() << " seconds." << std::endl;
    #endif
}

// Checks the local constraints:
//  - avoid water
//  - avoid steep inclines
//  - avoid small angles between roads
//  - Tries to connect the segment to an existing node/tile
//
// Returns true if the segment should be added to the network as normal. 
//
bool Roads_Generator::check_local_constraints(Segment& segment)
{
    // Changes the geometry of the segment (rotates it and prunes) to avoid water / steep inclines
    bool water_ok = check_water(segment);
    bool incline_ok = check_incline(segment);

    // Disallows segments to close to others based on angle
    if (!check_angle(segment.get_existing_node(), segment.get_new_coordinate()))
    {
        return false;
    }

    // Connects to a part of the existing road network.
    connect_to_road_network(segment);

    // Create intersection if segment crosses another segment
    create_intersection_node(segment);

    if (!check_angle(segment.get_new_node(), segment.get_existing_node()->get_position())
        || !check_angle(segment.get_existing_node(), segment.get_new_node()->get_position())
        || !check_distance_to_node(segment))
    {
        return false;
    }

    return water_ok && incline_ok;
}

// If angle between existing road segments and given position is below threshold, return false
bool Roads_Generator::check_angle(Road_Node* existing_node, vec3 const& pos) const
{
    float smallest_angle {existing_node->get_smallest_angle(pos)};
    if(smallest_angle < LC_MIN_ANGLE)
    {
        #ifdef LOCAL_CONSTRAINTS_DEBUG_TEXT
	        std::cout << "Disallowed connection between " << pos << " and "
		          << existing_node->get_position() << " due to low angle "
		          << smallest_angle << std::endl;
        #endif

	    return false;
    }
    return true;
}

// Checks if the given segment fullfills the given condition. If not it tries to rotate and prune the segment
// until it fullfills the condition
bool Roads_Generator::check_and_rotate(Segment& segment, std::function<bool(vec3, vec3)> condition_func)
{
    vec3 new_coord = segment.get_new_coordinate();
    vec3 rot_center = segment.get_existing_node()->get_position();

    // Start by checking if we fulfill given condition
    if(condition_func(rot_center, new_coord))
    {
	    return true;
    }
    
    #ifdef LOCAL_CONSTRAINTS_DEBUG_TEXT
        std::cout << "New segment at " << new_coord << " does not fulfill local constraint. Trying to rotate and prune..." << std::endl;
    #endif

    if(general_prune(rot_center, new_coord, LC_MAX_PRUNE_LENGTH, LC_PRUNE_NUM_STEPS, condition_func))
    {
	    segment.set_new_coordinate(new_coord);
	    return true;
    }
    	
    // Setup for rotation
    float rotation_dir = (float)(rand() % 2) * 2 - 1; // Decides +1 or -1. I.e rotate left or right first
    float rotation_step = LC_ROTATION_STEP_SIZE * rotation_dir; // How many degrees to rotate each step
    float tot_rotation = 0;
    mat4 trans_mat = T(rot_center.x, rot_center.y, rot_center.z);
    mat4 inv_trans_mat = T(-rot_center.x, -rot_center.y, -rot_center.z);
    mat4 rot_mat = Ry(rotation_step * M_PI / 180);
    
    // Rotate first direction. Prune for every rotation to see if the segment fulfills condition when pruned
    while(!general_prune(rot_center, new_coord, LC_MAX_PRUNE_LENGTH, LC_PRUNE_NUM_STEPS, condition_func))
    {
	    new_coord = MultVec3(Mult(trans_mat, Mult(rot_mat, inv_trans_mat)), new_coord); // Rotate segment
	    tot_rotation += rotation_step;
	
	    if(abs(tot_rotation + rotation_step) > LC_MAX_ROTATION)
	    {
	        new_coord = segment.get_new_coordinate(); // Did not find suitable rotation, reset to original
	        break;
	    }
    }
    
    rotation_step = -rotation_step;
    tot_rotation = 0;
    rot_mat = Ry(rotation_step * M_PI / 180);

    // Rotate other direction
    while(!general_prune(rot_center, new_coord, LC_MAX_PRUNE_LENGTH, LC_PRUNE_NUM_STEPS, condition_func))
    {
        new_coord = MultVec3(Mult(trans_mat, Mult(rot_mat,inv_trans_mat)), new_coord);
        tot_rotation += rotation_step;
        
        if(abs(tot_rotation + rotation_step) < LC_MAX_ROTATION)
        {
            #ifdef LOCAL_CONSTRAINTS_DEBUG_TEXT
                std::cout << "Did not find suitable rotation (and pruning)" << std::endl;
                #endif
            
            return false;
        }
    }
    
    segment.set_new_coordinate(new_coord);
    
    #ifdef LOCAL_CONSTRAINTS_DEBUG_TEXT
        std::cout << "Found suitable rotation (and pruning)! Segment now ends at " << new_coord << std::endl;
    #endif
	
    return true;
}

// Tries to prune road segment to make it fulfill the given condition
//
// Max_prune_length is the max percentage of a segment that is allowed to be pruned (cut).
// num_steps is the ammount of steps that should be done when pruning. Larger number means more steps
// which in turn means higher precision.
bool Roads_Generator::general_prune(Segment& segment, float max_prune_length, int num_steps, std::function<bool(vec3, vec3)> condition_func)
{
    vec3 existing_node_pos = segment.get_existing_node()->get_position();
    vec3 new_coord {segment.get_new_coordinate()};

    if(general_prune(existing_node_pos, new_coord, max_prune_length, num_steps, condition_func))
    {
	    segment.set_new_coordinate(new_coord);
	    return true;
    }
    return false;
}

bool Roads_Generator::general_prune(vec3 existing_node_pos, vec3 & new_coord, float max_prune_length, int num_steps, std::function<bool(vec3, vec3)> condition_func)
{
    // If road placement allowed, return true. Otherwise check prune
    if(condition_func(existing_node_pos, new_coord))
    {
        return true;
    }
    else
    {
        vec3 segment_dir = normalize(new_coord - existing_node_pos);
        float segment_length = norm(new_coord - existing_node_pos);
        float step_length = segment_length * max_prune_length / ((float)num_steps);
        vec3 step = segment_dir * step_length;
        vec3 max_prune_point = existing_node_pos + segment_dir * segment_length * max_prune_length;
                        
        // If we prune maximum distance and find a solution, we search for the best solution.
        // Otherwise we won't prune (return false, no solution)
        if(condition_func(existing_node_pos, max_prune_point))
        {
            // Step by step prune a bit of the segment. Stop as quickly as we fulfill condition
            for(int i = 1; i <= num_steps; i++)
            {
                new_coord = new_coord - step;
                if(condition_func(existing_node_pos, new_coord))
                {
                    #ifdef LOCAL_CONSTRAINTS_DEBUG_TEXT
                        std::cout << "Found suitable pruning. Segment now ends at " << new_coord << std::endl;
                    #endif

                    return true;
                }
            }
        }
        return false;
    }
}

// Calls check_and_rotate with lambda function that checks if the road road segment ends up in water
bool Roads_Generator::check_water(Segment& segment)
{
    auto water_lambda = [&](vec3 a, vec3 b) -> bool
    {
        return terrain.get_watermap_value((int)b.x, (int)b.z) != 1;
    };
    return check_and_rotate(segment, water_lambda);
}

// Calls check_and_rotate with lambda function that checks if the incline of the road segment is allowed
bool Roads_Generator::check_incline(Segment& segment)
{
    auto incline_lambda = [&](vec3 a, vec3 b) -> bool
    {
        return terrain.get_incline(a, b) < LC_MAX_INCLINE;
    };
    return check_and_rotate(segment, incline_lambda);
}

// Checks if the segment crosses another road tile and creates a node at that intersection.
void Roads_Generator::create_intersection_node(Segment& segment)
{

    std::vector<Road_Tile*> tiles {network.get_tiles()};
    for (auto it = tiles.begin(); it != tiles.end(); it++)
    {
        Road_Node* left {(*it)->get_left_node()};
        Road_Node* right {(*it)->get_right_node()};

        // from: https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
        vec3 p1 = segment.get_existing_node()->get_position();
        vec3 p2 = segment.get_new_coordinate();
        vec3 p3 = left->get_position();
        vec3 p4 = right->get_position();
        float t =  ((p1.x - p3.x) * (p3.z - p4.z) - (p1.z - p3.z) * (p3.x - p4.x)) / ((p1.x - p2.x) * (p3.z - p4.z) - (p1.z - p2.z) * (p3.x - p4.x));
        float u = -((p1.x - p2.x) * (p1.z - p3.z) - (p1.z - p2.z) * (p1.x - p3.x)) / ((p1.x - p2.x) * (p3.z - p4.z) - (p1.z - p2.z) * (p3.x - p4.x));

        if (t > 0 && t < 1 && u > 0 && u < 1)
        {
            vec3 intersection_point {p1.x + t * (p2.x - p1.x), 0, p1.z + t * (p2.z - p1.z)};

            if (norm(intersection_point - segment.get_new_coordinate()) < 0.2)
            {
                continue;
            }

            #ifdef LOCAL_CONSTRAINTS_DEBUG_TEXT
                std::cout << "Creating intersection at " << intersection_point << std::endl;
            #endif

            //remove old tile
            network.remove_tile(*it);

            //create new node
            Road_Node* intersection_node = network.add_node(intersection_point, {left, right, segment.get_existing_node()});
            segment.set_existing_node(intersection_node);
        }
    }
}

void Roads_Generator::add_new_segments_to_queue(Segment const& current_segment, Road_Node* current_node)
{
    //Nr of new segments 
    unsigned nr_new_segments = rand() % NEW_SEGMENT_RAND_NUM + NEW_SEGMENT_BASE_NUM;

    vec3 current_node_position = current_node->get_position();
    vec3 rotating_position = current_segment.get_existing_node()->get_position();

    // Calculate the rotations needed
    unsigned segment_count = nr_new_segments + 1; // Segment count including the already existing segment 
    float radians_between_segments = 2 * pi / segment_count;
    mat4 rotation_matrix = Ry(radians_between_segments);

    for (unsigned i = 0; i < segment_count - 1; i++)
    {

        // Get random segment length multiplier
        float multiplier = 1 + (rand() / (float) RAND_MAX - 0.5) / 4;

        // Translate to origin and rotate
        vec3 temp_position = (rotation_matrix * (rotating_position - current_node_position));

        rotating_position = temp_position + current_node_position;

        vec3 new_node_position = (normalize(temp_position) * DEFAULT_SEGMENT_LENGTH * multiplier) + current_node_position;

        new_node_position.y = terrain.get_interpol_height(new_node_position.x, new_node_position.z, 0.0);

        // Make sure segment ends inside terrain
        if(terrain.inside_terrain(new_node_position))
        {
            // Add new segment to prio-queue
            Segment new_segment {new_node_position, current_node};
            float new_prio_value = current_segment.get_prio_value() + norm(current_node_position - new_node_position) * new_segment.get_speed();
            new_segment.set_prio_value(new_prio_value);
            prio_queue.push(new_segment);
        }
    }
}

// Tries to connect the current segment to an existing node within a radius, 
// instead of having to create a new node.
//
// Return true if it manages to connect to an existing node.
//
bool Roads_Generator::connect_to_road_network(Segment & current_segment)
{
    
    vec3 new_position = current_segment.get_new_coordinate();
    std::vector<Road_Node*> existing_nodes = network.get_nodes();
    std::vector<Road_Tile*> existing_tiles = network.get_tiles();

    //snap to nearby existing node
    for(auto it = existing_nodes.begin(); it != existing_nodes.end(); it++) 
    {
        //dont compare to own existing node
        if ((*it) == current_segment.get_existing_node())
        {
            continue;
        }

        vec3 existing_node_position = (*it)->get_position();

        float dist = norm(existing_node_position - new_position);
        if (dist < LC_CONNECTION_RADIUS && dist != 0)
        {
            delete current_segment.get_new_node();
            current_segment.set_new_node(*it);

            #ifdef DEBUG_TEXT
                std::cout << "Connected new segment to existing node at: " << existing_node_position << std::endl;
            #endif

            return true;
        }
    }

    // Snap to nearby existing tile
    for(auto it = existing_tiles.begin(); it != existing_tiles.end(); it++) 
    {
        vec3 left_node_position = (*it)->get_left_node()->get_position();
        vec3 right_node_position = (*it)->get_right_node()->get_position();
        vec3 right_to_left = left_node_position - right_node_position;
        vec3 right_to_new = new_position - right_node_position;
        GLfloat t = dot(right_to_new, right_to_left) / pow(norm(right_to_left), 2);

        if(t > 0.0 && t < 1.0)
        {
            vec3 point_on_tile = right_node_position + t * right_to_left;
            float dist = norm(point_on_tile - new_position);
            if (dist < LC_CONNECTION_RADIUS && dist != 0)
            {
                auto new_node = network.split_tile_at(*it, point_on_tile);
                current_segment.set_new_node(new_node);

                #ifdef DEBUG_TEXT
                    std::cout << "Connected new segment to existing tile at: " << point_on_tile << std::endl;
                #endif

                return true;
            }
        }
    }

    return false;
}


// Checks if the perpendicular distance to a node is above a threshold.
bool Roads_Generator::check_distance_to_node(Segment & segment) const
{
    auto nodes = network.get_nodes();

    for(auto it = nodes.begin(); it != nodes.end(); it++) 
    {
        // Don't check with yourself!
        if (*it == segment.get_existing_node() || *it == segment.get_new_node())
        {
            continue;
        }

        // from: https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
        vec3 p1 = segment.get_existing_node()->get_position();
        vec3 p2 = segment.get_new_coordinate();

        vec3 x = (*it)->get_position();

        float dist = abs((p2.z - p1.z) * x.x - (p2.x - p1.x) * x.z + p2.x * p1.z - p2.z * p1.x) / sqrt(pow(p2.z - p1.z, 1) + pow(p2.x - p1.x, 2));

        // Check that perpendicular point is within segment
        vec3 line_dir = p2 - p1;
        vec3 point_dir = x - p1;

        float comp = dot(line_dir, point_dir);

        if (dist < LC_MIN_PERPENDICULAR_DIST && comp > 0.0 && comp < 1.0)
        {
            std::cout << "Disallow connection because of distance to existing node: " << p1 << " : " << p2 << std::endl;
            return false;
        }
    }

    return true;
}

// =================
// ===| Segment |===
// =================

void Segment::set_new_coordinate(vec3 const& coordinate)
{
    new_node->set_position(coordinate);
}

Segment::Segment(vec3 const& new_coordinate, Road_Node* existing_node)
    : existing_node{existing_node}
{
    new_node = new Road_Node(new_coordinate);
}

