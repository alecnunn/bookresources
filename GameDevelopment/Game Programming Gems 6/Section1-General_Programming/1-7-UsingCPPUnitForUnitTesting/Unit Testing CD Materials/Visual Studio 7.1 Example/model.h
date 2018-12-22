//Model loading code
/*NOTE: THIS CODE IS VERY BUGGY AND IS ONLY MEANT FOR THE PURPOSE OF DEMONSTRATING
UNIT TESTING.  DO NOT ACTUALLY USE THIS IN PRODUCTION CODE.*/

#ifndef __MODEL_H__
#define __MODEL_H__

#include <cstring>
#include <cmath>

struct vertex_t {};
struct triangle_t {};
struct mesh_t {};
struct material_t {};
struct joint_t {};

class LoadModelTest;

class model_invalid_header {};
class model_invalid_version {};
class model_invalid_type {};

enum model_type
    {
    character,
    world_level
    };

class model
    {
public:
    void load_file(const char* file_path);
    void render();
    void animate(float speed, bool loop = true);
     friend LoadModelTest;
private:
    //Utility functions used to load model file
     void parse_header_section(char* file_text);
    void parse_triangle_section(char* file_text);
    void parse_mesh_section(char* file_text);
    void parse_material_section(char* file_text);
    void parse_animation_section(char* file_text);
    void prepare_joints();
    //Loads texture from file (BMP supported)
    void load_texture(const char* file_path);
    //File info functions
    double get_version() const
        { return m_version; }
    model_type get_type() const
        { return m_model_type; }
    const char* get_name() const
        { return m_name; }
    const char* get_author() const
        { return m_author; }
    //Data information functions
    size_t get_number_of_vertices() const
        { return m_number_of_vertices; }
    size_t get_number_of_triangles() const
        { return m_number_of_triangles; }
    size_t get_number_of_meshes() const
        { return m_number_of_materials; }
    size_t get_number_of_materials() const
        { return m_number_of_materials; }
    size_t get_number_of_joints() const
        { return m_number_of_joints; }
private:
    //Private data
    double m_version;
    model_type m_model_type;
    char* m_name;
    char* m_author;
    unsigned short m_number_of_vertices;
    unsigned short m_number_of_triangles;
    unsigned short m_number_of_meshes;
    unsigned short m_number_of_materials;
    unsigned short m_number_of_joints;
    vertex_t* m_vertices;
    triangle_t* m_triangles;
    mesh_t* m_meshes;
    material_t* m_materials;
    joint_t* m_joints;
    };

void model::parse_header_section(char* file_text)
     {
     if (!file_text)
          { return; }
     if (std::strstr(file_text, "<HEADER>") == NULL)
          { throw model_invalid_header(); }
     char* version = std::strstr(file_text, "<VERSION>");
     if (!version)
          { throw model_invalid_version(); }
     m_version = std::atof(version+9);
     char* type = std::strstr(file_text, "<TYPE>");
     if (!type)
          { throw model_invalid_version(); }
     if (std::strcmp(type+6, "WorldLevel"))
          m_model_type = world_level;
     else
          m_model_type = character;
     //author
     char* author = std::strstr(file_text, "<AUTHOR>");
     if (author)
          {
          author += 7;
          char* end = std::strstr(author, "</AUTHOR>");
          m_author = new char[(end-author)+1];
          std::memset(m_author, 0, (end-author)+1);
          std::strncpy(m_author, author, end-author);
          }
     char* name = std::strstr(file_text, "<NAME>");
     if (name)
          {
          name += 6;
          char* end = std::strstr(name, "</NAME>");
          m_name = new char[(end-name)+1];
          std::memset(m_name, 0, (end-name)+1);
          std::strncpy(m_name, name, end-name);
          }
     }

#endif //__MODEL_H__
