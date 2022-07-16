#include "Generator.h"

#include "LoadTGA.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <stdlib.h> 
#include <iostream> 
using namespace std;

Generator::Generator()
{

}


Generator::Generator(int size, int num_octaves, float smooth_factor, int seed)
    : size{size}, num_octaves{num_octaves}, smoothness{smooth_factor}, seed{seed}
{

}

float Generator::generate_height(int x, int z)
{
    float total_height = 0;
    float start_frequency = 1/8.0f;
    for(int o=0;o<num_octaves;o++)
    {
        // for every octave: increase the frequency and lower the amplitude
        // result: terrain with mostly low-frequency parts but also some high-frequency components
        float frequency = (float)pow(2,o)*start_frequency;
        float amplitude = (float)pow((1-smoothness),o);
        total_height += generate_interpolated_noise(x*frequency,z*frequency)*amplitude;
    }
    return total_height;
}

float Generator::generate_noise(int x, int z)
{
    srand(x+z*size+seed);    //sets the seed
    return ((float)rand() / (float)RAND_MAX);   //returns a random number between 0 and 1
}

float Generator::generate_smooth_noise(int x, int z)
{
    float center = generate_noise(x,z)/4.0f; //influence most
    float neighbouring_sides = (generate_noise(x+1,z) + generate_noise(x-1,z) + generate_noise(x,z+1) + generate_noise(x,z-1))/8.0f;
    float neighbouring_corners = (generate_noise(x+1,z+1) + generate_noise(x-1,z-1) + generate_noise(x+1,z-1) + generate_noise(x-1,z+1))/16.0f; //influence least

    return center + neighbouring_sides + neighbouring_corners;
}

float Generator::interpolate(float p1, float p2, float blend)
{
    double angle = blend*M_PI;
    float new_blend = (1.0f-(float)cos(angle))*0.5;  //value between 0 and 1
    return p1*(1.0f-new_blend) + p2*new_blend; 
}

float Generator::generate_interpolated_noise(float x, float z)
{
    int int_x = (int)x; //get the integer-part
    int int_z = (int)z;
    float fraction_x = x - int_x; //get fraction-part
    float fraction_z = z - int_z;

    float corner_upleft = generate_smooth_noise(int_x,int_z);
    float corner_upright = generate_smooth_noise(int_x+1,int_z);
    float corner_downleft = generate_smooth_noise(int_x,int_z+1);
    float corner_downright = generate_smooth_noise(int_x+1,int_z+1);

    float horizontal1 = interpolate(corner_upleft,corner_upright,fraction_x);
    float horizontal2 = interpolate(corner_downleft,corner_downright,fraction_x);

    return interpolate(horizontal1,horizontal2,fraction_z);
}

void Generator::create_heightmap(int size)
{
    unsigned char *value_array = (unsigned char*)malloc(sizeof(unsigned char)*size*size*3);
    int x,z;

    for (x = 0; x < size; x++)
		for (z = 0; z < size; z++)
		{
            float height = generate_height(x,z);
            unsigned char height_value = height * 255;

            value_array[(x + z * size)*3 + 0] = height_value;
			value_array[(x + z * size)*3 + 1] = height_value;
		    value_array[(x + z * size)*3 + 2] = height_value;
		}

    std::string file_name = "res/textures/heightmap.tga";
    SaveDataToTGA((char*)file_name.c_str(), size, size, 8*3, value_array);
}