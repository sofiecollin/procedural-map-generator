#include "Object.h"

#include <iostream>

#include "OBJ_Loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/gl.h>

Object::Object()
	: Object {"low_poly_car", vec3 {0,0,0}}
{
}

Object::Object(std::string const& object_name)
	: Object {object_name, vec3 {0, 0, 0}}
{
}

Object::Object(std::string const& object_name, vec3 const& pos)
	: position {pos}
{
	load_model(object_name);
}

Object::Object(std::vector<GLfloat> const& vertexArray, std::vector<GLfloat> const& normalArray, 
           	   	std::vector<GLfloat> const& texCoordArray, std::vector<GLuint> const& indexArray,
				std::string const& texture_name)
	: vertexArray {vertexArray}, normalArray {normalArray}, texCoordArray {texCoordArray}, indexArray {indexArray}
{
	model_data.load_buffer_data(vertexArray, normalArray, texCoordArray, indexArray);
    model_data.material.texture_id = load_material_texture("res/textures/" + texture_name);
    model_data.material.use_specularity_map = false;
    model_data.material.ka = vec3{0.8, 0.8, 0.8};
    model_data.material.kd = vec3{0.7, 0.7, 0.7};
    model_data.material.ks = vec3{0.2, 0.2, 0.2};
    model_data.material.a = 1.0;

}

Object::Object(Object const& rhs)
{
	*this = rhs;
}

Object& Object::operator=(Object const& rhs)
{
	position = rhs.position;
	rotation = rhs.rotation;
	object_scale = rhs.object_scale;

	vertexArray = rhs.vertexArray;
	normalArray = rhs.normalArray;
	texCoordArray = rhs.texCoordArray;
	indexArray = rhs.indexArray;

	model_data = rhs.model_data;

	return *this;
}

Object::~Object()
{
}

void Object::render(GLuint shader_program) const
{
	glBindVertexArray(model_data.vao);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model_data.material.texture_id);
	if (model_data.material.use_specularity_map)
	{
		glActiveTexture(GL_TEXTURE1);
    	glBindTexture(GL_TEXTURE_2D, model_data.material.specularity_map_id);
	}

	glDrawElements(GL_TRIANGLES, model_data.indices_count, GL_UNSIGNED_INT, 0L);
}

mat4 Object::get_model_matrix() const
{
    mat4 t = T(position.x, position.y, position.z);
    mat4 s = S(object_scale.x, object_scale.y, object_scale.z);

    return (t * rotation * s);
}

unsigned Object::load_material_texture(std::string const& map_path) const
{
	unsigned texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(map_path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
	if (data)
	    {
		if (nrChannels == 3)
		    {
				data = stbi_load(map_path.c_str(), &width, &height, &nrChannels, STBI_rgb);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		    } else if (nrChannels == 4)
		    {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		    }
		glGenerateMipmap(GL_TEXTURE_2D);
	    } else
	    {
		std::cout << "Failed to load texture:" << map_path <<  std::endl;
	    }
	stbi_image_free(data);

	return texture_id;
}

void Object::set_position(vec3 const& new_pos)
{
	position = new_pos;
}

void Object::set_rotation(mat4 const& new_rotation)
{
	rotation = new_rotation;
}

void Object::set_scale(vec3 const& scale)
{
	object_scale = scale;
}

void Object::move(vec3 const& vector)
{
	set_position(VectorAdd(position, vector));
}

void Object::rotate(mat4 const& matrix)
{
	set_rotation(Mult(matrix, rotation));
}

void Object::scale(vec3 const& vector)
{
	set_scale(VectorAdd(object_scale, vector));
}

void Object::set_material(vec3 const& ka, vec3 const& kd, vec3 const& ks, GLfloat a, GLuint texture_id, GLuint specularity_map_id, bool use_spec_map)
{
	model_data.material.ka = ka;
	model_data.material.kd = kd;
	model_data.material.ks = ks;
	model_data.material.a = a;
	model_data.material.texture_id = texture_id;
	model_data.material.specularity_map_id = specularity_map_id;
	model_data.material.use_specularity_map = use_spec_map;
}

// ====================
// ===| Model_Data |===
// ====================

void Object::load_model(std::string const& file_name)
{
    Model_Data current_model;

    auto it = models.find(file_name);
    if ( it != models.end())
    {
        current_model = it->second;
    } else
    {
        current_model = load_model_from_file(file_name);
        models[file_name] = current_model;
    }

    model_data = current_model;
}

Object::Model_Data Object::load_model_from_file(std::string const& file_name) const
{
	objl::Loader obj_loader {};
    
    if (obj_loader.LoadFile("res/objects/" + file_name + "/" + file_name + ".obj"))
    {
        std::vector<GLfloat> vertices {};
        std::vector<GLfloat> normals {};
        std::vector<GLuint> indices {};
        std::vector<GLfloat> texture_coords {};

	    for (auto it = obj_loader.LoadedVertices.begin(); it != obj_loader.LoadedVertices.end(); it++)
        {
            vertices.push_back(it->Position.X);
            vertices.push_back(it->Position.Y);
            vertices.push_back(it->Position.Z);
            normals.push_back(it->Normal.X);
            normals.push_back(it->Normal.Y);
            normals.push_back(it->Normal.Z);
			texture_coords.push_back(it->TextureCoordinate.X);
            texture_coords.push_back(it->TextureCoordinate.Y);
        }

        for (auto it = obj_loader.LoadedIndices.begin(); it != obj_loader.LoadedIndices.end(); it++)
        {
            indices.push_back(*it);
        }

        objl::Material mat = obj_loader.LoadedMaterials[0];

        bool use_specularity_map {false};
        unsigned int specularity_map {};
        unsigned int kd_texture {load_material_texture("res/objects/" + file_name + "/" + mat.map_Kd)};
		if (mat.map_Ks != "")
        {
            specularity_map = load_material_texture("res/objects/" + file_name + "/" + mat.map_Ks);
            use_specularity_map = true;
        }
        Material material {kd_texture, specularity_map, use_specularity_map, mat.Ka, mat.Kd, mat.Ks, mat.Ni};
        Model_Data model_data {};
        model_data.material = material;
        model_data.load_buffer_data(vertices, normals, texture_coords, indices);
        return model_data;

    } else
    {
        throw std::runtime_error("Cound not find model: " + file_name);
    }
}

void Object::Model_Data::load_buffer_data(std::vector<GLfloat> const& vertices,
                                         std::vector<GLfloat> const& normals,
                                         std::vector<GLfloat> const& texture_coords,
                                         std::vector<GLuint> const& indices)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vb);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(IN_POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);

    glGenBuffers(1, &nb);
    glBindBuffer(GL_ARRAY_BUFFER, nb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(IN_NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);

    glGenBuffers(1, &tb);
    glBindBuffer(GL_ARRAY_BUFFER, tb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texture_coords.size(), &texture_coords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(IN_TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);

    glGenBuffers(1, &ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * indices.size(), &indices[0], GL_STATIC_DRAW);
    indices_count = indices.size();

    glEnableVertexAttribArray(IN_POSITION_LOCATION);
    glEnableVertexAttribArray(IN_NORMAL_LOCATION);
    glEnableVertexAttribArray(IN_TEX_COORD_LOCATION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}