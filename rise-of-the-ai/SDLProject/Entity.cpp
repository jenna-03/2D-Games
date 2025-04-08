/**
* Author: Jenna Nandlall
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include <cmath>





void Entity::ai_activate(Entity *player)
{
    switch (m_ai_type)
    {
        case WALKER:
            ai_walk();
            break;
            
        case GUARD:
            ai_guard(player);
            break;
            
        case FLYER:
            ai_fly();
            break;
            
        default:
            break;
    }
}

void Entity::ai_walk()
{
    if (glm::length(m_patrol_point) == 0.0f) {
        m_patrol_point = m_position;
    }
    
    
    float distance_from_start = m_position.x - m_patrol_point.x;
    
    
    if (distance_from_start < -m_patrol_range) {
        m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
        set_facing_left(false);
    } else if (distance_from_start > m_patrol_range) {
        m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
        set_facing_left(true);
    } else if (glm::length(m_movement) == 0.0f) {
        m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
        set_facing_left(true);
    }
}


void Entity::ai_guard(Entity *player)
{
    switch (m_ai_state) {
        case IDLE:
            if (glm::distance(m_position, player->get_position()) < 3.0f) m_ai_state = WALKING;
            break;
            
        case WALKING:
            if (m_position.x > player->get_position().x) {
                m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
                set_facing_left(true);
            } else {
                m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
                set_facing_left(false);
            }
            break;
            
        case ATTACKING:
            break;
            
        default:
            break;
    }
}


void Entity::ai_fly() {
    
    float patrol_range = 1.0f;

    
    if (fabs(m_movement.y) < 0.01f) {
        m_movement.y = 1.0f; // start moving upward
    }
    
    
    if (m_position.y > m_patrol_point.y + patrol_range) {
        m_movement.y = -fabs(m_movement.y); // move downward
    } else if (m_position.y < m_patrol_point.y - patrol_range) {
        m_movement.y = fabs(m_movement.y);  // move upward
    }
    
    
    m_movement.x = 0.0f;
    
    
    m_velocity.y = m_movement.y * m_speed;
    
    
    m_animation_indices = m_walking[0];
}


void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index) {
    float u = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v = (float)(index / m_animation_cols) / (float)m_animation_rows;
    
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;
    
    float texCoords[] = {
        u, v + height,
        u + width, v + height,
        u + width, v,
        u, v + height,
        u + width, v,
        u, v
    };
    
    float vertices[] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f
    };
    
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const {
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);
    
    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count) {
    for (int i = 0; i < collidable_entity_count; i++) {
        Entity* collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity)) {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));
            
            if (m_velocity.y > 0) {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            } else if (m_velocity.y < 0) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count) {
    for (int i = 0; i < collidable_entity_count; i++) {
        Entity* collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity)) {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            
            if (m_velocity.x > 0) {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
            } else if (m_velocity.x < 0) {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
            }
        }
    }
}

void const Entity::check_collision_y(Map* map) {
    
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0) {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    
    else if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0) {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
}

void const Entity::check_collision_x(Map* map) {
    
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);
    
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    
    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0) {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    
    else if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0) {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

void Entity::update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map)
{
    if (!m_is_active) return;
 
    m_collided_top    = false;
    m_collided_bottom = false;
    m_collided_left   = false;
    m_collided_right  = false;
    
    
    if (m_entity_type == ENEMY) {
        ai_activate(player);
    }
    
    // Animation update
    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float) 1 / SECONDS_PER_FRAME;
            
            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }
    
   
    if (m_movement.x < 0) {
        set_facing_left(true);
        face_left();
    } else if (m_movement.x > 0) {
        set_facing_left(false);
        face_right();
    }
    
    
    m_velocity.x = m_movement.x * m_speed;
    
    
    if (m_ai_type == FLYER && m_ai_state == FLYING) {
        m_velocity.y = m_movement.y * m_speed;
    } else {
        
        m_velocity += m_acceleration * delta_time;
    }
    
    if (m_is_jumping)
    {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }
    
    
    m_position.y += m_velocity.y * delta_time;
    
    // Skip collision for flying enemies
    if (m_ai_type != FLYER) {
        check_collision_y(collidable_entities, collidable_entity_count);
        check_collision_y(map);
    }
    
    m_position.x += m_velocity.x * delta_time;
    
    
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);
    
    
    if (m_collided_left) {
        m_movement.x = 1.0f;
        set_facing_left(false);
    } else if (m_collided_right) {
        m_movement.x = -1.0f;
        set_facing_left(true);
    }
    
    
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, glm::vec3(m_width, m_height, 3.0f));
}

void Entity::render(ShaderProgram* program) {
    if (!m_is_active) return;
    
    program->set_model_matrix(m_model_matrix);
    
    if (m_animation_indices != NULL) {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
    } else {
        float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
        float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
        
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        
        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    }
}


Entity::Entity() {}

Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, int walking[4][6], float animation_time,
        int animation_frames, int animation_index, int animation_cols,
           int animation_rows, float width, float height, EntityType entity_type) {
    
    m_texture_id = texture_id;
    m_speed = speed;
    m_acceleration = acceleration;
    m_jumping_power = jump_power;
    
    
    set_walking(walking);
    m_animation_time = animation_time;
    m_animation_frames = animation_frames;
    m_animation_index = animation_index;
    m_animation_cols = animation_cols;
    m_animation_rows = animation_rows;
    
    m_animation_indices = m_walking[RIGHT];
    
    m_width = width;
    m_height = height;
    
    m_entity_type = entity_type;
    
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_movement = glm::vec3(0.0f);
    m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
    
    m_model_matrix = glm::mat4(1.0f);
    m_is_jumping = false;
}

Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType entity_type) {
    m_texture_id = texture_id;
    m_speed = speed;
    m_width = width;
    m_height = height;
    m_entity_type = entity_type;
    
    m_acceleration = glm::vec3(0.0f);
    m_jumping_power = 0.0f;
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_movement = glm::vec3(0.0f);
    m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
    
    m_animation_time = 0.0f;
    m_animation_frames = 1;
    m_animation_index = 0;
    m_animation_cols = 1;
    m_animation_rows = 1;
    m_animation_indices = nullptr;
    
    m_model_matrix = glm::mat4(1.0f);
    m_is_jumping = false;
    
    m_ai_type = WALKER;
    m_ai_state = IDLE;
}

Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType entity_type, AIType ai_type, AIState ai_state) {
    m_texture_id = texture_id;
    m_speed = speed;
    m_width = width;
    m_height = height;
    m_entity_type = entity_type;
    m_ai_type = ai_type;
    m_ai_state = ai_state;
    
    m_acceleration = glm::vec3(0.0f);
    m_jumping_power = 0.0f;
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_movement = glm::vec3(0.0f);
    m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
    
    m_animation_time = 0.0f;
    m_animation_frames = 1;
    m_animation_index = 0;
    m_animation_cols = 1;
    m_animation_rows = 1;
    m_animation_indices = nullptr;
    
    m_model_matrix = glm::mat4(1.0f);
    m_is_jumping = false;
}

Entity::~Entity() {
}
