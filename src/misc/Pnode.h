/*
    This is a helper class to the pathfinder function. 
    Nodes represent coordinates in the world. Cordinates
    are only integers as of right now. One node can be
    marked as a start node and one as an end node. 
*/

#ifndef PNODE_H
#define PNODE_H

#include "Terrain.h"
#include "parameters.h"
#include <memory>

class Pnode
{
public:

    // Constructors
    Pnode();
    Pnode(int new_x, int new_y, Terrain *terrain);
    Pnode(int new_x, int new_y, Terrain *terrain, std::shared_ptr<Pnode> in_parent);
    Pnode(Terrain* terrain);
    ~Pnode();

    // Setters & Getters
    int get_x();
    int get_y();
    int get_final_x();
    int get_final_y();
    float get_height();
    float get_heuristic_cost();
    float get_cost();
    float get_parent_cost();
    bool get_traverseable();
    std::shared_ptr <Pnode> get_parent();
    std::shared_ptr <Pnode> get_end_node();

    void set_x(int const& new_x);
    void set_y(int const& new_y);
    void set_height(float const& new_height);
    void set_cost(float const& new_cost);
    void set_heuristic_cost(float const& new_cost);
    void calc_heuristic_cost();
    void set_traverseable(bool const& new_bool);
    void set_end_node(std::shared_ptr <Pnode> node);
    void set_parent(std::shared_ptr <Pnode> new_parent);

    // Others
    void calc_cost();

private:
    int x, y;
    float height, cost, heuristic;
    bool traverseable;
    std::shared_ptr <Pnode> end_node;
    std::shared_ptr <Pnode> parent = nullptr;
};



#endif