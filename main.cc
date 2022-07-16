#include "MicroGlut.h"

#include <memory>
#include <string>
#include <map>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION

#include "State.h"
#include "Game_State.h"

using namespace std;

void render();
void update();
void check_input();
void change_state();

map<string, unique_ptr<State>> all_states {};
State* current_state;

bool running {true};

int window_width {720};
int window_height {720};

float milli_sec_per_loop {20};

void init()
{
    all_states["game"] = make_unique<Game_State>();
    
    current_state = all_states["game"].get();
}

void display()
{
    update();

    render();

    check_input();

    change_state();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitContextVersion(3, 2);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow ("CDIO");
    glutDisplayFunc(display); 
    glutRepeatingTimer(milli_sec_per_loop);

    init();
    glutMainLoop();
    exit(0);

    return 0;
}

void render()
{
    current_state->render();
}

void update()
{
    float delta_time {milli_sec_per_loop / 1000.0f};
    current_state->update(delta_time);
}

void check_input()
{
    current_state->check_input();
}

void change_state()
{
    //change state if needed
    string state_flag {current_state->update_state()};

    if (state_flag == "exit")
    {
        glutExit();
    }

    auto it {all_states.find(state_flag)};
    if (it != all_states.end())
    {
	current_state = it->second.get();
    }
}