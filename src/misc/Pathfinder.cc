/*
    OBS: Notes in this document refers to the Pnode class.
*/

#include "Pathfinder.h"
#include <iostream>

Pathfinder::Pathfinder(Terrain *in_terrain) : terrain{in_terrain} {}

Pathfinder::Pathfinder(Terrain *in_terrain, int const& start_x, int const& start_y, int const& end_x, int const& end_y)
    : terrain{in_terrain}, start{std::make_shared <Pnode> (start_x, start_y, terrain)}, end{std::make_shared <Pnode> (end_x, end_y, terrain)}
    {
        start->set_end_node(end);
    }

Pathfinder::~Pathfinder(){}

Terrain* Pathfinder::get_terrain()
{
    return terrain;
}

void Pathfinder::set_terrain(Terrain *new_terrain)
{
    terrain = new_terrain;
}

bool Pathfinder::end_found(std::shared_ptr <Pnode> node)
{
    if (node->get_x() == end->get_x() && node->get_y() == end->get_y())
    {
        return true;
    }
    else 
    {
        return false;
    }
}

// Findes and returns the cheapest path, with A*, of the object as a vector of coordinates. 
// OBS, nodes in this case are coordinates (as integers)
std::vector<vec2> Pathfinder::find_path()
{
    // Init: add the first node to the to_search
    to_search.push_back(start);
    while(to_search.size() > 0 && !found_end)
    {
        std::vector< std::shared_ptr <Pnode> > v;       // Create temp vector
        v = get_adjacent_nodes(to_search.back());       // Fill with adjacent nodes to last node in vector
        searched.push_back(to_search.back());           // Move last to searched
        to_search.pop_back();                           // Remove last element from to_search

        while (v.size() > 0 && !found_end)
        {
            // Check v, if end found set found_end = true. Move all to to_search
            to_search.insert(to_search.begin(),v.back());

            if (end_found(v.back()))
            {
                found_end = true;
                end = v.back();
                searched.push_back(v.back());
            }

            v.pop_back();
        }

        // If we haven't found the goal, move the node with lowest cost
        // to the back of to_search (which means it will be searched next)
        if (!found_end)
        {
            int lowest = find_lowest_cost(to_search);
            auto it = to_search.begin() + lowest;
            std::rotate(it, it+1, to_search.end());
        }    

    }  

    // Goal has been found. Create path
    std::vector<vec2> path;
    bool pathing = true;
    std::shared_ptr <Pnode> temp = end;
    while (pathing)
    {
        vec2 coords {(float) temp->get_x(), (float) temp->get_y()};
        path.push_back(coords);
        temp = temp->get_parent();
        if (!temp) { pathing = false; }
    }
    
    return path;
}

// Finds and returns the position (in a vector) of the node with the lowest cost + 
// heuristic cost of a vector of nodes. 
int Pathfinder::find_lowest_cost(std::vector< std::shared_ptr <Pnode> > vec)
{
    int i = 0;
    int lowest = 0;
    float temp_cost = vec.front()->get_cost() + vec.front()->get_heuristic_cost();
    for (auto it : vec)
    {
        float new_cost = it->get_cost() + it->get_heuristic_cost();
        if (new_cost < temp_cost)
        {
            lowest = i;
            temp_cost = new_cost;
        }
        i++;
    }
    return lowest;
}

// Checks if a node already exists, based on it's in coordinates. 
bool Pathfinder::node_exist(int const& x, int const& y, std::vector< std::shared_ptr <Pnode> > vec)
{
    // This line is probably unnecessary
    if (!vec.empty())
    {
        while (vec.size() > 0)
        {
            if (vec.back()->get_x() == x && vec.back()->get_y() == y)
            {
                return true;
            }
            else
            {
                vec.pop_back();
            }
            
        }
    }
    return false;
}

// Takes a node and returns a vector of adjacent nodes. 
std::vector< std::shared_ptr <Pnode> > Pathfinder::get_adjacent_nodes(std::shared_ptr <Pnode> node)
{
    std::vector< std::shared_ptr <Pnode> > v;
    int x = node->get_x();
    int y = node->get_y();
    int curr_x = x-1;
    int curr_y = y-1;
    if (!node_exist(curr_x, curr_y, searched) &&
        !node_exist(curr_x, curr_y, to_search))
        {
            v.push_back(std::make_shared<Pnode> (curr_x, curr_y, terrain, node));
            
        }
    curr_x = x;
    curr_y = y-1;
    if (!node_exist(curr_x, curr_y, searched) &&
        !node_exist(curr_x, curr_y, to_search))
        {
            v.push_back(std::make_shared<Pnode> (curr_x, curr_y, terrain, node));
            
        }
    curr_x = x+1;
    curr_y = y-1;
    if (!node_exist(curr_x, curr_y, searched) &&
        !node_exist(curr_x, curr_y, to_search))
        {
            v.push_back(std::make_shared<Pnode> (curr_x, curr_y, terrain, node));
            
        }
    curr_x = x-1;
    curr_y = y;
    if (!node_exist(curr_x, curr_y, searched) &&
        !node_exist(curr_x, curr_y, to_search))
        {
            v.push_back(std::make_shared<Pnode> (curr_x, curr_y, terrain, node));
            
        }
    curr_x = x+1;
    curr_y = y;
    if (!node_exist(curr_x, curr_y, searched) &&
        !node_exist(curr_x, curr_y, to_search))
        {
            v.push_back(std::make_shared<Pnode> (curr_x, curr_y, terrain, node));
            
        }
    curr_x = x-1;
    curr_y = y+1;
    if (!node_exist(curr_x, curr_y, searched) &&
        !node_exist(curr_x, curr_y, to_search))
        {
            v.push_back(std::make_shared<Pnode> (curr_x, curr_y, terrain, node));
            
        }
    curr_x = x;
    curr_y = y+1;
    if (!node_exist(curr_x, curr_y, searched) &&
        !node_exist(curr_x, curr_y, to_search))
        {
            v.push_back(std::make_shared<Pnode> (curr_x, curr_y, terrain, node));
            
        }
    curr_x = x+1;
    curr_y = y+1;
    if (!node_exist(curr_x, curr_y, searched) &&
        !node_exist(curr_x, curr_y, to_search))
        {
            v.push_back(std::make_shared<Pnode> (curr_x, curr_y, terrain, node));
            
        }
    return v;
}
