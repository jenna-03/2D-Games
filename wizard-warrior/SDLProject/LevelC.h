#pragma once
#include "Scene.h"

class LevelC : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 4;
    
    GLuint m_background_texture_id;
    
    Entity* m_moving_platforms;
    int m_platform_count;
        
    ShaderProgram* m_current_program = nullptr;
    
    // ————— DESTRUCTOR ————— //
    ~LevelC();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    
    void update_camera(float delta_time);
    void update_light_position();
    void check_platform_collisions(); 
    
private:
    struct MovingPlatform {
        glm::vec3 position;
        float     left_x,
                  right_x,
                  speed;
        int       tile_idx;
    };
    std::vector<MovingPlatform> m_platforms;

    void drawMovingPlatforms(ShaderProgram *program);
};

