#ifndef HEIGHTS_GENERATOR_H
#define HEIGHTS_GENERATOR_H

class Heights_Generator
{
public:
    Heights_Generator();
    Heights_Generator(int size, int num_octaves, float smooth_factor, int seed);
    float generate_height(int x, int z);
    void create_heightmap(int size);

private:
    int size;           //size of the heightmap
    int num_octaves;    //number of noise functions of different density, more octaves means higher level of detail
    float smoothness;   //between 0-1
    int seed;           //for the random part, (same seed=same output)

    float generate_noise(int x, int z);
    float generate_smooth_noise(int x, int z);
    float interpolate(float p1, float p2, float blend);
    float generate_interpolated_noise(float x, float z);

};


#endif