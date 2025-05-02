/**
* Author: Jenna Nandlall
* Assignment: Wizard Warrior
* Date due: 5/3/25, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 18
#define LEVEL1_HEIGHT 8
#define LEVEL_C_WIDTH 45

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
#include "Menu.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Effects.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640 * 2,
          WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED     = 0.1f,
            BG_BLUE    = 0.1f,
            BG_GREEN   = 0.1f,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

// Shader paths
constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
constexpr char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
constexpr char V_LIT_SHADER_PATH[] = "shaders/vertex_lit.glsl";
constexpr char F_LIT_SHADER_PATH[] = "shaders/fragment_lit.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;


constexpr float INITIAL_ZOOM = 0.7f;
constexpr float GAMEPLAY_ZOOM = 1.0f;
constexpr float MAX_ZOOM = 0.6f;
constexpr float ZOOM_SPEED = 0.008f;
constexpr float CAMERA_LERP = 0.08f;


constexpr float DEFAULT_LIGHT_INTENSITY = 1.0f;
constexpr float DEFAULT_LIGHT_RADIUS = 3.0f;

enum AppStatus { RUNNING, TERMINATED };
enum GameStatus { MENU, PLAYING, GAME_OVER, GAME_WIN };

// ————— GLOBAL VARIABLES ————— //
GLuint g_font_texture_id;
Scene *g_current_scene;
Menu *g_menu;
LevelA *g_level_a;
LevelB *g_level_b;
LevelC *g_level_c;

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
GameStatus g_game_status = MENU;
float g_win_message_timer = 0.0f;
Effects g_effects;

glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

float g_current_zoom = INITIAL_ZOOM;
glm::vec3 g_camera_position = glm::vec3(6.0f, 0.0f, 0.0f);
glm::vec3 g_target_position = glm::vec3(6.0f, 0.0f, 0.0f);

void switch_to_scene(Scene *scene, bool transfer_player = false)
{
    Entity* previous_player = nullptr;
    
    if (transfer_player && g_current_scene != nullptr) {
        previous_player = g_current_scene->get_state().player;
    }
    
    g_current_scene = scene;
    g_current_scene->initialise();
    
    if (previous_player != nullptr) {
        g_current_scene->get_state().player->set_lives(previous_player->get_lives());
    }
}
void initialise();
void process_input();
void update();
void render();
void shutdown();
void update_camera(float delta_time);

void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Platform, Fighter!",
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
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_effects.initialize(V_SHADER_PATH, F_SHADER_PATH);
    g_effects.initialize_lighting(V_LIT_SHADER_PATH, F_LIT_SHADER_PATH);
    
    g_effects.set_light_intensity(DEFAULT_LIGHT_INTENSITY);
    g_effects.set_light_radius(DEFAULT_LIGHT_RADIUS);
    
    g_font_texture_id = Utility::load_texture("assets/font1.png");
    
    g_camera_position = glm::vec3(LEVEL1_WIDTH / 2.0f, 0.0f, 0.0f);
    g_current_zoom = INITIAL_ZOOM;
    
    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_camera_position.x, 3.75f, 0.0f));
    
    g_projection_matrix = glm::ortho(-5.0f / g_current_zoom, 5.0f / g_current_zoom,
                                    -3.75f / g_current_zoom, 3.75f / g_current_zoom,
                                    -1.0f, 1.0f);
    
    g_effects.apply_view_matrix(g_view_matrix);
    g_effects.apply_projection_matrix(g_projection_matrix);
    
    ShaderProgram* active_program = g_effects.get_active_program();
    glUseProgram(active_program->get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // ————— LEVEL SETUP ————— //
    g_menu = new Menu();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    

    switch_to_scene(g_menu);
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
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
                        // ————— JUMPING ————— //
                        if (g_game_status == PLAYING &&
                            g_current_scene->get_state().player->get_collided_bottom())
                        {
                            g_current_scene->get_state().player->jump();
                            Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                        }
                        break;
                        
                    case SDLK_l:
                        if (g_game_status == PLAYING) {
                            g_effects.toggle_lighting();
                        }
                        break;
                        
                    case SDLK_PAGEUP:
                        if (g_game_status == PLAYING) {
                            g_effects.adjust_light_intensity(0.1f);
                        }
                        break;
                    case SDLK_PAGEDOWN:
                        if (g_game_status == PLAYING) {
                            g_effects.adjust_light_intensity(-0.1f);
                        }
                        break;
                    case SDLK_PERIOD:
                        if (g_game_status == PLAYING) {
                            g_effects.adjust_light_radius(0.2f);
                        }
                        break;
                    case SDLK_COMMA:
                        if (g_game_status == PLAYING) {
                            g_effects.adjust_light_radius(-0.2f);
                        }
                        break;
                    
                    case SDLK_r:
                        if (g_game_status == GAME_OVER || g_game_status == GAME_WIN) {
                            g_game_status = PLAYING;
                        }
                    
                        if (g_current_scene == g_level_a) {
                            g_level_a->initialise();
                        }
                        else if (g_current_scene == g_level_b) {
                            g_level_b->initialise();
                        }
                        else if (g_current_scene == g_level_c) {
                            g_level_c->initialise();
                        }
                        
                        if (g_current_scene->get_state().player != nullptr) {
                            glm::vec3 player_position = g_current_scene->get_state().player->get_position();
                            g_camera_position = glm::vec3(player_position.x, player_position.y, 0.0f);
                        }
                        break;
                        
                    case SDLK_RETURN:
                    case SDLK_RETURN2:
                        if (g_game_status == MENU) {
                            g_game_status = PLAYING;
                            switch_to_scene(g_level_a);
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
    
    if (g_game_status == PLAYING) {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
        
        // ————— KEY HOLD ————— //
        const Uint8 *key_state = SDL_GetKeyboardState(NULL);

        if (key_state[SDL_SCANCODE_LEFT])        g_current_scene->get_state().player->move_left();
        else if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->get_state().player->move_right();
        
        if (key_state[SDL_SCANCODE_UP])         g_current_scene->get_state().player->attack();
        else if (key_state[SDL_SCANCODE_DOWN])  g_current_scene->get_state().player->attack2();
        else if (key_state[SDL_SCANCODE_RSHIFT]) g_current_scene->get_state().player->attack3();
         
        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
            g_current_scene->get_state().player->normalise_movement();
    }
}

void update_camera(float delta_time)
{
    if (g_game_status != PLAYING) return;

    glm::vec3 player_position = g_current_scene->get_state().player->get_position();
    glm::vec3 enemy_position = g_current_scene->get_state().enemies[0].get_position();
    
    float current_level_width = LEVEL1_WIDTH;
    if (g_current_scene == g_level_c) {
        current_level_width = LEVEL_C_WIDTH;
    } else if (g_current_scene == g_level_b) {
        current_level_width = 30;
    }
    
    glm::vec3 level_center = glm::vec3(current_level_width / 2.0f, 4.0f, 0.0f);
    
    float player_speed = glm::length(g_current_scene->get_state().player->get_velocity());
    float enemy_speed = glm::length(g_current_scene->get_state().enemies[0].get_velocity());
    bool is_moving = (player_speed > 0.1f) || (enemy_speed > 0.1f);
    

    static bool player_has_moved = false;
    static float zoom_transition_timer = 0.0f;
    static const float ZOOM_TRANSITION_DURATION = 0.5f;
    
    if (player_speed > 0.1f && !player_has_moved) {
        player_has_moved = true;
        zoom_transition_timer = 0.0f;
    }
    
    glm::vec3 target_position;
    
    if (!player_has_moved) {
        target_position = level_center;
        
        g_current_zoom = INITIAL_ZOOM;
        
        g_projection_matrix = glm::ortho(-5.0f / g_current_zoom, 5.0f / g_current_zoom,
                                       -3.75f / g_current_zoom, 3.75f / g_current_zoom,
                                       -1.0f, 1.0f);
        g_effects.apply_projection_matrix(g_projection_matrix);
    }
    else{
        target_position = (player_position * 0.7f) + (enemy_position * 0.3f);
        
       
        float player_enemy_distance = glm::distance(player_position, enemy_position);
        
       
        if (zoom_transition_timer < ZOOM_TRANSITION_DURATION) {
           
            zoom_transition_timer += delta_time;
            if (zoom_transition_timer > ZOOM_TRANSITION_DURATION) {
                zoom_transition_timer = ZOOM_TRANSITION_DURATION;
            }
            
           
            float transition_progress = zoom_transition_timer / ZOOM_TRANSITION_DURATION;
            
           
            float eased_progress = transition_progress * transition_progress; // Quadratic easing
            g_current_zoom = INITIAL_ZOOM + (GAMEPLAY_ZOOM - INITIAL_ZOOM) * eased_progress;
        } else {
           
            float target_zoom;
            
            if (is_moving || player_enemy_distance > 4.0f) {
               
                target_zoom = glm::max(MAX_ZOOM, GAMEPLAY_ZOOM - (player_enemy_distance * 0.03f));
                if (target_zoom < MAX_ZOOM) target_zoom = MAX_ZOOM;
            } else {
                
                target_zoom = GAMEPLAY_ZOOM;
            }
            
            if (g_current_zoom < target_zoom) {
                g_current_zoom += ZOOM_SPEED * delta_time;
                if (g_current_zoom > target_zoom) g_current_zoom = target_zoom;
            } else if (g_current_zoom > target_zoom) {
                g_current_zoom -= ZOOM_SPEED * delta_time;
                if (g_current_zoom < target_zoom) g_current_zoom = target_zoom;
            }
        }
        
        // Update projection matrix
        g_projection_matrix = glm::ortho(-5.0f / g_current_zoom, 5.0f / g_current_zoom,
                                       -3.75f / g_current_zoom, 3.75f / g_current_zoom,
                                       -1.0f, 1.0f);
        g_effects.apply_projection_matrix(g_projection_matrix);
    }
    
   
    float camera_smoothing = CAMERA_LERP;
    if (player_has_moved && zoom_transition_timer < ZOOM_TRANSITION_DURATION) {

        camera_smoothing = CAMERA_LERP * 2.0f;
    }
    
    g_camera_position = g_camera_position + (target_position - g_camera_position) * camera_smoothing;
    
    float visible_width = 10.0f / g_current_zoom;
    float half_visible_width = visible_width / 2.0f;
    
    float left_boundary = half_visible_width;
    float right_boundary = current_level_width - half_visible_width;
    
    if (g_camera_position.x < left_boundary) {
        g_camera_position.x = left_boundary;
    }
    if (g_camera_position.x > right_boundary) {
        g_camera_position.x = right_boundary;
    }
    
    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_camera_position.x, 3.75f, 0.0f));
    g_effects.apply_view_matrix(g_view_matrix);
    
    g_effects.update_light_position(player_position);
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
        if (g_game_status == GAME_WIN) {
            g_win_message_timer += FIXED_TIMESTEP;
            delta_time -= FIXED_TIMESTEP;
            continue;
        }
        
        if (g_game_status == MENU) {
            g_current_scene->update(FIXED_TIMESTEP);
            
            if (((Menu*)g_current_scene)->is_game_start()) {
                g_game_status = PLAYING;
                switch_to_scene(g_level_a);
            }
        }
        else if (g_game_status == PLAYING) {
            g_current_scene->update(FIXED_TIMESTEP);
            
            if (g_current_scene->get_state().game_over) {
                g_game_status = GAME_OVER;
            }
            
            if (g_current_scene->get_state().level_completed) {
                if (g_current_scene == g_level_a) {
                    switch_to_scene(g_level_b, true);
                    
                    glm::vec3 player_position = g_current_scene->get_state().player->get_position();
                    g_camera_position = glm::vec3(player_position.x, player_position.y, 0.0f);
                }
                else if (g_current_scene == g_level_b) {
                    switch_to_scene(g_level_c, true);
                    
                    glm::vec3 player_position = g_current_scene->get_state().player->get_position();
                    g_camera_position = glm::vec3(player_position.x, player_position.y, 0.0f);
                }
                else if (g_current_scene == g_level_c) {
                    g_game_status = GAME_WIN;
                    g_win_message_timer = 0.0f;
                }
            }
            
            // Update camera based on the current scene
            if (g_current_scene == g_level_b || g_current_scene == g_level_c) {
                // For Level B and C, use their custom camera update methods
                if (g_current_scene == g_level_b) {
                    g_level_b->update_camera(FIXED_TIMESTEP);
                } else {
                    g_level_c->update_camera(FIXED_TIMESTEP);
                }
            } else {
                // For other levels, use the regular camera update
                update_camera(FIXED_TIMESTEP);
            }
            
            // Update shader view matrices after camera position has been updated
            g_effects.apply_view_matrix(g_view_matrix);
        }
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
}

 void render()
{
    ShaderProgram* program_to_use;
    
    if (g_game_status == MENU) {
        program_to_use = &g_effects.get_default_program();
    }
    else {
        program_to_use = g_effects.get_active_program();
        if (program_to_use == nullptr) return;
    }
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // ————— RENDERING THE SCENE ————— //
    // Menu state
    if (g_game_status == MENU) {
        glm::mat4 ui_view_matrix = glm::mat4(1.0f);
        glm::mat4 ui_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
        
        program_to_use->set_view_matrix(ui_view_matrix);
        program_to_use->set_projection_matrix(ui_projection_matrix);
        
        g_current_scene->render(program_to_use);
    }
    // Gameplay state
    else if (g_game_status == PLAYING) {
        g_current_scene->render(program_to_use);
        
        glm::mat4 original_view_matrix = g_view_matrix;
        glm::mat4 original_projection_matrix = g_projection_matrix;
        
        glm::mat4 ui_view_matrix = glm::mat4(1.0f);
        glm::mat4 ui_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
        
        program_to_use->set_view_matrix(ui_view_matrix);
        program_to_use->set_projection_matrix(ui_projection_matrix);
        

        std::string player_lives_text = "Lives: " + std::to_string(g_current_scene->get_state().player->get_lives());
        Utility::draw_text(program_to_use, g_font_texture_id, player_lives_text,
                          0.4f, 0.0f, glm::vec3(-4.5f, 3.4f, 0.0f));
        
        std::string enemy_lives_text = "Lives: " + std::to_string(g_current_scene->get_state().enemies[0].get_lives());
        Utility::draw_text(program_to_use, g_font_texture_id, enemy_lives_text,
                        0.4f, 0.0f, glm::vec3(1.8f, 3.4f, 0.0f));
        
        // Restore original matrices
        program_to_use->set_view_matrix(original_view_matrix);
        program_to_use->set_projection_matrix(original_projection_matrix);
    }
    else if (g_game_status == GAME_OVER) {
        glClearColor(0.8f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glm::mat4 ui_view_matrix = glm::mat4(1.0f);
        glm::mat4 ui_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
        
        program_to_use->set_view_matrix(ui_view_matrix);
        program_to_use->set_projection_matrix(ui_projection_matrix);
        
        std::string game_over_text = "GAME OVER";
        Utility::draw_text(program_to_use, g_font_texture_id, game_over_text,
                         0.8f, 0.0f, glm::vec3(-3.0, 0.5f, 0.0f));
        
        std::string quit_text = "Press Q to quit";
        Utility::draw_text(program_to_use, g_font_texture_id, quit_text,
                         0.35f, 0.0f, glm::vec3(-2.2f, -0.5f, 0.0f));
        
        std::string restart_text = "Press R to restart level";
        Utility::draw_text(program_to_use, g_font_texture_id, restart_text,
                         0.35f, 0.0f, glm::vec3(-3.9f, -1.0f, 0.0f));
    }
    else if (g_game_status == GAME_WIN) {
        glClearColor(0.6f, 0.8f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glm::mat4 ui_view_matrix = glm::mat4(1.0f);
        glm::mat4 ui_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
        
        program_to_use->set_view_matrix(ui_view_matrix);
        program_to_use->set_projection_matrix(ui_projection_matrix);
        
        std::string win_text = "YOU WIN!";
        Utility::draw_text(program_to_use, g_font_texture_id, win_text,
                         0.8f, 0.0f, glm::vec3(-2.8f, 0.8f, 0.0f));
        
        std::string congrats_text = "Congratulations!";
        Utility::draw_text(program_to_use, g_font_texture_id, congrats_text,
                         0.5f, 0.0f, glm::vec3(-3.5f, -0.2f, 0.0f));
        
        std::string quit_text = "Press Q to quit";
        Utility::draw_text(program_to_use, g_font_texture_id, quit_text,
                         0.3f, 0.0f, glm::vec3(-2.0f, -1.2f, 0.0f));
    }
    
    SDL_GL_SwapWindow(g_display_window);
}
    void shutdown()
    {
        SDL_Quit();
        
        // ————— DELETING LEVEL DATA ————— //
        if (g_menu != nullptr) {
            delete g_menu;
            g_menu = nullptr;
        }
        
        if (g_level_a != nullptr) {
            delete g_level_a;
            g_level_a = nullptr;
        }
        
        if (g_level_b != nullptr) {
            delete g_level_b;
            g_level_b = nullptr;
        }
        
        if (g_level_c != nullptr) {
            delete g_level_c;
            g_level_c = nullptr;
        }
    }

    // ————— GAME LOOP ————— //
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





