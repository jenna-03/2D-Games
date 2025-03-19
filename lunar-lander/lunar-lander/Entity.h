/**
* Author: Jenna Nandlall
* Assignment: Lunar Lander
* Date due: 2025-3-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#ifndef ENTITY_H
#define ENTITY_H

#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include <vector>
#pragma once

class Entity
{
private:
    // ————— PHYSICS AND TRANSFORMATIONS ————— //
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    glm::vec3 m_scale;
    glm::mat4 m_model_matrix;
    float m_rotation_angle; 
    
    
    // ————— TEXTURES ————— //
    std::vector<GLuint> m_texture_ids; 
    
    float m_fuel = 100.0f;
    
public:
    // collision state
    bool is_landed = false;
    bool is_crashed = false;
    
    // flag for collision detection
    bool win = false;
    
    // ————— CONSTRUCTORS ————— //
    Entity();
    Entity(std::vector<GLuint> texture_ids, float speed);
    ~Entity();
    
    // ————— METHODS ————— //
    void update(float delta_time);
    void render(ShaderProgram* program);
    
    // collision methods
    bool check_collision_with_platform(Entity* platform);
    bool check_collision_with_boundaries(float min_x, float max_x, float min_y, float max_y);
    
    // getters and Setters
    glm::vec3 const get_position() const { return m_position; }
    glm::vec3 const get_velocity() const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_scale() const { return m_scale; }
    float const get_speed() const { return m_speed; }
    bool const get_crashed() const { return is_crashed; }
    bool const get_landed() const { return is_landed; }
    float const get_fuel() const { return m_fuel; }
    float const get_rotation_angle() const { return m_rotation_angle; } // New getter for rotation
    
    void set_position(glm::vec3 new_position) { m_position = new_position; }
    void set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void set_speed(float new_speed) { m_speed = new_speed; }
    void set_fuel(float new_fuel) { m_fuel = new_fuel; }
    void set_rotation(float angle) { m_rotation_angle = angle; } // New setter for rotation
    void rotate(float degrees) { m_rotation_angle += degrees; } // New method to rotate by specified amount

private:
    float m_speed;
};

#endif // ENTITY_H
