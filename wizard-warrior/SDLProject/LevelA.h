#include "Scene.h"

class LevelA : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 1;
    
    GLuint m_background_texture_id;

    int m_number_of_enemies = 0;
    
    // ————— DESTRUCTOR ————— //
    ~LevelA();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
