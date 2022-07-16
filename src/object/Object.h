#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <map>

#include "VectorUtils3.h"
#include "LoadTGA.h"

class Object
{
public:
    Object();
    Object(std::string const& object_name);
    Object(std::string const& object_name, vec3 const& position);
    Object(std::vector<GLfloat> const& vertexArray, std::vector<GLfloat> const& normalArray, 
           std::vector<GLfloat> const& texCoordArray, std::vector<GLuint> const& indexArray,
           std::string const& texture_name);
    Object(Object const& rhs);
    Object& operator=(Object const& rhs);
    ~Object();

    void update(float delta_time) {}
    void render(GLuint shader_program) const;
    unsigned load_material_texture(std::string const& map_path) const;

    //getters
    vec3 get_position() const { return position; }
    mat4 get_rotation() const { return rotation; }
    vec3 get_scale() const { return object_scale; }

    //setters
    void set_position(vec3 const& new_pos);
    void set_rotation(mat4 const& new_rotation);
    void set_scale(vec3 const& new_scale);
    void move(vec3 const& vector);
    void rotate(mat4 const& matrix);
    void scale(vec3 const& vector);

    mat4 get_model_matrix() const;

    uint get_texture() const
    {
        return model_data.material.texture_id;
    }

protected:

    vec3 position;
    vec3 object_scale {1, 1, 1};
    mat4 rotation {IdentityMatrix()};
    
    std::vector<GLfloat> vertexArray {};
    std::vector<GLfloat> normalArray {};
    std::vector<GLfloat> texCoordArray {};
    std::vector<GLuint> indexArray {};

private:

    struct Material
    {
        GLuint texture_id {};
        GLuint specularity_map_id {};
        bool use_specularity_map {};
        vec3 ka {};
        vec3 kd {};
        vec3 ks {};
        GLfloat a {};
    };

    struct Model_Data
    {
        void load_buffer_data(std::vector<GLfloat> const&, std::vector<GLfloat> const&, std::vector<GLfloat> const&, std::vector<GLuint> const&);
        GLuint vao {};
        GLuint vb {}, nb {}, tb {}, ib {};
        GLuint indices_count {};
        Material material {};
    };
    
protected:
    Model_Data model_data;

//tmp public
public:
    Material get_material() const { return model_data.material; }
    void set_material(vec3 const& ka, vec3 const& kd, vec3 const& ks, GLfloat a, GLuint texture_id, GLuint speculatory_ma_id, bool use_spec_map);

private:

    //For model handler
    inline static std::map<std::string, Model_Data> models {};
    void load_model(std::string const& file_name);
    Model_Data load_model_from_file(std::string const& file_name) const;

    const static int IN_POSITION_LOCATION {0};
    const static int IN_NORMAL_LOCATION {1};
    const static int IN_TEX_COORD_LOCATION {2};
};

#endif
