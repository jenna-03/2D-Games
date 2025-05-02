#include "Effects.h"
#include <iostream>

constexpr float DEFAULT_LIGHT_INTENSITY = 1.0f;
constexpr float DEFAULT_LIGHT_RADIUS = 3.0f;

Effects::Effects() :
    m_light_intensity(DEFAULT_LIGHT_INTENSITY),
    m_light_radius(DEFAULT_LIGHT_RADIUS),
    m_light_position(0.0f, 0.0f),
    m_lighting_enabled(true),
    m_active_program(nullptr)
{
}

Effects::~Effects() {}

void Effects::initialize(const char* vertex_shader_path, const char* fragment_shader_path)
{
    m_default_program.load(vertex_shader_path, fragment_shader_path);
    m_default_program.set_effects(this);
    
    m_active_program = &m_default_program;
}

void Effects::initialize_lighting(const char* vertex_lit_path, const char* fragment_lit_path)
{
    m_lighting_program.load(vertex_lit_path, fragment_lit_path);
    m_lighting_program.set_effects(this);
    
    m_lighting_program.set_light_position(m_light_position);
    m_lighting_program.set_light_intensity(m_light_intensity);
    m_lighting_program.set_light_radius(m_light_radius);
    
    if (m_lighting_enabled) {
        m_active_program = &m_lighting_program;
    }
}

void Effects::toggle_lighting()
{
    m_lighting_enabled = !m_lighting_enabled;
    
    if (m_lighting_enabled) {
        m_active_program = &m_lighting_program;
        std::cout << "Lighting effect enabled" << std::endl;
    } else {
        m_active_program = &m_default_program;
        std::cout << "Lighting effect disabled" << std::endl;
    }
}

ShaderProgram& Effects::get_default_program()
{
    return m_default_program;
}

void Effects::update_light_position(const glm::vec3& position)
{
    m_light_position = glm::vec2(position.x, position.y);
    
    if (m_lighting_enabled) {
        m_lighting_program.set_light_position(m_light_position);
    }
}

void Effects::set_light_intensity(float intensity)
{
    m_light_intensity = intensity;
    if (m_lighting_enabled) {
        m_lighting_program.set_light_intensity(m_light_intensity);
    }
}

void Effects::set_light_radius(float radius)
{
    m_light_radius = radius;
    if (m_lighting_enabled) {
        m_lighting_program.set_light_radius(m_light_radius);
    }
}

void Effects::adjust_light_intensity(float amount)
{
    m_light_intensity += amount;
    
    if (m_light_intensity < 0.1f) m_light_intensity = 0.1f;
    if (m_light_intensity > 5.0f) m_light_intensity = 5.0f;
    
    if (m_lighting_enabled) {
        m_lighting_program.set_light_intensity(m_light_intensity);
        std::cout << "Light intensity: " << m_light_intensity << std::endl;
    }
}

void Effects::adjust_light_radius(float amount)
{
    m_light_radius += amount;
    
    if (m_light_radius < 0.5f) m_light_radius = 0.5f;
    if (m_light_radius > 10.0f) m_light_radius = 10.0f;
    
    if (m_lighting_enabled) {
        m_lighting_program.set_light_radius(m_light_radius);
        std::cout << "Light radius: " << m_light_radius << std::endl;
    }
}

ShaderProgram* Effects::get_active_program()
{
    return m_active_program;
}

void Effects::apply_view_matrix(const glm::mat4& view_matrix)
{
    m_default_program.set_view_matrix(view_matrix);
    m_lighting_program.set_view_matrix(view_matrix);
}

void Effects::apply_projection_matrix(const glm::mat4& projection_matrix)
{
    m_default_program.set_projection_matrix(projection_matrix);
    m_lighting_program.set_projection_matrix(projection_matrix);
}
