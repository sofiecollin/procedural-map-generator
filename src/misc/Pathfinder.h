/*
    Given 2 positions in the world, this class finds the shortest route
    using A*. The cost function can take euclidean dist and height differences
    into consideration. The route is returned as a list of Pnodes which can be
    converted to a list of road nodes.
*/

#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "VectorUtils3.h"
#include "Terrain.h"
#include "Pnode.h"
#include <vector>
#include <tuple>
#include <memory>

class Pathfinder
{
public:
    Pathfinder(Terrain *in_terrain);
    Pathfinder(Terrain *in_terrain, int const& start_x, int const& start_y, int const& end_x, int const& end_y);
    ~Pathfinder();

    // Setters & Getters
    Terrain* get_terrain();
    std::shared_ptr <Pnode> get_start() { return start; }

    void set_terrain(Terrain *new_terrain);

    // Others
    std::vector<vec2> find_path();
    std::vector< std::shared_ptr <Pnode> > get_adjacent_nodes(std::shared_ptr <Pnode> node);
    Pnode get_lowest_cost_node();
    int find_lowest_cost(std::vector< std::shared_ptr <Pnode> > vec);
    void sort_adjacent_nodes(std::vector< Pnode >* adj_nodes);
    bool end_found(std::shared_ptr <Pnode> node);
    bool node_exist(int const& x, int const& y, std::vector< std::shared_ptr <Pnode> > vec);

private:
    Terrain *terrain;
    std::shared_ptr <Pnode> start;
    std::shared_ptr <Pnode> end;
    bool found_end = false;
    
    std::vector< std::shared_ptr <Pnode> > searched;
    std::vector< std::shared_ptr <Pnode> > to_search;
    std::vector< std::shared_ptr <Pnode> > path;
   
};

#endif