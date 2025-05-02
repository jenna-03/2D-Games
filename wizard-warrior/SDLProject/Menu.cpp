#include "Menu.h"
#include "Utility.h"
#include <SDL_mixer.h>

// Constructor
Menu::Menu() {
    m_game_state.next_scene_id = -1;
    m_game_state.level_completed = false;
    m_game_state.game_over = false;
    
    m_game_start = false;
    
    m_background_texture_id = 0;
}

// Destructor
Menu::~Menu() {
    if (m_game_state.bgm != nullptr) {
        Mix_FreeMusic(m_game_state.bgm);
        m_game_state.bgm = nullptr;
    }
}

void Menu::initialise() {
    m_font_texture_id = Utility::load_texture("assets/font1.png");
    m_background_texture_id = Utility::load_texture("assets/menu.png");
    m_game_start = false;
}

void Menu::update(float delta_time) {
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_RETURN2]) {
        m_game_start = true;
    }
}

void Menu::render(ShaderProgram *program) {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 7.5f, 1.0f));
    program->set_model_matrix(model_matrix);
    
    // Draw the background texture
    float vertices[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};
    float texture_coordinates[] = {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    
    glBindTexture(GL_TEXTURE_2D, m_background_texture_id);
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    // Reset model matrix for text
    model_matrix = glm::mat4(1.0f);
    program->set_model_matrix(model_matrix);
    
    program->set_tint_color(1.0f, 1.0f, 1.0f, 1.0f); 
    Utility::draw_text(program, m_font_texture_id, "WIZARD WARRIOR",
                      0.6f, 0.0f, glm::vec3(-4.0f, 1.0f, 0.0f));
    
    Utility::draw_text(program, m_font_texture_id, "PRESS ENTER TO START",
                      0.4f, 0.0f, glm::vec3(-3.9f, 0.0f, 0.0f));
}
