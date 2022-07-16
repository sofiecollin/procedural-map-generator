#include "Road.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <array>

Road_Network::Road_Network()
{
}

void Road_Network::render(Object_Shader const& shader) const
{
    for (auto it = tiles.begin(); it != tiles.end(); it++)
    {
        (*it)->render(shader);
    }

    for (auto it = nodes.begin(); it != nodes.end(); it++)
    {
        // (*it)->render(shader); // Render nodes
    }
}

void Road_Network::update(float delta_time)
{
}

void Road_Network::add_nodes(std::vector<vec3> const& list, GLfloat custom_width)
{
    if (list.size() <= 1)
    {
        return;
    }

    Road_Node* prev;
    for(auto it = list.begin(); it != list.end(); it++)
    {
        if (it == list.begin())
        {
            prev = add_node(*it);
        } else
        {
            prev = add_node(*it, {prev}, custom_width);
        }
    }
}

Road_Node* Road_Network::add_node(vec3 const& position)
{
    Road_Node* node = new Road_Node(position);
    nodes.push_back(std::unique_ptr<Road_Node>(node));
    return node;
}

// Adds a node to the specified position and connects it the the given other nodes.
Road_Node* Road_Network::add_node(vec3 const& position, std::initializer_list<Road_Node*> other_nodes, GLfloat custom_width)
{
    Road_Node* node = new Road_Node(position);

    for(auto it = other_nodes.begin(); it != other_nodes.end(); it++)
    {
        auto tile = std::make_unique<Road_Tile>(node, *it, custom_width);
        node->add_tile(tile.get());
        (*it)->add_tile(tile.get());
        tiles.push_back(std::move(tile));
    }

    nodes.push_back(std::unique_ptr<Road_Node>(node));
    return node;
}

// Adds a node if that node does not alreay exist in the network.
Road_Node* Road_Network::try_add_node(Road_Node* node)
{
    auto it = std::find_if(nodes.begin(), nodes.end(), [node](auto const& n){ return n->get_position() == node->get_position(); });
    if (it == nodes.end())
    {
        nodes.push_back(std::unique_ptr<Road_Node>(node));
        return node;
    }
    else 
    {
        return it->get();
    }
}

// Adds a node if that node does not alreay exist in the network. 
// Also connects the new/old node the the other given nodes
Road_Node* Road_Network::try_add_node(Road_Node* node, std::initializer_list<Road_Node*> other_nodes)
{
    auto it = std::find_if(nodes.begin(), nodes.end(), 
        [node](auto const& n)
        { 

            return n->get_position().x == node->get_position().x && n->get_position().z == node->get_position().z; 
        });
    if (it == nodes.end())
    {
        nodes.push_back(std::unique_ptr<Road_Node>(node));

        for(auto other_it = other_nodes.begin(); other_it != other_nodes.end(); other_it++)
        {
            add_tile(node, *other_it);
        }

        return node;
    }
    else 
    {
        for(auto other_it = other_nodes.begin(); other_it != other_nodes.end(); other_it++)
        {
            add_tile(*other_it, it->get());
        }
        return it->get();
    }
}

// Adds a road tile between to existing nodes.
void Road_Network::add_tile(Road_Node* left, Road_Node* right)
{

    std::vector<Road_Node*> neighbours = left->get_neighbour_nodes();
    if(std::any_of(neighbours.begin(), neighbours.end(),
		   [right](Road_Node* node){ return node == right; }))
    {
	    std::cout << "\"Add tile\": Did not add new road, this road already exists!" << std::endl;
    }
    else
    {
        Road_Tile* tile = new Road_Tile(left, right);
        left->add_tile(tile);
        right->add_tile(tile);
        tiles.push_back(std::unique_ptr<Road_Tile>(tile));
    }
}

void Road_Network::remove_tile(Road_Tile* tile)
{
    Road_Node* left_node = tile->get_left_node();
    Road_Node* right_node = tile->get_right_node();
    left_node->remove_tile(tile);
    right_node->remove_tile(tile);
    
    for(auto it = tiles.begin(); it != tiles.end(); it++)
    {
	if ((*it).get() == tile)
	{
        tiles.erase(it);
	    break;
        }
    }
}

// Splits a tile at the given position.
Road_Node* Road_Network::split_tile_at(Road_Tile* tile, vec3 pos)
{
    Road_Node* left_node = tile->get_left_node();
    Road_Node* right_node = tile->get_right_node();
    remove_tile(tile);
    return add_node(pos, {left_node, right_node});
}

Road_Node* Road_Network::split_tile_between(Road_Node* left, Road_Node* right, vec3 pos)
{
    Road_Tile* tile = left->get_connecting_tile(right);
    return split_tile_at(tile, pos);
}

Road_Node* Road_Network::get_node(vec3 const& pos) const
{
    for(auto it = nodes.begin(); it != nodes.end(); it++)
    {
        if ((*it)->get_position() == pos)
        {
            return it->get();
        }
    }
    return nullptr;
}

std::vector<Road_Node*> Road_Network::get_nodes() const
{
    std::vector<Road_Node*> nodes_raw {};
    std::transform(nodes.begin(), nodes.end(), std::back_inserter(nodes_raw), [](auto &p) { return p.get(); });
    return nodes_raw;
}

std::vector<Road_Tile*> Road_Network::get_tiles() const
{
    std::vector<Road_Tile*> tiles_raw {};
    std::transform(tiles.begin(), tiles.end(), std::back_inserter(tiles_raw), [](auto &p) { return p.get(); });
    return tiles_raw;
}

void Road_Network::generate_mesh(Terrain const& terrain)
{
    for(auto it = nodes.begin(); it != nodes.end(); it++)
    {
        (*it)->sort_tiles();
    }

    for(auto it = tiles.begin(); it != tiles.end(); it++)
    {
        (*it)->generate_mesh(terrain);
    }
}

// ===================
// ===| Road Node |===
// ===================

Road_Node::Road_Node(vec3 const& position)
    : position {position}
{
    object = Object{"rock1", position};
}

void Road_Node::render(Object_Shader const& shader) const
{
    shader.load_material_properties(object);

    int nr_tiles = tiles.size();
    for (int i = 0; i < nr_tiles; i++)
    {
        shader.load_model_matrix(T(0, i, 0) * object.get_model_matrix());

        object.render(shader.get_program_id());
    }
}

void Road_Node::add_tile(Road_Tile* tile)
{
    tiles.push_back(tile);
}

void Road_Node::remove_tile(Road_Tile* tile)
{
    for(auto it = tiles.begin(); it != tiles.end(); it++)
    {
        if (*it == tile)
        {
            tiles.erase(it);
            break;
        }
    }
}

Road_Node* Road_Tile::get_left_node()
{
    return left;
}

Road_Node* Road_Tile::get_right_node()
{
    return right;
}

// Returns a list of all nodes that are connected via a tile.
std::vector<Road_Node*> Road_Node::get_neighbour_nodes()
{
    std::vector<Road_Node*> neighbours;
    for(auto it = tiles.begin(); it != tiles.end(); it++)
    {
        Road_Node* left_node = (*it)->get_left_node();
        Road_Node* right_node = (*it)->get_right_node();

        if (left_node == this)
        {
            neighbours.push_back(right_node);
        }
        else
        {
            neighbours.push_back(left_node);
        }
    }
    return neighbours;
}

Road_Tile* Road_Node::get_connecting_tile(Road_Node* other)
{
    for(auto it = tiles.begin(); it != tiles.end(); it++)
    {
	    Road_Node* left_node = (*it)->get_left_node();
	    Road_Node* right_node = (*it)->get_right_node();

        if (left_node == other || right_node == other)
        {
            return (*it);
        }
    }
    return nullptr;
}

// Returns the smallest angle between the vector between this node and given point
// and the tiles that are connected to this node.
GLfloat Road_Node::get_smallest_angle(vec3 pos)
{
    float smallest_angle = 180;
    vec3 a = normalize(pos - get_position());
    std::vector<Road_Node*> neighbours = get_neighbour_nodes();
    for(auto it = neighbours.begin(); it != neighbours.end(); it++)
    {
	    vec3 b = normalize((*it)->get_position() - get_position());
	    GLfloat angle = acos(dot(a, b)) * 180.0 / M_PI;
    
        if (angle < smallest_angle)
	    {
	        smallest_angle = angle;
	    }
    }
    
    return smallest_angle;
}

// Sort the list of tiles so that they are in order of angle around the unit circle. 
void Road_Node::sort_tiles()
{
    std::sort(tiles.begin(), tiles.end(), 
        [this](Road_Tile* a, Road_Tile* b)
        {
            vec3 a_dir = this->get_position() - a->get_left_node()->get_position();
            if (a_dir == vec3{0,0,0})
            {
                a_dir = this->get_position() - a->get_right_node()->get_position();
            }

            double a_angle {acos(a_dir.x / sqrt(pow(a_dir.x, 2) + pow(a_dir.z, 2)))};

            if (a_dir.z < 0)
            {
                a_angle = 6.283 - a_angle;
            }

            vec3 b_dir = this->get_position() - b->get_left_node()->get_position();
            if (b_dir == vec3{0,0,0})
            {
                b_dir = this->get_position() - b->get_right_node()->get_position();
            }

            double b_angle {acos(b_dir.x / sqrt(pow(b_dir.x, 2) + pow(b_dir.z, 2)))};

            if (b_dir.z < 0)
            {
                b_angle  = 6.283 - b_angle;
            }

            return a_angle < b_angle;

        });
}

// ===================
// ===| Road Tile |===
// ===================

Road_Tile::Road_Tile(Road_Node* left, Road_Node* right, GLfloat custom_width)
    : left {left}, right {right}
{
    width = custom_width;
}

// Generates the object used when rendering the road
void Road_Tile::generate_mesh(Terrain const& terrain)
{   
    // Init 
    vec3 up {0,1,0};
    auto left_points = get_intersection_points(left, right, terrain);
    auto right_points = get_intersection_points(right, left, terrain);

    float dist = Norm(VectorSub(left_points[0], right_points[1]));
    int nr_points = floor(dist/0.1);
    float fact = 1.0/((float)(nr_points+1));
    float factor = length() / 4;


    std::vector<GLfloat> vertex_array {
        left_points[0].x, left_points[0].y, left_points[0].z,       // 0
        left_points[1].x, left_points[1].y, left_points[1].z,       // 1
        left_points[2].x, left_points[2].y, left_points[2].z,       // 2
        right_points[0].x, right_points[0].y, right_points[0].z,    // 3
        right_points[1].x, right_points[1].y, right_points[1].z,    // 4
        right_points[2].x, right_points[2].y, right_points[2].z,    // 5
    };

    std::vector<GLfloat> normal_array {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 
        0.0f, 1.0f, 0.0f,
    };

     std::vector<GLfloat> tex_coord_array {
        0.0f, 0.0f, 
        0.0f, 1.0f, 
        -1.0f, 0.5f,
        factor, 1.0f, 
        factor, 0.0f, 
        factor+1.0f, 0.5f
    };

    std::vector<vec3> upper;
    std::vector<vec3> lower;
    vec3 up_vec = VectorSub(right_points[0], left_points[1]);
    vec3 down_vec = VectorSub(right_points[1], left_points[0]);
    float tex_fact = factor/((float)(nr_points+1));
    for (int i = 0; i < nr_points; i++)
    {
        // Vertex array
        vec3 new_up{left_points[1]+ScalarMult(up_vec, fact*(i+1))};
        vec3 new_down{left_points[0]+ScalarMult(down_vec, fact*(i+1))};
        // choose height
        float up_height = terrain.get_interpol_height(new_up.x, new_up.z, 0.0);
        float down_height = terrain.get_interpol_height(new_down.x, new_down.z, 0.0);

        vertex_array.push_back(new_up.x);
        vertex_array.push_back(up_height + 0.1);
        vertex_array.push_back(new_up.z);
        vertex_array.push_back(new_down.x);
        vertex_array.push_back(down_height + 0.1);
        vertex_array.push_back(new_down.z);

        // Normal array
        normal_array.push_back(0.0);
        normal_array.push_back(1.0);
        normal_array.push_back(0.0);
        normal_array.push_back(0.0);
        normal_array.push_back(1.0);
        normal_array.push_back(0.0);

        // Tex array
        tex_coord_array.push_back(((float)(i+1))*tex_fact);
        tex_coord_array.push_back(1.0);
        tex_coord_array.push_back(((float)(i+1))*tex_fact);
        tex_coord_array.push_back(0.0);
    }

    std::vector<GLuint> index_array {
        0, 1, 2, 
        3, 4, 5,
    };

    // Index list for all except last two triangels
    for (int i = 0; i < nr_points; i++)
    {
        if (i == 0)
        {
            index_array.push_back(0);
            index_array.push_back(6);
            index_array.push_back(1);
            index_array.push_back(0);
            index_array.push_back(7);
            index_array.push_back(6);
        }
        else
        {
            int index = 2*i+4;
            index_array.push_back(index+1);
            index_array.push_back(index+2);
            index_array.push_back(index);
            index_array.push_back(index+1);
            index_array.push_back(index+3);
            index_array.push_back(index+2);
        }
    }

    // Indices for last two triangels
    index_array.push_back(nr_points*2+5);
    index_array.push_back(3);
    index_array.push_back(nr_points*2+4);
    index_array.push_back(nr_points*2+5);
    index_array.push_back(4);
    index_array.push_back(3);


    //***************************************************************************************************
    

    // Create foundation, vertices:
    float new_x = vertex_array.at(0);
    float new_z = vertex_array.at(2);
    float new_y = terrain.get_interpol_height(new_x, new_z, vertex_array.at(1));
    vertex_array.push_back(new_x);
    vertex_array.push_back(new_y);
    vertex_array.push_back(new_z);
    
    
    new_x = vertex_array.at(3);
    new_z = vertex_array.at(5);
    new_y = terrain.get_interpol_height(new_x, new_z, vertex_array.at(4));
    vertex_array.push_back(new_x);
    vertex_array.push_back(new_y);
    vertex_array.push_back(new_z);
    
    for (int i = 0; i < nr_points; i++)
    {
        new_x = vertex_array.at((2*i+6)*3);
        new_z = vertex_array.at((2*i+6)*3+2);
        new_y = terrain.get_interpol_height(new_x, new_z, 0.0);
        vertex_array.push_back(new_x);
        vertex_array.push_back(new_y);
        vertex_array.push_back(new_z);
        new_x = vertex_array.at((2*i+7)*3);
        new_z = vertex_array.at((2*i+7)*3+2);
        new_y = terrain.get_interpol_height(new_x, new_z, 0.0);
        vertex_array.push_back(new_x);
        vertex_array.push_back(new_y);
        vertex_array.push_back(new_z);
    }
    new_x = vertex_array.at(9);
    new_z = vertex_array.at(11);
    new_y = terrain.get_interpol_height(new_x, new_z, vertex_array.at(10));
    vertex_array.push_back(new_x);
    vertex_array.push_back(new_y);
    vertex_array.push_back(new_z);
    
    
    new_x = vertex_array.at(12);
    new_z = vertex_array.at(14);
    new_y = terrain.get_interpol_height(new_x, new_z, vertex_array.at(3));
    vertex_array.push_back(new_x);
    vertex_array.push_back(new_y);
    vertex_array.push_back(new_z);
    
    // Foundation, normals:
    for (int i = 0; i < nr_points*2+4; i++)
    {
        normal_array.push_back(0.0);
        normal_array.push_back(1.0);
        normal_array.push_back(0.0);
    }

    // Foundation, tex coords:
    tex_coord_array.push_back(1);
    tex_coord_array.push_back(1);
    tex_coord_array.push_back(0);
    tex_coord_array.push_back(0);
    for (int i = 0; i < nr_points; i++)
    {
        tex_coord_array.push_back(1);
        tex_coord_array.push_back(1);
        tex_coord_array.push_back(0);
        tex_coord_array.push_back(0);
    }
    tex_coord_array.push_back(1);
    tex_coord_array.push_back(1);
    tex_coord_array.push_back(0);
    tex_coord_array.push_back(0);

    // Foundation, indices:
    for (int i = 0; i< nr_points; i++)
    {
        if (i == 0)
        {
            // down
            index_array.push_back(12);
            index_array.push_back(15);
            index_array.push_back(7);
            index_array.push_back(12);
            index_array.push_back(7);
            index_array.push_back(0);

            // up
            index_array.push_back(13);
            index_array.push_back(6);
            index_array.push_back(14);
            index_array.push_back(13);
            index_array.push_back(1);
            index_array.push_back(6);
        }
        else
        {
            // up
            int index = i*2+5;
            index_array.push_back(index+(nr_points+1)*2);
            index_array.push_back(index+(nr_points+1)*2+2);
            index_array.push_back(index+2);
            index_array.push_back(index+(nr_points+1)*2);
            index_array.push_back(index+2);
            index_array.push_back(index);

            // down
            index = i*2+4;
            index_array.push_back(index+(nr_points+1)*2);
            index_array.push_back(index);
            index_array.push_back(index+2);
            index_array.push_back(index+(nr_points+1)*2);
            index_array.push_back(index+2);
            index_array.push_back(index+(nr_points+1)*2+2);
        } 
    }

    // Last part:
    int index = nr_points*2+5;
    index_array.push_back(index+(nr_points+1)*2);
    index_array.push_back(4);
    index_array.push_back(index);
    index_array.push_back(index + (nr_points+1)*2);
    index_array.push_back(index + (nr_points+1)*2+2);
    index_array.push_back(4);
    
    index = nr_points*2+4;
    index_array.push_back(index+ (nr_points+1)*2);
    index_array.push_back(index);
    index_array.push_back(3);
    index_array.push_back(index + (nr_points+1)*2);
    index_array.push_back(3);
    index_array.push_back(index + (nr_points+1)*2+2);

    object = Object {vertex_array, normal_array, tex_coord_array, index_array, "road.png"};

}

// Calculates the intersection points of the tile when connecting to the node defined as 'main'
std::array<vec3, 3> Road_Tile::get_intersection_points(Road_Node* main, Road_Node* other, Terrain const& terrain) const
{
    auto tiles = main->get_tiles();

    vec3 up {0, 1, 0};
    vec3 own_tile_dir = normalize(main->get_position() - other->get_position()); // Direction of tile, pointing towards 'main'
    vec3 own_perp_dir = cross(up, own_tile_dir); // Direction vector pointing left relative to road

    // If no other roads connected to node, return own corners as intersection points
    if (tiles.size() == 1)
    {
        return std::array<vec3, 3> {
            main->get_position() + own_perp_dir * width / 2, 
            main->get_position() - own_perp_dir * width / 2, 
            main->get_position()};
    }

    // Find the two closest neighbouring roads in the intersection, here called right and left
    // The tile list is sorted according to angle, hence the neighbours will be +1/-1
    auto it = std::find(tiles.begin(), tiles.end(), this);
    Road_Tile* left_road {*(it + 1)};
    Road_Tile* right_road {*(it - 1)};

    // If iterator at end of tile list, loop around
    if (it + 1 == tiles.end())
    {
        left_road = *(tiles.begin());
    }

    if (it == tiles.begin())
    {
        right_road = *(tiles.end() - 1);
    }


    // Find intersection of this road and neighbouring road to the left
    Road_Node* left_other {left_road->get_left_node() == main ? left_road->get_right_node() : left_road->get_left_node()};
    vec3 left_tile_dir {normalize(main->get_position() - left_other->get_position())};
    vec3 left_perp_dir {cross(up, left_tile_dir)};

    // Get positions. Setting y coordinate to 0 since intersection point is calculated in 2D
    vec3 main_pos {main->get_position()};
    vec3 other_pos {other->get_position()};
    vec3 left_other_pos {left_other->get_position()};
    main_pos.y = 0;
    other_pos.y = 0;
    left_other_pos.y = 0;

    // Calculate intersection point
    vec3 p1 {main_pos + own_perp_dir * width / 2};
    vec3 p2 {other_pos + own_perp_dir * width / 2};
    vec3 p3 {main_pos - left_perp_dir * left_road->get_width() / 2};
    vec3 p4 {left_other_pos - left_perp_dir * left_road->get_width() / 2};

    // from: https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
    float t = ((p1.x - p3.x) * (p3.z - p4.z) - (p1.z - p3.z) * (p3.x - p4.x)) / ((p1.x - p2.x) * (p3.z - p4.z) - (p1.z - p2.z) * (p3.x - p4.x));
    vec3 left_intersection_point {p1.x + t * (p2.x - p1.x), 0, p1.z + t * (p2.z - p1.z)};
    if (isnan(t) || abs(t) > 2)
    {
        left_intersection_point = p1;
    }

    left_intersection_point.y = terrain.get_interpol_height(left_intersection_point.x, left_intersection_point.z, 0.0) + 0.2f;


    // Find intersection of this road and neighbouring road to the right
    Road_Node* right_other {right_road->get_left_node() == main ? right_road->get_right_node() : right_road->get_left_node()};
    vec3 right_tile_dir {normalize(main->get_position() - right_other->get_position())};
    vec3 right_perp_dir {cross(up, right_tile_dir)};

    // Get positions. Setting y coordinate to 0 since intersection point is calculated in 2D
    vec3 right_other_pos {right_other->get_position()};
    right_other_pos.y = 0;

    // Calculate intersection point
    p1 = main_pos - own_perp_dir * width / 2;
    p2 = other_pos - own_perp_dir * width / 2;
    p3 = main_pos + right_perp_dir * right_road->get_width() / 2;
    p4 = right_other_pos + right_perp_dir * right_road->get_width() / 2;

    t = ((p1.x - p3.x) * (p3.z - p4.z) - (p1.z - p3.z) * (p3.x - p4.x)) / ((p1.x - p2.x) * (p3.z - p4.z) - (p1.z - p2.z) * (p3.x - p4.x));
    vec3 right_intersection_point {p1.x + t * (p2.x - p1.x), 0, p1.z + t * (p2.z - p1.z)};
    if (isnan(t) || abs(t) > 2)
    {
        right_intersection_point = p1;
    }

    

    right_intersection_point.y = terrain.get_interpol_height(right_intersection_point.x, right_intersection_point.z, 0.0) + 0.2f;

    vec3 mid_point = main->get_position();
    mid_point.y = terrain.get_interpol_height(mid_point.x, mid_point.z, 0.0) + 0.2f;

    return std::array<vec3, 3>{left_intersection_point, right_intersection_point, mid_point};
}

void Road_Tile::render(Object_Shader const& shader) const
{
    shader.load_model_matrix(object.get_model_matrix());
    shader.load_material_properties(object);

    object.render(shader.get_program_id());
}
