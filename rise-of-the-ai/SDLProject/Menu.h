#pragma once

#include "Scene.h"
#include "ShaderProgram.h"
#include <SDL_opengl.h>

class Menu : public Scene {
public:
    Menu();
    ~Menu();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

    
    bool shouldStartGame() const { return m_startGame; }

private:
    GLuint font_texture_id;
    bool m_startGame;        
    GLuint m_background_texture_id;
        float m_background_width;
        float m_background_height;
    
};

