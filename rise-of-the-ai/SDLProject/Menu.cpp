/**
* Author: Jenna Nandlall
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#include "Menu.h"
#include "Utility.h"
#include <SDL.h>


constexpr char FONT_PATH[] = "assets/font1.png",
               BACKGROUND_FILEPATH[] = "assets/opening.png";


Menu::Menu() : m_startGame(false) {
    
    m_game_state.map = nullptr;
    m_game_state.player = nullptr;
    m_game_state.enemies = nullptr;
    m_game_state.bgm = nullptr;
    m_game_state.jump_sfx = nullptr;
    m_game_state.hurt_sfx = nullptr;
    m_game_state.win_sfx = nullptr;
}

Menu::~Menu() {
    
}

void Menu::initialise() {
    
    font_texture_id = Utility::load_texture(FONT_PATH);
    
   
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    m_background_texture_id = Utility::load_texture(BACKGROUND_FILEPATH);
    m_background_width = 70.0f;
    m_background_height = 12.0f;
    
    
    
    m_startGame = false;
}

void Menu::update(float delta_time) {
    
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_RETURN]) {
        m_startGame = true;
    }
}

void Menu::render(ShaderProgram* program) {
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
        glm::mat4 model_matrix = glm::mat4(1.0f);
        
        model_matrix = glm::translate(model_matrix, glm::vec3(21.0f, -3.0f, 0.0f));
        
        model_matrix = glm::scale(model_matrix, glm::vec3(70.0f, 12.0f, 1.0f)); 
        
        program->set_model_matrix(model_matrix);
        
       
        float vertices[] = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f, 0.5f,
            -0.5f, -0.5f,
            0.5f, 0.5f,
            -0.5f, 0.5f
        };
        
        float texCoords[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f
        };
        
        glBindTexture(GL_TEXTURE_2D, m_background_texture_id);
        
        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_tex_coordinate_attribute());

    
    Utility::draw_text(program, font_texture_id, "Rise of the AI", 0.5f, 0.02f, glm::vec3(1.5f, -3.5f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Press Enter to Start", 0.4f, 0.02f, glm::vec3(1.0f, -4.5f, 0.0f));
}

