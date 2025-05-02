#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include <vector>
#include <map>

enum EntityType { PLATFORM, PLAYER, ENEMY, FIRE_EFFECT };
enum AIType { WALKER, GUARD, PATROL };
enum AIState { WALKING, IDLE, ATTACKING,};

enum AnimationState {
    ATTACK1 = 0,
    ATTACK2 = 1,
    DEATH = 2,
    ATTACK3 = 3,
    HURT = 4,
    STANDING = 5,
    JUMPING = 6,
    RUN = 7,
    FLY = 8
};

// Animation class to manage animation frames and states
class Animation {
private:
    std::vector<int> m_frames;  // animation frame indices
    int m_cols;                 // columns in spritesheet
    GLuint m_texture_id;        // texture ID
    int m_current_index;        // current frame index in animation
    float m_time;               // time for animation
    
public:
    // Constructor
    Animation() : m_cols(0), m_texture_id(0), m_current_index(0), m_time(0.0f) {}
    
    Animation(const std::vector<int>& frames, int cols, GLuint texture_id)
        : m_frames(frames), m_cols(cols), m_texture_id(texture_id), m_current_index(0), m_time(0.0f) {}
    
    // Methods
    void update(float delta_time, float seconds_per_frame) {
        if (m_frames.empty()) return;
        
        m_time += delta_time;
        
        // Calculate frame duration
        float frame_duration = seconds_per_frame;
        
        if (m_time >= frame_duration) {
            m_current_index++;
            m_time -= frame_duration;
            

            if (m_current_index >= m_frames.size()) {
                m_current_index = 0;
            }
        }
    }
    bool is_finished() const {
        return m_current_index >= m_frames.size() - 1;
    }
    
    void reset() {
        m_current_index = 0;
        m_time = 0.0f;
    }
    
    // Getters
    int get_current_frame() const {
        if (m_frames.empty()) return 0;
        return m_frames[m_current_index];
    }
    
    GLuint get_texture_id() const {
        return m_texture_id;
    }
    
    int get_cols() const {
        return m_cols;
    }
    
    int get_frame_count() const {
        return m_frames.size();
    }
    
    // For comparing animations
    bool operator==(const Animation& other) const {
        return m_texture_id == other.m_texture_id && m_frames == other.m_frames;
    }
    
    bool operator!=(const Animation& other) const {
        return !(*this == other);
    }
};

class Entity
{
private:
    bool m_is_active = true;
    
    std::map<AnimationState, Animation> m_animations;
    
    AnimationState m_current_animation_state = STANDING;
    
    EntityType m_entity_type;
    AIType     m_ai_type;
    AIState    m_ai_state;
    
    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_movement;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    glm::mat4 m_model_matrix;

    float     m_speed,
              m_jumping_power;
    
    bool m_is_jumping;

    // ————— ANIMATION ————— //
    float m_animation_time = 0.0f;

    float m_width = 1.0f,
          m_height = 1.0f;
    
    // ————— COLLISIONS ————— //
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;
    
    bool m_animation_changed = false;
    
    bool m_is_attacking = false;
    float m_attack_timer = 0.0f;
    
    
    int m_lives = 5;
    bool m_is_hurt = false;
    float m_hurt_timer = 0.0f;
    float m_invincibility_timer = 0.0f;
    bool m_is_invincible = false;
    bool m_is_respawning = false;
    float m_respawn_timer = 0.0f;
    glm::vec3 m_respawn_position;
    
    
    bool m_is_dying = false;
    float m_death_timer = 0.0f;
    
    float m_blink_timer = 0.0f;
    bool m_is_visible = true;
    float m_blink_rate = 0.1f;
    
public:
    // ————— STATIC VARIABLES ————— //
    static constexpr float SECONDS_PER_FRAME = 1.0f/8.0f;

    // ————— METHODS ————— //
    Entity();
    
    // Constructor with standard parameters
    Entity(EntityType entityType, float speed, glm::vec3 acceleration, float jump_power,
           float width, float height);
    
    // Simpler constructor for non-animated entities
    Entity(GLuint texture_id, float speed, float width, float height, EntityType entityType);
    
    // AI constructor
    Entity(GLuint texture_id, float speed, float width, float height, EntityType entityType, AIType aiType, AIState aiState);
    
    ~Entity();

    // Add an animation to this entity
    void add_animation(AnimationState state, const std::vector<int>& frames, int cols, GLuint texture_id);
    
    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;
    
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
    
    // Overloading our methods to check for only the map
    void const check_collision_y(Map *map);
    void const check_collision_x(Map *map);
    
    void update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map);
    void render(ShaderProgram* program);

    void ai_activate(Entity *player);
    void ai_walk();
    void ai_guard(Entity *player);
    void ai_patrol();
    
    
    void normalise_movement() { m_movement = glm::normalize(m_movement); }
    void constrain_to_map(int map_width, int map_height);

    // Animation state management
    void set_animation_state(AnimationState state);
    bool has_animation(AnimationState state) const;
   
    // Movement controls
    void move_left();
    void move_right();
    void move_up() { m_movement.y = 1.0f; }
    void move_down() { m_movement.y = -1.0f; }
    
    void const jump() { m_is_jumping = true; set_animation_state(JUMPING); }
    void attack();
    void attack2();
    void attack3();
    void take_damage(int damage);
    void respawn();
    bool check_attack_collision(Entity* other);
    void check_for_fall(int map_height);
    bool is_defeated() const;
    void update_model_matrix();
    
    // ————— GETTERS ————— //
    EntityType const get_entity_type()    const { return m_entity_type;   };
    AIType     const get_ai_type()        const { return m_ai_type;       };
    AIState    const get_ai_state()       const { return m_ai_state;      };
    float const get_jumping_power() const { return m_jumping_power; }
    AnimationState const get_current_animation_state() const { return m_current_animation_state; }
    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_movement()     const { return m_movement; }
    glm::vec3 const get_scale()        const { return m_scale; }
    GLuint    const get_texture_id()   const {
        auto it = m_animations.find(m_current_animation_state);
        if (it != m_animations.end()) {
            return it->second.get_texture_id();
        }
        return 0;
    }
    float     const get_speed()        const { return m_speed; }
    bool      const get_collided_top() const { return m_collided_top; }
    bool      const get_collided_bottom() const { return m_collided_bottom; }
    bool      const get_collided_right() const { return m_collided_right; }
    bool      const get_collided_left() const { return m_collided_left; }
    int             get_lives() const { return m_lives; }
    
    float const get_width()  const { return m_width; }
    float const get_height() const { return m_height; }
    
    bool is_attacking() const { return m_is_attacking; }
    bool is_entity_type(EntityType type) const { return m_entity_type == type; }
    bool is_hurt() const { return m_is_hurt; }
    bool is_dying() const { return m_is_dying; }
    bool is_invincible() const { return m_is_invincible; }
    bool is_respawning() const { return m_is_respawning; }
    bool is_active() const { return m_is_active; }
    
    void activate()   { m_is_active = true;  };
    void deactivate() { m_is_active = false; };
    
    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;};
    void const set_ai_type(AIType new_ai_type){ m_ai_type = new_ai_type;};
    void const set_ai_state(AIState new_state){ m_ai_state = new_state;};
    void const set_position(glm::vec3 new_position) { m_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    void const set_jumping_power(float new_jumping_power) { m_jumping_power = new_jumping_power;}
    void const set_width(float new_width) {m_width = new_width; }
    void const set_height(float new_height) {m_height = new_height; }
    void set_lives(int lives) { m_lives = lives; }
    void set_respawn_position(glm::vec3 position) { m_respawn_position = position; }
    void const set_collided_bottom_true() { m_collided_bottom = true; }
};

#endif // ENTITY_H
