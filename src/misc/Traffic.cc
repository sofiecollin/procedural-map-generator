#include "Traffic.h"

boost::mt19937 gen;

Traffic::Traffic()
{
}

Traffic::Traffic(std::shared_ptr <Road_Network*> new_network)
{
    network = new_network;
}

Traffic::Traffic(std::shared_ptr <Road_Network*> new_network, int const& nr_cars)
{
    network = new_network;
    for (int i = 0; i < nr_cars; i++)
    {
        create_car();
    }
}

Traffic::~Traffic()
{
}

void Traffic::create_car()
{
    Driver temp_car;
    
    // Set starting pos
    int nr_nodes = (*network)->get_nodes().size();
    int start_nr = get_rand() % nr_nodes;
    vec3 init_pos = (*network)->get_nodes().at(start_nr)->get_position();
    temp_car.set_position(init_pos);
    temp_car.set_start_node(std::make_shared <Road_Node*> ((*network)->get_nodes().at(start_nr)));

    // Set first goal
    int nr_adj_nodes = (*network)->get_nodes().at(start_nr)->get_neighbour_nodes().size();
    int goal_nr = get_rand() % nr_adj_nodes;
    vec3 goal_pos = (*network)->get_nodes().at(start_nr)->get_neighbour_nodes().at(goal_nr)->get_position();
    temp_car.new_route(goal_pos);
    temp_car.set_goal_node(std::make_shared <Road_Node*> ((*network)->get_nodes().at(start_nr)->get_neighbour_nodes().at(goal_nr)));

    // Set other things
    temp_car.set_speed(0.05);
    temp_car.rot_towards_goal();
    temp_car.set_state(2);
    cars.push_back(temp_car);
}

void Traffic::render_cars(Object_Shader const& shader) const
{
    for (auto it = cars.begin(); it != cars.end(); it++)
    {
        shader.load_material_properties(it->get_car());
        shader.load_model_matrix(it->get_car().get_model_matrix());
        it->get_car().render(shader.get_program_id());
    }
}

void Traffic::update()
{
    for (unsigned i = 0; i < cars.size(); i++)
    {
        cars[i].update(terrain);
        if (cars[i].get_state() == 0)
        {
            assign_new_route(i);
        }
    }
}

void Traffic::set_all_states(int const& new_state)
{
    for (auto it = cars.begin(); it != cars.end(); it++)
    {
        it->set_state(new_state);
    }
}

void Traffic::set_terrain(Terrain* new_terrain)
{
    terrain = new_terrain;
}

void Traffic::assign_new_route(unsigned const& i)
{
    // Find number of ajacent nodes
    int nr_adj_nodes = (*cars[i].get_goal_node())->get_neighbour_nodes().size();
    // init goal node nr
    int goal_nr = 0;
    // If multiple neighbours
    if (!(nr_adj_nodes == 1))
    {
        // Get a random neighbour which will be our goal
        goal_nr = get_rand() % nr_adj_nodes;
        // If new goal is same as old
        if (*cars[i].get_start_node() == (*cars[i].get_goal_node())->get_neighbour_nodes().at(goal_nr))
        {
            goal_nr--;
            if (goal_nr < 0)
            {
                goal_nr = nr_adj_nodes-1;
            }
        }
    }
    cars[i].set_start_node(cars[i].get_goal_node());
    vec3 goal_pos = (*cars[i].get_start_node())->get_neighbour_nodes().at(goal_nr)->get_position();
    cars[i].set_goal_node(std::make_shared <Road_Node*> ((*cars[i].get_start_node())->get_neighbour_nodes().at(goal_nr)));
    cars[i].set_goal(goal_pos);
    cars[i].new_route(goal_pos);
    cars[i].rot_towards_goal();
    cars[i].set_state(1);
}

std::shared_ptr <Road_Node*> Traffic::get_new_destination(std::shared_ptr <Road_Node*> node)
{
    std::vector<Road_Node*> neighbours = (*node)->get_neighbour_nodes();
    int N = neighbours.size();
    int n = get_rand() % N;
    return std::make_shared<Road_Node*> (neighbours[n]);
}

int Traffic::get_rand()
{
    boost::uniform_int<> dist(0,1000000);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rand_nr(gen, dist);
    return rand_nr();
}