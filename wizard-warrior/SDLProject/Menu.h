#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Scene.h"

class Menu : public Scene {
private:
    GLuint m_font_texture_id;
    GLuint m_background_texture_id;
    
    bool m_game_start;
    
public:
    // Constructor and destructor
    Menu();
    ~Menu();
    
    // Scene methods
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    
    // Getters
    bool is_game_start() const { return m_game_start; }
};
