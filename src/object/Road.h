#ifndef ROAD_NETWORK_H
#define ROAD_NETWORK_H

#include <vector>
#include <memory>
#include <array>

#include "VectorUtils3.h"
#include "Object.h"
#include "Shader.h"
#include "Terrain.h"
//#include "parameters.h"

class Road_Node;
class Road_Tile;

class Road_Network
{
public:
    Road_Network();

    void render(Object_Shader const& shader) const;
    void update(float delta_time);

    void add_nodes(std::vector<vec3> const& list, GLfloat custom_width = 4);
    void add_tile(Road_Node* left, Road_Node* right);
    void remove_tile(Road_Tile* tile);
    Road_Node* add_node(vec3 const& pos);
    Road_Node* add_node(vec3 const& pos, std::initializer_list<Road_Node*> other_nodes, GLfloat custom_width = 4);
    Road_Node* try_add_node(Road_Node* node);
    Road_Node* try_add_node(Road_Node* node, std::initializer_list<Road_Node*> other_nodes);

    Road_Node* split_tile_at(Road_Tile* tile, vec3 pos);
    Road_Node* split_tile_between(Road_Node* left, Road_Node* right, vec3 pos);

    Road_Node* get_node(vec3 const& pos) const;
    std::vector<Road_Node*> get_nodes() const;
    std::vector<Road_Tile*> get_tiles() const;

    void generate_mesh(Terrain const& terrain);

private:

    std::vector<std::unique_ptr<Road_Node>> nodes {};
    std::vector<std::unique_ptr<Road_Tile>> tiles {};
};

class Road_Node
{
public:
    Road_Node(vec3 const& position);

    void render(Object_Shader const& shader) const;
    void sort_tiles();

    void add_tile(Road_Tile* tile);
    void remove_tile(Road_Tile* tile);
    std::vector<Road_Tile*> get_tiles() const { return tiles; }
    vec3 get_position() const { return position; }
    void set_position(vec3 pos) {position = pos;};

    GLfloat get_smallest_angle(vec3 pos);
    std::vector<Road_Node*> get_neighbour_nodes();
    Road_Tile* get_connecting_tile(Road_Node* other);
    
private:
    vec3 position;
    std::vector<Road_Tile*> tiles {};

    Object object {};
};

class Road_Tile
{
public:
    Road_Tile() = delete;
    Road_Tile(Road_Node* left, Road_Node* right, GLfloat custom_width = 4);

    void generate_mesh(Terrain const& terrain);
    void render(Object_Shader const& shader) const;
    float length() const { return norm(left->get_position() - right->get_position()); }

    Road_Node* get_left_node();
    Road_Node* get_right_node();
    float get_width() const { return width; }

private:

    std::array<vec3, 3> get_intersection_points(Road_Node* main, Road_Node* other, Terrain const& terrain) const;

    Road_Node* left {};
    Road_Node* right {};

    Object object;

    GLfloat width {4};

};
#endif
