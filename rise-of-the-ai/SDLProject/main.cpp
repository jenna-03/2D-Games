/**
* Author: Jenna Nandlall
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/




#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 42
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Menu.h"  // Added to include the Menu scene

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640 * 2,
          WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl",
           FONT_FILEPATH[] = "assets/font1.png";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

// ————— ENUMS ————— //
enum LevelState { MENU, LEVEL_A, LEVEL_B, LEVEL_C, GAME_OVER };

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene *g_current_scene;
Menu  *g_menu;     // Added Menu pointer
LevelA *g_level_a;
LevelB *g_level_b;
LevelC *g_level_c;
LevelState g_current_level = MENU;
bool g_transitioning_level = false;

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

// Font texture for UI elements
GLuint g_font_texture_id;

void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();

void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Rise of the AI",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    

    g_font_texture_id = Utility::load_texture(FONT_FILEPATH);
    
        
        // ————— SCENE SETUP ————— //
        g_menu = new Menu();
        g_level_a = new LevelA();
        g_level_b = new LevelB();
        

        switch_to_scene(g_menu);
        g_current_level = MENU;
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    
    GameState current_state = g_current_scene->get_state();
    if (current_state.game_over && g_current_level != LEVEL_B) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                event.type == SDL_WINDOWEVENT_CLOSE ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)) {
                g_app_status = TERMINATED;
            }
        }
        return;
    }
    
        if (g_current_scene->get_state().player) {
            g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
        }
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                case SDL_WINDOWEVENT_CLOSE:
                    g_app_status = TERMINATED;
                    break;
                    
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_q:
                            g_app_status = TERMINATED;
                            break;
                        case SDLK_SPACE:
                            // In level scenes, space triggers jump.
                            if (g_current_scene->get_state().player && g_current_scene->get_state().player->get_collided_bottom()) {
                                g_current_scene->get_state().player->jump();
                                Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
        

        const Uint8 *key_state = SDL_GetKeyboardState(NULL);
        if (g_current_scene->get_state().player) {
            if (key_state[SDL_SCANCODE_LEFT])
                g_current_scene->get_state().player->move_left();
            else if (key_state[SDL_SCANCODE_RIGHT])
                g_current_scene->get_state().player->move_right();
             
            if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
                g_current_scene->get_state().player->normalise_movement();
        }
    }

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }
    g_accumulator = delta_time;
        
    // --- Check for menu transition ---
    if (g_current_level == MENU) {
        Menu* menuScene = static_cast<Menu*>(g_current_scene);
        if (menuScene->shouldStartGame()) {
            switch_to_scene(g_level_a);
            g_current_level = LEVEL_A;
        }
    }
    
    // ————— PLAYER CAMERA ————— //
    GameState current_state = g_current_scene->get_state();
    g_view_matrix = glm::mat4(1.0f);
    
    if (current_state.game_over == false && g_current_level != MENU) {
        if (g_current_level == LEVEL_A &&
            current_state.player->get_position().y < -10.0f &&
            !g_transitioning_level)
        {
            g_transitioning_level = true;
            int current_lives = current_state.lives;
            g_level_b->set_lives(current_lives);
            switch_to_scene(g_level_b);
            g_current_level = LEVEL_B;
            g_transitioning_level = false;
        }
        
        // Level-specific camera behavior
        if (current_state.player->get_position().x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-current_state.player->get_position().x, 3.75, 0));
        } else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        }
    }
    else {
        // Fixed camera when game is over or in the menu
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    }
    
    
    if (g_current_level == LEVEL_B &&
        g_current_scene->get_state().game_over &&
        g_current_scene->get_state().lives > 0 &&
        !g_transitioning_level)
    {
        g_transitioning_level = true;
        int current_lives = g_current_scene->get_state().lives;
        g_level_c = new LevelC();
        g_level_c->set_lives(current_lives); // Pass these lives to Level C
        switch_to_scene(g_level_c);
        g_current_level = LEVEL_C;
        g_transitioning_level = false;
    }
}


void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
   
    g_current_scene->render(&g_shader_program);
    
    
    if (g_current_scene != g_menu)
    {
       
        glm::mat4 original_view_matrix = g_view_matrix;
        g_shader_program.set_view_matrix(glm::mat4(1.0f));
        
        // Draw lives.
        std::string lives_text = "LIVES: " + std::to_string(g_current_scene->get_state().lives);
        Utility::draw_text(&g_shader_program, g_font_texture_id, lives_text, 0.4f, 0.01f, glm::vec3(-4.5f, 3.25f, 0.0f));
        
        
        std::string level_text;
        if (g_current_level == LEVEL_A)
            level_text = "LEVEL: 1";
        else if (g_current_level == LEVEL_B)
            level_text = "LEVEL: 2";
        else if (g_current_level == LEVEL_C)
            level_text = "LEVEL: 3";
        Utility::draw_text(&g_shader_program, g_font_texture_id, level_text, 0.4f, 0.01f, glm::vec3(1.5f, 3.25f, 0.0f));
        
        // Draw game over or win messages if the game is over.
        if (g_current_scene->get_state().game_over)
        {
            if (g_current_scene->get_state().lives <= 0)
            {
                Utility::draw_text(&g_shader_program, g_font_texture_id, "GAME OVER", 0.5f, 0.05f, glm::vec3(-2.0f, 0.0f, 0.0f));
            }
            else if (g_current_level == LEVEL_C)
            {
                Utility::draw_text(&g_shader_program, g_font_texture_id, "YOU WIN!", 0.5f, 0.05f, glm::vec3(-1.5f, 0.0f, 0.0f));

            }
        }
        
        // Restore the original view matrix.
        g_shader_program.set_view_matrix(original_view_matrix);
    }
    
    SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
    SDL_Quit();
    delete g_menu;
    delete g_level_a;
    delete g_level_b;
}

int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}

