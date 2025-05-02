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

void Entity::ai_walk()
{
    // For simple walkers, just move left
    m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
    m_scale.x = -1.0f; // Face the direction of movement
}

// Update the ai_activate method to handle smarter enemy movements
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
        
        case PATROL:
            ai_patrol();
            break;
            
        default:
            break;
    }
}


// Modify the ai_guard method in Entity.cpp to prevent jumping and handle platform edges

// Updated AI guard method with pauses between attacks

void Entity::ai_guard(Entity *player)
{
    // Define distance thresholds
    const float DETECTION_DISTANCE = 4.0f;
    const float IDLE_DISTANCE = 5.0f;
    const float ATTACK_DISTANCE = 0.8f;
    const float EDGE_DETECTION_DISTANCE = 0.5f; // Distance to check for platform edge
    
    // Calculate distance to player
    float distance = glm::distance(m_position, player->get_position());
    
    // Check if player has moved from starting position (to decide if enemy should activate)
    bool player_has_moved = fabs(player->get_velocity().x) > 0.1f ||
                           fabs(player->get_velocity().y) > 0.1f;

    // NEW: Track attack cooldown time
    static float attack_cooldown = 0.0f;
    static float pause_timer = 0.0f;
    static bool is_pausing = false;

    // Update timers
    if (attack_cooldown > 0.0f) {
        attack_cooldown -= 0.016f; // Approximately one frame at 60 FPS
    }
    
    if (is_pausing) {
        pause_timer -= 0.016f;
        if (pause_timer <= 0.0f) {
            is_pausing = false;
        }
    }
    
    switch (m_ai_state) {
        case IDLE:
            // Only start walking if player has moved AND is within detection range
            if (player_has_moved && distance < DETECTION_DISTANCE) {
                m_ai_state = WALKING;
                
                // Explicitly set RUN animation when transitioning to WALKING state
                if (has_animation(RUN)) {
                    set_animation_state(RUN);
                }
            } else {
                // Make sure we're in STANDING animation when IDLE and not doing anything
                if (m_current_animation_state != STANDING && has_animation(STANDING)) {
                    set_animation_state(STANDING);
                }
                // Stop any movement
                m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            break;
            
        case WALKING:
            // Return to IDLE if player moves too far away
            if (distance > IDLE_DISTANCE) {
                m_ai_state = IDLE;
                set_animation_state(STANDING);
                m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
                return;
            }
            
            // If in pause mode, don't move but keep facing player
            if (is_pausing) {
                m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
                
                // Keep facing the player
                if (m_position.x > player->get_position().x) {
                    m_scale.x = -1.0f; // Face left
                } else {
                    m_scale.x = 1.0f; // Face right
                }
                
                // Make sure we're in STANDING animation when pausing
                if (m_current_animation_state != STANDING && has_animation(STANDING)) {
                    set_animation_state(STANDING);
                }
                return;
            }
            
            // If player gets too close and attack cooldown is finished, switch to attacking
            if (distance < ATTACK_DISTANCE && attack_cooldown <= 0.0f) {
                m_ai_state = ATTACKING;
                
                // Randomly choose which attack to use
                int attack_type = rand() % 2; // Only choose between 0 and 1
                
                switch (attack_type) {
                    case 0:
                        if (has_animation(ATTACK1)) {
                            set_animation_state(ATTACK1);
                            m_is_attacking = true;
                            m_attack_timer = 0.0f;
                            m_animations[ATTACK1].reset();
                        }
                        break;
                    case 1:
                        if (has_animation(ATTACK2)) {
                            set_animation_state(ATTACK2);
                            m_is_attacking = true;
                            m_attack_timer = 0.0f;
                            m_animations[ATTACK2].reset();
                        }
                        break;
                }
            }
            // Otherwise, move towards player
            else {
                // Get direction to player
                float dir_x = (player->get_position().x > m_position.x) ? 1.0f : -1.0f;
                
                // *** EDGE DETECTION ***
                // Check if we're about to walk off a platform
                bool edge_detected = false;
                
                // Only check for edges if we're on the ground
                if (m_collided_bottom) {
                    // Create a point slightly ahead in the direction we're moving
                    glm::vec3 edge_check_point = glm::vec3(
                        m_position.x + (dir_x * EDGE_DETECTION_DISTANCE),
                        m_position.y - (m_height / 2.0f) - 0.1f, // Slightly below feet
                        m_position.z
                    );
                    
                    // Check if there's ground beneath this point
                    // For simplicity, we assume no ground means an edge
                    float penetration_x = 0;
                    float penetration_y = 0;
                    
                    // Alternative: If we're moving right and there's no right collision OR
                    // if we're moving left and there's no left collision, it might be an edge
                    if ((dir_x > 0 && !m_collided_right) || (dir_x < 0 && !m_collided_left)) {
                        // Only consider it an edge if there's nothing directly below
                        edge_detected = !m_collided_bottom ||
                                       (m_position.x + dir_x > player->get_position().x + 1.0f) ||
                                       (m_position.x + dir_x < player->get_position().x - 1.0f);
                    }
                }
                
                // Only move if not at an edge or if player is very close
                if (!edge_detected || distance < 1.0f) {
                    // Set movement direction
                    if (m_position.x > player->get_position().x) {
                        m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
                        m_scale.x = -1.0f; // Face left when moving left
                    } else {
                        m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
                        m_scale.x = 1.0f; // Face right when moving right
                    }
                } else {
                    // If at an edge, stop and wait
                    m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
                    
                    // But keep facing the player
                    if (m_position.x > player->get_position().x) {
                        m_scale.x = -1.0f; // Face left
                    } else {
                        m_scale.x = 1.0f; // Face right
                    }
                }
                
                // *** ANIMATION MANAGEMENT ***
                if (!m_is_attacking) {
                    if (m_collided_bottom && fabs(m_movement.x) > 0.1f && has_animation(RUN)) {
                        // Use RUN when moving on ground
                        if (m_current_animation_state != RUN) {
                            set_animation_state(RUN);
                        }
                    } else if (m_collided_bottom && fabs(m_movement.x) < 0.1f && has_animation(STANDING)) {
                        // Use STANDING when not moving on ground
                        if (m_current_animation_state != STANDING) {
                            set_animation_state(STANDING);
                        }
                    }
                }
            }
            break;
            
        case ATTACKING:
            // If we're no longer attacking, switch back to walking state
            if (!m_is_attacking) {
                // Set attack cooldown and pause state
                attack_cooldown = 2.0f; // 2 seconds cooldown between attacks
                is_pausing = true;
                pause_timer = 1.5f; // Pause for 1.5 seconds after attacking
                
                m_ai_state = WALKING;
                
                // Explicitly set standing animation when pausing after attack
                if (m_collided_bottom && has_animation(STANDING)) {
                    set_animation_state(STANDING);
                }
                
                // Stop movement during pause
                m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            // If player moves away while we're attacking, face them
            else {
                // Make sure enemy faces the player during attack
                if (m_position.x > player->get_position().x) {
                    m_scale.x = -1.0f; // Face left
                } else {
                    m_scale.x = 1.0f; // Face right
                }
            }
            break;
            
        default:
            break;
    }
}

void Entity::ai_patrol() {
    // Keep current movement direction (already set in initialise)
    if (m_movement.x > 0) {
        m_scale.x = 1.0f;
    } else if (m_movement.x < 0) {
        m_scale.x = -1.0f;
    }
    // --- NEW: make sure flyers use the FLY animation ---
    if (has_animation(FLY) && m_current_animation_state != FLY) {
        set_animation_state(FLY);
    }
}
// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(0.0f), m_velocity(0.0f), m_acceleration(0.0f), m_jumping_power(0.0f),
    m_is_jumping(false), m_animation_time(0.0f), m_width(0.0f), m_height(0.0f),
    m_entity_type(PLATFORM), m_ai_type(WALKER), m_ai_state(IDLE), m_is_active(true)
{
}

// Main constructor with parameters
Entity::Entity(EntityType entityType, float speed, glm::vec3 acceleration, float jump_power,
               float width, float height)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_velocity(0.0f), m_acceleration(acceleration), m_jumping_power(jump_power),
    m_is_jumping(false), m_animation_time(0.0f), m_width(width), m_height(height),
    m_entity_type(entityType), m_ai_type(WALKER), m_ai_state(IDLE)
{
}

// Simpler constructor for non-animated entities
Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType entityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_velocity(0.0f), m_acceleration(0.0f), m_jumping_power(0.0f),
    m_is_jumping(false), m_animation_time(0.0f), m_width(width), m_height(height),
    m_entity_type(entityType), m_ai_type(WALKER), m_ai_state(IDLE)
{
    // Create a simple animation with just one frame
    std::vector<int> frames = {0};
    add_animation(STANDING, frames, 1, texture_id);
    set_animation_state(STANDING);
}

// AI constructor
Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType entityType, AIType aiType, AIState aiState)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_velocity(0.0f), m_acceleration(0.0f), m_jumping_power(0.0f),
    m_is_jumping(false), m_animation_time(0.0f), m_width(width), m_height(height),
    m_entity_type(entityType), m_ai_type(aiType), m_ai_state(aiState)
{
    // Create a simple animation with just one frame
    std::vector<int> frames = {0};
    add_animation(STANDING, frames, 1, texture_id);
    set_animation_state(STANDING);
}

Entity::~Entity() { }

void Entity::add_animation(AnimationState state, const std::vector<int>& frames, int cols, GLuint texture_id) {
    m_animations[state] = Animation(frames, cols, texture_id);
}

bool Entity::has_animation(AnimationState state) const {
    return m_animations.find(state) != m_animations.end();
}

void Entity::set_animation_state(AnimationState state) {
    // Only change if the state is different and we have the animation for it
    if (m_current_animation_state != state && has_animation(state)) {
        m_current_animation_state = state;
        // Always reset the animation when changing states
        auto it = m_animations.find(state);
        if (it != m_animations.end()) {
            it->second.reset();
        }
        m_animation_changed = true;
    }
}

// Updated movement methods to handle animations and flipping
void Entity::move_left()
{
    m_movement.x = -1.0f;
    m_scale.x = -1.0f; // Always face left when moving left
    
    // Only change animation if not attacking
    if (!m_is_attacking && has_animation(RUN)) {
        set_animation_state(RUN);
    }
}

void Entity::move_right()
{
    m_movement.x = 1.0f;
    m_scale.x = 1.0f; // Always face right when moving right
    
    // Only change animation if not attacking
    if (!m_is_attacking && has_animation(RUN)) {
        set_animation_state(RUN);
    }
}

void Entity::attack()
{
    // Only start attacking if we're not already attacking and have the animation
    if (!m_is_attacking && has_animation(ATTACK1)) {
        m_is_attacking = true;
        m_attack_timer = 0.0f;
        // Always reset the animation when starting a new attack
        m_animations[ATTACK1].reset();
        // Set animation state AFTER resetting the animation
        set_animation_state(ATTACK1);
    }
}

void Entity::attack2() {
    // Only start attacking if we're not already attacking and have the animation
    if (!m_is_attacking && has_animation(ATTACK2)) {
        m_is_attacking = true;
        m_attack_timer = 0.0f;
        // Always reset the animation when starting a new attack
        m_animations[ATTACK2].reset();
        // Set animation state AFTER resetting the animation
        set_animation_state(ATTACK2);
    }
}

void Entity::attack3() {
    // Only start attacking if we're not already attacking and have the animation
    if (!m_is_attacking && has_animation(ATTACK3)) {
        m_is_attacking = true;
        m_attack_timer = 0.0f;
        
        // Reset the animation and ensure it starts from the beginning
        auto it = m_animations.find(ATTACK3);
        if (it != m_animations.end()) {
            it->second.reset();
        }
        
        // Set animation state AFTER resetting the animation
        set_animation_state(ATTACK3);
        
        // For ATTACK3, we may need a longer duration due to more frames
        // This will be handled in the update method with the attack_duration calculation
    }
}
void Entity::take_damage(int damage)
{
    // Only take damage if not invincible and not already in a special state
    if (!m_is_invincible && !m_is_respawning && !m_is_dying)
    {
        m_lives -= damage;
        if (m_lives < 0) m_lives = 0;
        
        // Start invincibility timer
        m_is_invincible = true;
        m_invincibility_timer = 1.5f; // 1.5 seconds of invincibility
        
        // Reset blinking effect
        m_blink_timer = 0.0f;
        m_is_visible = true; // Start visible
        
        // If out of lives, trigger death animation
        if (m_lives <= 0 && has_animation(DEATH)) {
            set_animation_state(DEATH);
            m_is_dying = true;
            m_death_timer = 1.5f; // Duration to show death animation
        }
        // Otherwise trigger hurt animation, but NOT if respawning
        else if (!m_is_respawning && has_animation(HURT)) {
            set_animation_state(HURT);
            m_is_hurt = true;
            m_hurt_timer = 0.5f; // Duration of hurt animation
        }
    }
}
// Handle respawning after falling
void Entity::respawn()
{
    // Reset position to respawn point
    m_position = m_respawn_position;
    
    // Reset velocities
    m_velocity = glm::vec3(0.0f);
    m_movement = glm::vec3(0.0f);
    
    // End respawn state
    m_is_respawning = false;
    
    // Set invincibility to give a grace period
    m_is_invincible = true;
    m_invincibility_timer = 1.5f;
    
    // Reset blinking effect
    m_blink_timer = 0.0f;
    m_is_visible = true; // Start visible
    
    // Reset animation to standing
    set_animation_state(STANDING);
    
    // If it's an enemy, reset AI state
    if (m_entity_type == ENEMY)
    {
        m_ai_state = IDLE;
    }
}

bool Entity::is_defeated() const
{
    return m_lives <= 0;
}


bool Entity::check_attack_collision(Entity* other)
{

    if (!m_is_attacking || other->is_invincible() || other->is_respawning()) return false;
    
    float attack_duration = 0.0f;
    auto it = m_animations.find(m_current_animation_state);
    if (it != m_animations.end()) {
        attack_duration = SECONDS_PER_FRAME * it->second.get_frame_count();
    }
    
    float attack_progress = m_attack_timer / attack_duration;
    if (attack_progress < 0.3f || attack_progress > 0.7f) return false;
    
    float attack_range = 1.2f;
    float attack_dir = (m_scale.x > 0) ? 1.0f : -1.0f;
    
    float attack_x = m_position.x + (attack_dir * 0.6f);
    float attack_width = 0.8f;
    float attack_height = 0.9f;
    
    float other_x = other->get_position().x;
    float other_y = other->get_position().y;
    
    float x_distance = fabs(attack_x - other_x);
    float y_distance = fabs(m_position.y - other_y);
    
    bool x_overlap = x_distance < (attack_width + other->m_width) / 2.0f;
    bool y_overlap = y_distance < (attack_height + other->m_height) / 2.0f;
    
    if (x_overlap && y_overlap)
    {
        other->take_damage(1);
        return true;
    }
    
    return false;
}

void Entity::check_for_fall(int map_height)
{
   
    if (m_position.y < -5.0f && !m_is_respawning && !m_is_dying)
    {
        m_lives -= 1;
        if (m_lives < 0) m_lives = 0;
        
        if (m_lives <= 0 && has_animation(DEATH)) {
            set_animation_state(DEATH);
            m_is_dying = true;
            m_death_timer = 1.5f;
            return;
        }
        
       
        m_is_respawning = true;
        m_respawn_timer = 2.0f;
        
        m_velocity = glm::vec3(0.0f);
        m_movement = glm::vec3(0.0f);
        
        m_is_invincible = true;
        m_invincibility_timer = 1.5f;
        
        m_blink_timer = 0.0f;
        m_is_visible = true;
        
        m_position = glm::vec3(m_respawn_position.x, 10.0f, 0.0f);
        
    }
}







void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    auto it = m_animations.find(m_current_animation_state);
    if (it == m_animations.end()) return;
    
    int cols = it->second.get_cols();
    if (index < 0 || index >= cols) {
        index = 0;
    }
    
    float u_coord = (float)(index) / (float)cols;
    float v_coord = 0.0f;

    float width = 1.0f / (float)cols;
    float height = 1.0f;

    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
bool const Entity::check_collision(Entity* other) const
{
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity *collidable_entities, int collidable_entity_count)
{
    const float FIXED_DELTA = 0.0166666f;
    
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
    
        if (!collidable_entity->is_active()) continue;
        
        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));
            
            if (m_velocity.y < 0)
            {
                if (m_position.y > collidable_entity->m_position.y)
                {
                    m_position.y += y_overlap;
                    m_velocity.y = 0;
                    m_collided_bottom = true;
                    
                    if (collidable_entity->get_entity_type() == PLATFORM)
                    {
                        glm::vec3 platform_movement = collidable_entity->get_movement();
                        float platform_speed = collidable_entity->get_speed();
                        
                        if (fabs(platform_movement.x) > 0.001f) {
                            m_position.x += platform_movement.x * platform_speed * FIXED_DELTA;
                        }
                        
                        if (platform_movement.y > 0.001f) {
                            m_position.y += platform_movement.y * platform_speed * FIXED_DELTA;
                        }
                    }
                }
            }
            else if (m_velocity.y > 0)
            {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0)
            {
                m_position.x     -= x_overlap;
                m_velocity.x      = 0;
                m_collided_right  = true;
                
            } else if (m_velocity.x < 0)
            {
                m_position.x    += x_overlap;
                m_velocity.x     = 0;
 
                m_collided_left  = true;
            }
        }
    }
}

void const Entity::check_collision_y(Map *map)
{
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
            m_position.y += penetration_y;
            m_velocity.y = 0;
            m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
        
    }
}

void const Entity::check_collision_x(Map *map)
{
    glm::vec3 left  = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

void Entity::constrain_to_map(int map_width, int map_height) {
    if (m_position.x - (m_width / 2.0f) < 0) {
        m_position.x = m_width / 2.0f;
        m_velocity.x = 0;
    }
    
    if (m_position.x + (m_width / 2.0f) > map_width) {
        m_position.x = map_width - (m_width / 2.0f);
        m_velocity.x = 0;
    }
    
    if (m_position.y < -10.0f) {
        m_position.y = 5.0f;
        m_velocity.y = 0;
    }
}


void Entity::update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map)
{
    if (!m_is_active) return;
    
    if (m_is_dying) {
        m_death_timer -= delta_time;
        
        auto it = m_animations.find(DEATH);
        if (it != m_animations.end()) {
            it->second.update(delta_time, SECONDS_PER_FRAME);
        }
        
        if (m_death_timer <= 0.0f) {
            m_is_active = false;
            return;
        }
        
        return;
    }
    
    if (m_is_respawning) {
        m_respawn_timer -= delta_time;
        if (m_respawn_timer <= 0.0f) {
            respawn();
        }
        return;
    }
    
    if (m_is_invincible) {
        m_invincibility_timer -= delta_time;
        
        m_blink_timer -= delta_time;
        if (m_blink_timer <= 0.0f) {
            m_is_visible = !m_is_visible;
            m_blink_timer = m_blink_rate;
        }
        
        if (m_invincibility_timer <= 0.0f) {
            m_is_invincible = false;
            m_is_visible = true;
        }
    }
    
    if (m_is_hurt) {
        m_hurt_timer -= delta_time;
        
        auto it = m_animations.find(HURT);
        if (it != m_animations.end()) {
            it->second.update(delta_time, SECONDS_PER_FRAME);
        }
        
        if (m_hurt_timer <= 0.0f) {
            m_is_hurt = false;
            
            if (!m_collided_bottom && has_animation(JUMPING)) {
                set_animation_state(JUMPING);
            } else if (fabs(m_velocity.x) > 0.1f && has_animation(RUN)) {
                set_animation_state(RUN);
            } else if (has_animation(STANDING)) {
                set_animation_state(STANDING);
            }
        }
    }
    
    
    m_collided_top    = false;
    m_collided_bottom = false;
    m_collided_left   = false;
    m_collided_right  = false;
    
    if (m_entity_type == ENEMY && !m_is_hurt && !m_is_dying) ai_activate(player);
    m_velocity.x = m_movement.x * m_speed;
    
    if (!m_is_jumping) {
        m_velocity += m_acceleration * delta_time;
    }
    
    if (m_is_jumping && m_entity_type != ENEMY)
    {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }
    else if (m_is_jumping && m_entity_type == ENEMY)
    {
        m_is_jumping = false;

    }
    
    m_position.y += m_velocity.y * delta_time;
    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);
    
    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);
    
    if (m_is_attacking && !m_is_hurt && !m_is_dying) {
        m_attack_timer += delta_time;
        
        auto it = m_animations.find(m_current_animation_state);
        if (it != m_animations.end()) {
            Animation& anim = it->second;
            
            float frame_duration = SECONDS_PER_FRAME;
            
            if (m_current_animation_state == ATTACK3) {
                frame_duration = SECONDS_PER_FRAME * 2.0f;
            } else {
                frame_duration = SECONDS_PER_FRAME * 1.5f;
            }
            
            float attack_duration = frame_duration * anim.get_frame_count();
            
            if (m_attack_timer >= attack_duration) {
                m_is_attacking = false;
                
                if (m_entity_type == ENEMY) {
                }
                else {
                    if (!m_collided_bottom) {
                        set_animation_state(JUMPING);
                    } else if (fabs(m_velocity.x) > 0.1f) {
                        set_animation_state(RUN);
                    } else {
                        set_animation_state(STANDING);
                    }
                }
            }
            
            if (m_current_animation_state == ATTACK3) {
                anim.update(delta_time * 0.5f, SECONDS_PER_FRAME);
            }
            else {
                anim.update(delta_time, SECONDS_PER_FRAME);
            }
        }
    }
    else if (!m_is_hurt && !m_is_dying && !m_is_attacking) {
        const float MOVEMENT_THRESHOLD = 0.01f;
        
       
        if (m_entity_type != ENEMY) {
            if (m_collided_bottom) {
                if (fabs(m_velocity.x) < MOVEMENT_THRESHOLD) {
                    if (m_current_animation_state != STANDING && has_animation(STANDING)) {
                        set_animation_state(STANDING);
                    }
                } else {
                    if (m_current_animation_state != RUN && has_animation(RUN)) {
                        set_animation_state(RUN);
                    }
                }
            }
            
            else if (!m_collided_bottom && m_current_animation_state != JUMPING && has_animation(JUMPING)) {
                set_animation_state(JUMPING);
            }
        }
        
        auto it = m_animations.find(m_current_animation_state);
        if (it != m_animations.end()) {
            it->second.update(delta_time, SECONDS_PER_FRAME);
        }
    }
    
    if (m_entity_type != ENEMY) {
        m_movement.x = 0;
    }
    
    check_for_fall(map->get_height());
    
    if (m_collided_bottom) {
        float ground_level = m_position.y - (m_height / 2.0f);
        m_position.y = ground_level + (m_height / 2.0f);
    }
    
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
    
    m_animation_changed = false;
}
void Entity::render(ShaderProgram* program)
{
    if (!m_is_active) return;

    if (m_is_invincible && !m_is_visible) return;
    
    program->set_model_matrix(m_model_matrix);

    auto it = m_animations.find(m_current_animation_state);
    if (it != m_animations.end()) {
        const Animation& anim = it->second;
        
        int frame_index = anim.get_current_frame();
        GLuint texture_id = anim.get_texture_id();
        
        draw_sprite_from_texture_atlas(program, texture_id, frame_index);
    }
    else {

        float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
        float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

        GLuint texture_id = 0;
        if (!m_animations.empty()) {
            texture_id = m_animations.begin()->second.get_texture_id();
        }
        
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    }
}
