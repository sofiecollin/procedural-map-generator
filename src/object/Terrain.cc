#include "Terrain.h"

using namespace std;

Terrain::Terrain()
{
    
}

Terrain::Terrain(float max_height, float width_param, std::string const& height_map_name)
	: max_height{max_height}, width_param{width_param}, use_heightmap{true}
{
	LoadTGATextureData(height_map_name.c_str(), &heightmap_texdata);
	LoadTGATextureData("res/textures/watermap_default.tga", &watermap_texdata);
	resolution_x = heightmap_texdata.width;
	resolution_z = heightmap_texdata.height;
	generate_terrain();

	model_data.load_buffer_data(vertexArray, normalArray, texCoordArray, indexArray);
}

Terrain::Terrain(float max_height, float width_param, std::string const& height_map_name, std::string const& water_map_name)
	: max_height{max_height}, width_param{width_param}, use_heightmap{true}
{
	LoadTGATextureData(height_map_name.c_str(), &heightmap_texdata);
	LoadTGATextureData(water_map_name.c_str(), &watermap_texdata);
	resolution_x = heightmap_texdata.width;
	resolution_z = heightmap_texdata.height;
	generate_terrain();

	model_data.load_buffer_data(vertexArray, normalArray, texCoordArray, indexArray);
}

Terrain::Terrain(int size, float max_height, float width_param, int num_octaves, float smoothness, int seed)
	: resolution_x{size}, resolution_z{size}, max_height{max_height}, width_param{width_param}
{
    LoadTGATextureData("res/textures/watermap_default.tga", &watermap_texdata);
    generator = Heights_Generator(size, num_octaves, smoothness, seed);
    generate_terrain();

	model_data.load_buffer_data(vertexArray, normalArray, texCoordArray, indexArray);
}

Terrain::Terrain(int size, float max_height, float width_param, int num_octaves, float smoothness, int seed, std::string const& water_map_name)
	: resolution_x{size}, resolution_z{size}, max_height{max_height}, width_param{width_param}
{
    LoadTGATextureData(water_map_name.c_str(), &watermap_texdata);
    generator = Heights_Generator(size, num_octaves, smoothness, seed);
    generate_terrain();

	model_data.load_buffer_data(vertexArray, normalArray, texCoordArray, indexArray);
}


void Terrain::render(GLuint shader_program) const
{
	glBindVertexArray(model_data.vao);

	for(GLuint i=0;i<texture_ids.size();i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
    	glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
	}


	if (model_data.material.use_specularity_map)
	{
		glActiveTexture(GL_TEXTURE1);
    	glBindTexture(GL_TEXTURE_2D, model_data.material.specularity_map_id);
	}

	glDrawElements(GL_TRIANGLES, model_data.indices_count, GL_UNSIGNED_INT, 0L);
}

float Terrain::get_height(int x, int z)
{
    return terrain_heights[std::make_pair((int)x/width_param, (int)z/width_param)];
}

float Terrain::get_width_param()
{
    return width_param;
}

float Terrain::get_max_height()
{
    return max_height;
}

float Terrain::get_interpol_height(float x, float z, float y) const
{
	x /= width_param;
	z /= width_param;
	float yret;
	int xfloor = floor(x);
	int zfloor = floor(z);
	float xfloor_float = floor(x);
	float zfloor_float = floor(z);
	
	if (xfloor < 0 || zfloor < 0 || xfloor >= size_x - 1 || zfloor >= size_z - 1)
	{
		yret = y;
	}
	else
	{
		float c1y, c2y, c3y;
		float xdist = x - xfloor_float;
		float xsdist = 1 - xdist;
		float zdist = z - zfloor_float;
		float zsdist = 1-zdist;
		bool triangle1 = 0;
		if (xdist < zsdist)
		{
			c3y = terrain_heights.at(std::make_pair(xfloor, zfloor));
			triangle1 = 1;
		}
		else
		{
			c3y = terrain_heights.at(std::make_pair(xfloor + 1, zfloor + 1));
		}
		c1y = terrain_heights.at(std::make_pair(xfloor + 1, zfloor ));
		c2y = terrain_heights.at(std::make_pair(xfloor, zfloor + 1));
		if (triangle1) 
		{
			yret = c3y+xdist*(c1y-c3y)+zdist*(c2y-c3y);
		}
		else 
		{
			yret = c3y+xsdist*(c2y-c3y)+zsdist*(c1y - c3y);
		}
	}
	return yret;
}

float Terrain::get_interpol_height(vec3 pos)
{
	return get_interpol_height(pos.x, pos.z, pos.y);
}

vec3 Terrain::get_normal(int x, int z)
{
	// Calculated using the finite difference method	
	float left_neighbour_height = terrain_heights[std::make_pair(x-1, z)];;
	float right_neighbour_height = terrain_heights[std::make_pair(x+1, z)];;
	float up_neighbour_height = terrain_heights[std::make_pair(x, z-1)];;
	float down_neighbour_height = terrain_heights[std::make_pair(x, z+1)];;

	vec3 normal = vec3(left_neighbour_height-right_neighbour_height, 2.0 * width_param, up_neighbour_height-down_neighbour_height);
	vec3 normalized_normal = Normalize(normal);

	return normalized_normal;
}

int Terrain::get_watermap_value(int x, int z)
{
	x = (int)x / width_param;
	z = (int)z / width_param;
	if (x < (int)watermap_texdata.width && x >= 0 && z < (int)watermap_texdata.height && z >= 0)
	{
    	return (int) (watermap_texdata.imageData[(x + z * resolution_x) * (watermap_texdata.bpp/8)]/255.0f);
	}
	else
	{
		return 0;
	}	
}

// Returns the incline between point a and b in degrees
float Terrain::get_incline(vec3 a, vec3 b)
{
	float length = Norm(b - a);
	float height_diff = abs(get_interpol_height(b) - get_interpol_height(a));
	return asin(height_diff / length) * 180 / M_PI;
}

bool Terrain::inside_terrain(vec3 pos)
{
	bool within_x = (int) (pos.x / width_param) < resolution_x && (int) (pos.x / width_param) > 0;
	bool within_z = (int) (pos.z / width_param) < resolution_z && (int) (pos.z / width_param) > 0;
	return within_x && within_z;
}

void Terrain::set_material(vec3 const& ka, vec3 const& kd, vec3 const& ks, GLfloat a, std::vector<std::string> texture_paths, GLuint specularity_map_id, bool use_spec_map)
{
	model_data.material.ka = ka;
	model_data.material.kd = kd;
	model_data.material.ks = ks;
	model_data.material.a = a;
	model_data.material.specularity_map_id = specularity_map_id;
	model_data.material.use_specularity_map = use_spec_map;

	for(GLuint i=0; i<texture_paths.size(); i++)
	{
		std::string path = texture_paths[i];
		GLuint tex_id = load_material_texture(path);
		texture_ids.push_back(tex_id);
	}

	model_data.material.texture_id = texture_ids[0];

}

void Terrain::generate_terrain()
{
	// Ingemars function from lab4, slightly modified
	int numVertices =  resolution_x * resolution_z;
	int numIndices = (resolution_x - 1) * (resolution_z - 1) * 2 * 3;
	int x, z;
	float height;
	vertexArray = std::vector<GLfloat> (numVertices * 3);
	texCoordArray = std::vector<GLfloat> (3 * numVertices);
	normalArray = std::vector<GLfloat> (numVertices * 3);
	indexArray = std::vector<GLuint> (numIndices);
	

    for (x = 0; x < resolution_x; x++)
		for (z = 0; z < resolution_z; z++)
		{
			if (use_heightmap) 
			{
			  	float raw_height = heightmap_texdata.imageData[(x + z * resolution_x) * (heightmap_texdata.bpp/8)]/255.0f;
			  	height = raw_height * max_height*2 - max_height;
			}
			else 
			{
				float raw_height = generator.generate_height(x ,z);
				height = raw_height * max_height*2 - max_height;
			}

			// If water, use different terrain height
			float raw_water = watermap_texdata.imageData[(x + z * resolution_x) * (watermap_texdata.bpp/8)]/255.0f;
			if(raw_water == 1.0)
			{
				vertexArray[(x + z * resolution_x)*3 + 0] = x * width_param;
				vertexArray[(x + z * resolution_x)*3 + 1] = -max_height/2;
				vertexArray[(x + z * resolution_x)*3 + 2] = z  * width_param;
			}
			else
			{
				vertexArray[(x + z * resolution_x)*3 + 0] = x * width_param;
				vertexArray[(x + z * resolution_x)*3 + 1] = height;
				vertexArray[(x + z * resolution_x)*3 + 2] = z  * width_param;
			}

			terrain_heights[std::make_pair(x, z)] = height;

			texCoordArray[(x + z * resolution_x)*2 + 0] = x; 
			texCoordArray[(x + z * resolution_x)*2 + 1] = z; 
		}

	for (x = 0; x < resolution_x; x++)
		for (z = 0; z < resolution_z; z++)
		{
			vec3 normal = get_normal(x,z);
			normalArray[(x + z * resolution_x)*3 + 0] = normal.x;
			normalArray[(x + z * resolution_x)*3 + 1] = normal.y;	
			normalArray[(x + z * resolution_x)*3 + 2] = normal.z;
		}

	for (x = 0; x < resolution_x-1; x++)
	{
		for (z = 0; z < resolution_z-1; z++)
		{
			indexArray[(x + z * (resolution_x-1))*6 + 0] = x + z * resolution_x;
			indexArray[(x + z * (resolution_x-1))*6 + 1] = x + (z+1) * resolution_x;
			indexArray[(x + z * (resolution_x-1))*6 + 2] = x+1 + z * resolution_x;
			
			indexArray[(x + z * (resolution_x-1))*6 + 3] = x+1 + z * resolution_x;
			indexArray[(x + z * (resolution_x-1))*6 + 4] = x + (z+1) * resolution_x;
			indexArray[(x + z * (resolution_x-1))*6 + 5] = x+1 + (z+1) * resolution_x;
		}
	}
}