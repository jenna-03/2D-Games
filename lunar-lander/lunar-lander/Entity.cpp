/**
* Author: Jenna Nandlall
* Assignment: Lunar Lander
* Date due: 2025-3-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_GLEXT_PROTOTYPES 1
#define GL_SILENCE_DEPRECATION
#include "Entity.h"
#include "glm/gtc/matrix_transform.hpp"

Entity::Entity()
    : m_position(0.0f), m_velocity(0.0f), m_acceleration(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
      m_rotation_angle(0.0f), m_speed(0.0f), m_fuel(100.0f)
{
}

Entity::Entity(std::vector<GLuint> texture_ids, float speed)
    : m_position(0.0f), m_velocity(0.0f), m_acceleration(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
      m_rotation_angle(0.0f), m_speed(speed), m_texture_ids(texture_ids), m_fuel(100.0f)
{
}

Entity::~Entity() { }

void Entity::update(float delta_time)
{
    m_velocity += m_acceleration * delta_time;
    
    m_position += m_velocity * delta_time;
    
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f));
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}

void Entity::render(ShaderProgram* program)
{
    
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f)); 
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
    
    program->set_model_matrix(m_model_matrix);
    
    if (!m_texture_ids.empty())
    {
        float vertices[] = {
            -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
        };
        
        float tex_coords[] = {
            0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
        };
        
        glBindTexture(GL_TEXTURE_2D, m_texture_ids[0]);
        
        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    }
}

bool Entity::check_collision_with_platform(Entity* platform)
{
    // trying to find boundaries of collision box
    float player_half_width = m_scale.x / 2.0f;
    float player_half_height = m_scale.y / 2.0f;
    float platform_half_width = platform->get_scale().x / 2.0f;
    float platform_half_height = platform->get_scale().y / 2.0f;
    float player_bottom = m_position.y - player_half_height;
    float platform_top = platform->get_position().y + platform_half_height;
    
    // calculate horizontal positions
    float player_left = m_position.x - player_half_width;
    float player_right = m_position.x + player_half_width;
    float platform_left = platform->get_position().x - platform_half_width;
    float platform_right = platform->get_position().x + platform_half_width;
    
    bool horizontal_overlap = !(player_right < platform_left || player_left > platform_right);
    
    // vertical distances
    float vertical_distance = player_bottom - platform_top;
    bool approaching_from_above = m_velocity.y <= 0;
    
    // calculate how much of the player is over the platform horizontally
    float overlap_percentage = 0.0f;
    if (horizontal_overlap) {
        float overlap_width = std::min(player_right, platform_right) - std::max(player_left, platform_left);
        overlap_percentage = overlap_width / (player_half_width * 2.0f);
    }
    
    if (horizontal_overlap && vertical_distance <= -0.3f && vertical_distance >= -0.4f &&
        approaching_from_above && overlap_percentage >= 0.5f) {
        
        m_position.y = platform_top + player_half_height - 0.3f;
        
        // stops movement
        m_velocity = glm::vec3(0.0f);
        
        is_landed = true;
        win = true;
        return true;
    }
    
    return false;
}

bool Entity::check_collision_with_boundaries(float min_x, float max_x, float min_y, float max_y)
{
    bool out_of_bounds =
        m_position.x < min_x ||
        m_position.x > max_x ||
        m_position.y < min_y ||
        m_position.y > max_y;
    
    if (out_of_bounds || m_position.y <= -3.0f) {
        is_crashed = true;
        return true;
    }
    
    return false;
}
