#ifndef GENERATOR_H
#define GENERATOR_H

class Generator
{
public:
    Generator() = default;
    Generator(int size, int num_octaves, float smooth_factor, int seed);
    float generate_height(int x, int z);
    void create_heightmap(int size);
    void set_size(int the_size) { size = the_size;};
    void set_num_octaves(int the_num_octaves) {num_octaves = the_num_octaves;};
    void set_smoothness(float the_smooth_factor) {smoothness = the_smooth_factor;};
    void set_seed(int the_seed) {seed = the_seed;};

protected:
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