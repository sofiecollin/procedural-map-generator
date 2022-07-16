#ifndef TERRAIN_H
#define TERRAIN_H

#include <map>
#include <utility>

#include "Object.h"
#include "Heights_Generator.h"

class Terrain : public Object
{
public:
    Terrain(); // Flat terrain
    Terrain(float max_height, float width_param, std::string const& height_map_name); // From height_map
    Terrain(float max_height, float width_param, std::string const& height_map_name, std::string const& water_map_name);
    Terrain(int size, float max_height, float width_param, int num_octaves, float smoothness, int seed); // From generated noise 
    Terrain(int size, float max_height, float width_param, int num_octaves, float smoothness, int seed, std::string const& water_map_name);

    void render(GLuint shader_program) const;

    // Gives the height on a specific point in the terrain.
    // Useful for placing objects on the correct height.
    float get_height(int x, int z);
    float get_interpol_height(float x, float z, float y) const;
    float get_interpol_height(vec3 pos);
    vec3 get_normal(int x, int z);
    int get_watermap_value(int x, int y);
    float get_incline(vec3 a, vec3 b);

    float get_width_param();
    float get_max_height();

    bool inside_terrain(vec3 pos);
    std::vector<GLuint> get_texture_ids() const { return texture_ids; }
    void set_material(vec3 const& ka, vec3 const& kd, vec3 const& ks, GLfloat a, std::vector<std::string> texture_paths, GLuint specularity_map_id = 16, bool use_spec_map=false);

private:

    // Size in the world
    float size_x {128};
    float size_z {128};

    // Size of the heightmap / nr of vertices
    int resolution_x;
    int resolution_z;

    Heights_Generator generator;
    float max_height;
    float width_param{1.0};
    bool use_heightmap = false;
    TextureData heightmap_texdata{};
    TextureData watermap_texdata{};
    
    std::vector<GLuint> texture_ids;

    void generate_terrain();

    std::map<std::pair<int, int>, float> terrain_heights {};

};

#endif
