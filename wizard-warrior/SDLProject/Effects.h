#pragma once

#include "ShaderProgram.h"
#include "glm/glm.hpp"
#include "Entity.h"

class Effects {
private:
    ShaderProgram m_lighting_program;
    ShaderProgram m_default_program;
    
    ShaderProgram* m_active_program;
    
    float m_light_intensity;
    float m_light_radius;
    glm::vec2 m_light_position;
    
    bool m_lighting_enabled;
    
public:
    Effects();
    ~Effects();
    
    void initialize(const char* vertex_shader_path, const char* fragment_shader_path);
    void initialize_lighting(const char* vertex_lit_path, const char* fragment_lit_path);
    
    void toggle_lighting();
    bool is_lighting_enabled() const { return m_lighting_enabled; }
    
    void update_light_position(const glm::vec3& position);
    void set_light_intensity(float intensity);
    void set_light_radius(float radius);
    
    void adjust_light_intensity(float amount);
    void adjust_light_radius(float amount);
    
    float get_light_intensity() const { return m_light_intensity; }
    float get_light_radius() const { return m_light_radius; }
    
    ShaderProgram* get_active_program();
    ShaderProgram& get_default_program();
    
    void apply_view_matrix(const glm::mat4& view_matrix);
    void apply_projection_matrix(const glm::mat4& projection_matrix);
};
