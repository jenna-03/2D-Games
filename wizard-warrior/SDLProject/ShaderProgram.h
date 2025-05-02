#pragma once

#ifdef _WINDOWS
    #include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "glm/mat4x4.hpp"

class Effects;

class ShaderProgram
{
private:
    void cleanup();
    
    GLuint load_shader_from_string(const std::string &shader_contents, GLenum shader_type);
    GLuint load_shader_from_file(const std::string &shader_file, GLenum shader_type);

    GLuint m_program_id;

    GLuint m_projection_matrix_uniform;
    GLuint m_model_matrix_uniform;
    GLuint m_view_matrix_uniform;
    GLuint m_colour_uniform;
    GLuint m_tint_colour_uniform;
    
    // Light-related uniforms
    GLuint m_light_position_uniform;
    GLuint m_light_intensity_uniform;
    GLuint m_light_radius_uniform;

    GLuint m_position_attribute;
    GLuint m_tex_coord_attribute;

    GLuint m_vertex_shader;
    GLuint m_fragment_shader;
    
    Effects* m_effects;
    
public:
    ShaderProgram() : m_effects(nullptr) {}

    void load(const char *vertex_shader_file, const char *fragment_shader_file);

    void set_model_matrix(const glm::mat4 &matrix);
    void set_projection_matrix(const glm::mat4 &matrix);
    void set_view_matrix(const glm::mat4 &matrix);
    void set_colour(float red, float green, float blue, float alpha);
    void set_tint_color(float red, float green, float blue, float alpha = 1.0f);
    
    // Light-related methods
    void set_light_position(glm::vec2 position);
    void set_light_intensity(float intensity);
    void set_light_radius(float radius);
    
    
    // Effects manager setter/getter
    void set_effects(Effects* effects) { m_effects = effects; }
    Effects* get_effects() const { return m_effects; }
    
    GLuint const get_program_id()               const { return m_program_id;          };
    GLuint const get_position_attribute()       const { return m_position_attribute;  };
    GLuint const get_tex_coordinate_attribute() const { return m_tex_coord_attribute; };
    
    void set_program_id(GLuint program_id)                         { m_program_id = program_id;                   };
};
