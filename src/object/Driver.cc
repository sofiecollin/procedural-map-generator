#include "Driver.h"

Driver::Driver()
{
    car.set_scale((vec3){0.4, 0.4, 0.4});
    car.set_rotation(IdentityMatrix());
}

Driver::~Driver()
{
}

// Checks state and does the appropriate action
void Driver::update(Terrain* terrain)
{
    // Done state, handled by traffic class.
    if (state == 0)
    {
    }

    // Driving state
    else if (state == 1)
    {
        // Drive forward
        drive(terrain);
        // If we are close enough to our goal, switch state and move to goal
        if (goal_reached())
        {
            car.set_position(goal);
            set_state(0);
        }
    }

    // Idle state, does nothing atm. 
    else if (state == 2)
    {
    }
}

// Takes a goal position, sets it as goal and calulates route
void Driver::new_route(vec3 const& new_goal)
{
    set_goal(new_goal);
    calc_route();
}

// Takes a goal node, sets it as goal and calulates route
void Driver::new_route(std::shared_ptr<Road_Node> node_ptr)
{
    set_goal(node_ptr);
    calc_route();
}

void Driver::set_goal(std::shared_ptr<Road_Node> node_ptr)
{
    goal = node_ptr->get_position();
}

void Driver::set_goal(vec3 const& new_goal)
{
    goal = new_goal;
}

void Driver::set_state(int const& new_state)
{
    if ( -1 < state && state < 3)
    {
        state = new_state;
    }
    else
    {
        printf("Bad state, only allows for 0, 1 and 2\n");
    }
}

void Driver::set_offset(float const& new_offset)
{
    offset = new_offset;
}

void Driver::set_speed(float const& new_speed)
{
    speed = new_speed;
}

void Driver::set_position(float const& x, float const& y, float const& z)
{
    vec3 new_pos{x, y, z};
    car.set_position(new_pos);
}

void Driver::set_position(vec3 const& new_pos)
{
    car.set_position(new_pos);
}

void Driver::set_start_node(std::shared_ptr <Road_Node*> new_start)
{
    start_node = new_start;
}

void Driver::set_goal_node(std::shared_ptr <Road_Node*> new_goal)
{
    goal_node = new_goal;
}

int Driver::get_state()
{
    return state;
}

std::shared_ptr <Road_Node*> Driver::get_goal_node()
{
    return goal_node;
}

std::shared_ptr <Road_Node*> Driver::get_start_node()
{
    return start_node;
}

Object Driver::get_car() const
{
    return car;
}

Object Driver::get_car()
{
    return car;
}

void Driver::rot_towards_goal()
{
    vec3 goal_y = ScalarMult(Normalize(drive_dir), -1.0);
    vec3 goal_z = Normalize(CrossProduct((vec3){0.0, 1.0, 0.0}, goal_y));
    vec3 goal_x = Normalize(CrossProduct(goal_y, goal_z));

    mat4 new_rot = Transpose(mat3tomat4(SetMat3(goal_z.x, goal_z.y, goal_z.z, goal_x.x, goal_x.y, goal_x.z, goal_y.x, goal_y.y, goal_y.z)));
    car.set_rotation(new_rot);
}

// Calcs new route. Assumes new goal, speed and offset has already been entered
void Driver::calc_route()
{
    vec3 path = VectorSub(goal, car.get_position());
    drive_dir = Normalize(path);
}

// Moves car towards goal
void Driver::drive(Terrain* terrain)
{
    vec3 old_pos = car.get_position();
    vec3 new_pos = VectorAdd(old_pos, ScalarMult(drive_dir, speed));

    // Adjust height
    vec3 up = (vec3){0.0, 1.0, 0.0};
    vec3 side_vec = ScalarMult(Normalize(CrossProduct(up, drive_dir)), ROAD_CHECK_WIDTH);
    float left_height = terrain->get_interpol_height(new_pos.x + side_vec.x, new_pos.z + side_vec.z, 0);
    float right_height = terrain->get_interpol_height(new_pos.x - side_vec.x, new_pos.z - side_vec.z, 0);
    float new_height = std::max(left_height, right_height);

    new_pos.y = new_height + 0.2;
    car.set_position(new_pos);
}

// Checks if we are close to goal.
bool Driver::goal_reached()
{
    vec3 car_to_goal = VectorSub(goal, car.get_position());
    if (DotProduct(drive_dir, car_to_goal) <= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
    
}
