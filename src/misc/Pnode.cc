#include "Pnode.h"


Pnode::Pnode(){};
Pnode::Pnode(int new_x, int new_y, Terrain *terrain)
    : x{new_x}, y{new_y} 
    {
        height = terrain->get_height(x, y);
        set_cost(0.0);
    }
Pnode::Pnode(int new_x, int new_y, Terrain *terrain, std::shared_ptr <Pnode> in_parent)
    : x{new_x}, y{new_y} 
    {
        parent = in_parent;
        height = terrain->get_height(x, y);
        set_end_node(parent->get_end_node());
        calc_cost();
         }
Pnode::~Pnode(){};


int Pnode::get_x()
{
    return x;
}

int Pnode::get_y()
{
    return y;
}

float Pnode::get_height()
{
    return height;
}

float Pnode::get_cost()
{
    return cost;
}

float Pnode::get_heuristic_cost()
{
    return heuristic;
}

bool Pnode::get_traverseable()
{
    return traverseable;
}

int Pnode::get_final_x()
{
    return end_node->get_x();
}

int Pnode::get_final_y()
{
    return end_node->get_y();
}

float Pnode::get_parent_cost()
{
    if (!parent)
    {
        return 0;
    }
    else
    {
        return parent->get_cost();
    }
}

std::shared_ptr <Pnode> Pnode::get_parent()
{
    return parent;
}

std::shared_ptr <Pnode> Pnode::get_end_node()
{
    return end_node;
}

void Pnode::set_x(int const& new_x)
{
    x = new_x;
}

void Pnode::set_y(int const& new_y)
{
    y = new_y;
}

void Pnode::set_height(float const& new_height)
{
    height = new_height;
}

void Pnode::set_cost(float const& new_cost)
{
    cost = new_cost;
}

void Pnode::set_heuristic_cost(float const& new_cost)
{
    heuristic = new_cost;
}

void Pnode::calc_heuristic_cost()
{
    float euclidean = pow(get_final_x()-x, 2) + pow(get_final_y()-y, 2);
    float steepness = pow(abs(get_height()-get_parent()->get_height()), 2);
    heuristic = EUCLIDEAN_PENALTY * euclidean + STEEPNESS_PENALTY * steepness;
}

void Pnode::set_traverseable(bool const& new_bool)
{
    traverseable = new_bool;
}

void Pnode::set_end_node(std::shared_ptr <Pnode> node)
{
    end_node = node;
}

void Pnode::set_parent(std::shared_ptr <Pnode> new_parent)
{
    parent = new_parent;
}

void Pnode::calc_cost()
{
    calc_heuristic_cost();
    cost = get_parent_cost() + 1;
    // Untested
    // float height_diff = abs(get_height() - get_parent().get_height())
    // cost = get_parent_cost() + pow(height_diff, 2);
}