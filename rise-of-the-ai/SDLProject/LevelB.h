#pragma once
#include "Scene.h"

class LevelB : public Scene {
public:
    // ————— MEMBER VARIABLES ————— //
    int m_enemy_count;
    
    float m_last_injury_time;
    const float INVINCIBILITY_TIME = 5.0f;
    
    
    int m_number_of_enemies;
    
    
    bool m_player_hurt;
    float m_hurt_animation_time;
    int m_blink_count;
    const float BLINK_DURATION = 0.2f;
    const int NUM_BLINKS = 3;
    
    
    int m_normal_walking[4][6];
    int m_hurt_walking[4][6];
    
    
    GLuint m_normal_texture_id;
    GLuint m_hurt_texture_id;
    GLuint m_background_texture_id;
        float m_background_width;
        float m_background_height;
    
    
    bool m_has_size_getters;
    bool m_level_complete;
    
    // ————— DESTRUCTOR ————— //
    ~LevelB();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    
    // Helper methods for sprite switching
    void switch_to_hurt_sprite();
    void switch_to_normal_sprite();
};
