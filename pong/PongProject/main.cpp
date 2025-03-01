/**
* Author: Jenna Nandlall
* Assignment: Pong Clone
* Date due: 2025-3-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <iostream>
#include <string>

enum GameStatus { RUNNING, TERMINATED };

constexpr int WINDOW_WIDTH  = 640 * 2,
              WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED     = 0.1f,
                BG_GREEN   = 0.5f,
                BG_BLUE    = 0.8f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL    = 0;
constexpr GLint TEXTURE_BORDER     = 0;

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

// file paths for textures
constexpr char BACKGROUND_FILEPATH[] = "underwater.jpg",
               PLAYER1_FILEPATH[]    = "pink.png",
               PLAYER2_FILEPATH[]    = "orange.png",
               BALL_FILEPATH[]       = "bubble.png",
               GAME_OVER_FILEPATH[]  = "game-over.png",
               PINK_WINS_FILEPATH[]  = "pink-wins.png",
               ORANGE_WINS_FILEPATH[] = "orange-wins.png";

// constants
constexpr float PADDLE_SPEED = 3.0f;
constexpr float COMP_PADDLE_SPEED = 2.0f;
constexpr float BALL_SPEED = 3.0f;
constexpr float PADDLE_WIDTH = 0.8f;
constexpr float PADDLE_HEIGHT = 1.6f;
constexpr float BALL_SIZE = 0.4f;

SDL_Window* g_display_window;

GameStatus g_game_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;

// Texture IDs
GLuint g_background_texture_id;
GLuint g_player1_texture_id;
GLuint g_player2_texture_id;
GLuint g_ball_texture_id;

// Game objects
glm::mat4 g_background_matrix;
glm::mat4 g_player1_matrix;
glm::mat4 g_player2_matrix;
glm::mat4 g_ball_matrix;

// Object positions
glm::vec3 g_player1_position = glm::vec3(-4.5f, 0.0f, 0.0f);
glm::vec3 g_player2_position = glm::vec3(4.5f, 0.0f, 0.0f);
glm::vec3 g_ball_position = glm::vec3(0.0f, 0.0f, 0.0f);

// Movement vectors
glm::vec3 g_player1_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_player2_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);

// arrays for multiple balls
constexpr int MAX_BALLS = 3;
int g_num_balls = 1; // start with 1 ball
glm::vec3 g_ball_positions[MAX_BALLS];
glm::vec3 g_ball_movements[MAX_BALLS];
glm::mat4 g_ball_matrices[MAX_BALLS];


bool g_single_player_mode = false;
float g_ai_direction = 1.0f;  // direction for computer paddle movement (1 = up, -1 = down)

GLuint g_game_over_texture_id;
GLuint g_pink_wins_texture_id;
GLuint g_orange_wins_texture_id;
glm::mat4 g_game_over_matrix;
glm::mat4 g_winner_text_matrix;

bool g_game_over = false;
float g_pulse_factor = 1.0f;
float g_pulse_direction = 0.1f;

int g_winner = 0; // 0 = no winner, 1 = player 1 (pink), 2 = player 2 (orange)


// Function declaration prototypes
void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct: " << filepath);
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    // Initialize SDL and create window
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Underwater Pong Clone",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr) shutdown();

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    // initialize matrices
    g_background_matrix = glm::mat4(1.0f);
    g_player1_matrix = glm::mat4(1.0f);
    g_player2_matrix = glm::mat4(1.0f);
    g_ball_matrix = glm::mat4(1.0f);
    g_game_over_matrix = glm::mat4(1.0f);
    g_winner_text_matrix = glm::mat4(1.0f);


    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

    // load textures
    g_background_texture_id = load_texture(BACKGROUND_FILEPATH);
    g_player1_texture_id = load_texture(PLAYER1_FILEPATH);
    g_player2_texture_id = load_texture(PLAYER2_FILEPATH);
    g_ball_texture_id = load_texture(BALL_FILEPATH);
    g_game_over_texture_id = load_texture(GAME_OVER_FILEPATH);
    g_pink_wins_texture_id = load_texture(PINK_WINS_FILEPATH);
    g_orange_wins_texture_id = load_texture(ORANGE_WINS_FILEPATH);

    // initialize ball movement with fixed direction
    g_ball_movement = glm::vec3(0.7f, -0.7f, 0.0f);
    g_ball_movement = glm::normalize(g_ball_movement);

    // initialize all balls with different starting positions and velocities
    g_ball_positions[0] = glm::vec3(0.0f, 0.0f, 0.0f);
    g_ball_movements[0] = glm::normalize(glm::vec3(0.7f, -0.7f, 0.0f));
    g_ball_matrices[0] = glm::mat4(1.0f);
    
    g_ball_positions[1] = glm::vec3(0.0f, 1.5f, 0.0f);
    g_ball_movements[1] = glm::normalize(glm::vec3(0.8f, -0.5f, 0.0f));
    g_ball_matrices[1] = glm::mat4(1.0f);
    
    g_ball_positions[2] = glm::vec3(0.0f, -1.5f, 0.0f);
    g_ball_movements[2] = glm::normalize(glm::vec3(0.6f, 0.9f, 0.0f));
    g_ball_matrices[2] = glm::mat4(1.0f);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // Reset movement
    g_player1_movement = glm::vec3(0.0f);
    g_player2_movement = glm::vec3(0.0f);
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                        g_game_status = TERMINATED;
                        break;
                    
                    case SDLK_r:
                        // reset the game whether it's over or not, ball positions and directions
                        g_num_balls = 1; // reset to 1 ball on restart
                        
                        g_ball_positions[0] = glm::vec3(0.0f, 0.0f, 0.0f);
                        g_ball_movements[0] = glm::normalize(glm::vec3(0.7f, -0.7f, 0.0f));
                        
                        g_ball_positions[1] = glm::vec3(0.0f, 1.5f, 0.0f);
                        g_ball_movements[1] = glm::normalize(glm::vec3(0.8f, -0.5f, 0.0f));
                        
                        g_ball_positions[2] = glm::vec3(0.0f, -1.5f, 0.0f);
                        g_ball_movements[2] = glm::normalize(glm::vec3(0.6f, 0.9f, 0.0f));
                        
                        // reset game over state
                        g_game_over = false;
                        g_winner = 0;
                        g_winner_text_matrix = glm::mat4(1.0f);
                        break;
                    
                    case SDLK_t:
                        // toggle single player mode (only if game is not over)
                        if (!g_game_over) {
                            g_single_player_mode = !g_single_player_mode;
                        }
                        break;
                        
                    case SDLK_1:
                    case SDLK_2:
                    case SDLK_3:
                        // only allow changing number of balls if game is not over
                        if (!g_game_over) {
                            int new_num_balls = event.key.keysym.sym - SDLK_0; // converts to number
                            
                            // initialize newly added balls
                            if (new_num_balls > g_num_balls) {
                                for (int i = g_num_balls; i < new_num_balls; i++) {
                                    if (i == 1) {
                                        // initialize ball 2
                                        g_ball_positions[1] = glm::vec3(0.0f, 1.5f, 0.0f);
                                        g_ball_movements[1] = glm::normalize(glm::vec3(0.8f, -0.5f, 0.0f));
                                    } else if (i == 2) {
                                        // initialize ball
                                        g_ball_positions[2] = glm::vec3(0.0f, -1.5f, 0.0f);
                                        g_ball_movements[2] = glm::normalize(glm::vec3(0.6f, 0.9f, 0.0f));
                                    }
                                }
                            }
                            
                            // update the number of balls
                            g_num_balls = new_num_balls;
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

    if (g_game_over) return;

    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    // player 1 controls up and down keys
    if (key_state[SDL_SCANCODE_UP]) {
        g_player1_movement.y = 1.0f;
    } else if (key_state[SDL_SCANCODE_DOWN]) {
        g_player1_movement.y = -1.0f;
    }
    
    // player 2 controls w and s keys only if not in single player mode
    if (!g_single_player_mode) {
        if (key_state[SDL_SCANCODE_W]) {
            g_player2_movement.y = 1.0f;
        } else if (key_state[SDL_SCANCODE_S]) {
            g_player2_movement.y = -1.0f;
        }
    }
}

void update()
{
    // calculate delta time
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    // if game is over
    if (g_game_over) {
        
        // pulsating effect for game over text
        g_pulse_factor += g_pulse_direction * delta_time;
        
        if (g_pulse_factor > 1.2f) {
            g_pulse_factor = 1.2f;
            g_pulse_direction = -0.1f;  // shrinking
        } else if (g_pulse_factor < 0.8f) {
            g_pulse_factor = 0.8f;
            g_pulse_direction = 0.1f;   // growing
        }
        
        // update game over matrix with pulsating effect
        g_game_over_matrix = glm::mat4(1.0f);
        g_game_over_matrix = glm::translate(g_game_over_matrix, glm::vec3(0.0f, 0.5f, 0.0f));
        g_game_over_matrix = glm::scale(g_game_over_matrix, glm::vec3(4.0f * g_pulse_factor, 3.0f * g_pulse_factor, 1.0f));
        
        // update winner text matrix with same pulsating effect
        g_winner_text_matrix = glm::mat4(1.0f);
        g_winner_text_matrix = glm::translate(g_winner_text_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
        g_winner_text_matrix = glm::scale(g_winner_text_matrix, glm::vec3(3.0f * g_pulse_factor, 0.7f * g_pulse_factor, 1.0f));
        
        // update other matrices
        g_background_matrix = glm::mat4(1.0f);
        g_background_matrix = glm::scale(g_background_matrix, glm::vec3(10.0f, 7.5f, 1.0f));
        
        g_player1_matrix = glm::mat4(1.0f);
        g_player1_matrix = glm::translate(g_player1_matrix, g_player1_position);
        g_player1_matrix = glm::scale(g_player1_matrix, glm::vec3(PADDLE_WIDTH, PADDLE_HEIGHT, 1.0f));
        
        g_player2_matrix = glm::mat4(1.0f);
        g_player2_matrix = glm::translate(g_player2_matrix, g_player2_position);
        g_player2_matrix = glm::scale(g_player2_matrix, glm::vec3(PADDLE_WIDTH, PADDLE_HEIGHT, 1.0f));
        
        // update all ball matrices that are frozen in position
        for (int i = 0; i < g_num_balls; i++) {
            g_ball_matrices[i] = glm::mat4(1.0f);
            g_ball_matrices[i] = glm::translate(g_ball_matrices[i], g_ball_positions[i]);
            g_ball_matrices[i] = glm::scale(g_ball_matrices[i], glm::vec3(BALL_SIZE, BALL_SIZE, 1.0f));
        }
        
        g_ball_matrix = g_ball_matrices[0];
        g_ball_position = g_ball_positions[0];
        g_ball_movement = g_ball_movements[0];
    }
    
    // if game is not over
    else {
        
        // update player 1 position based on movement input
        g_player1_position += g_player1_movement * PADDLE_SPEED * delta_time;
        
        // keep player 1 within screen bounds
        if (g_player1_position.y > 3.5f - PADDLE_HEIGHT/2) {
            g_player1_position.y = 3.5f - PADDLE_HEIGHT/2;
        } else if (g_player1_position.y < -3.5f + PADDLE_HEIGHT/2) {
            g_player1_position.y = -3.5f + PADDLE_HEIGHT/2;
        }
        
        // update player 2 position (either computer or human controlled)
        if (g_single_player_mode) {
            g_player2_position.y += g_ai_direction * COMP_PADDLE_SPEED * delta_time;
            
            if (g_player2_position.y > 3.0f - PADDLE_HEIGHT/2) {
                g_player2_position.y = 3.0f - PADDLE_HEIGHT/2;
                g_ai_direction = -1.0f;
            } else if (g_player2_position.y < -3.0f + PADDLE_HEIGHT/2) {
                g_player2_position.y = -3.0f + PADDLE_HEIGHT/2;
                g_ai_direction = 1.0f;
            }
        } else {
            g_player2_position += g_player2_movement * PADDLE_SPEED * delta_time;
            
            if (g_player2_position.y > 3.5f - PADDLE_HEIGHT/2) {
                g_player2_position.y = 3.5f - PADDLE_HEIGHT/2;
            } else if (g_player2_position.y < -3.5f + PADDLE_HEIGHT/2) {
                g_player2_position.y = -3.5f + PADDLE_HEIGHT/2;
            }
        }
        
        bool game_over_triggered = false;
        
        for (int i = 0; i < g_num_balls; i++) {
            g_ball_positions[i] += g_ball_movements[i] * BALL_SPEED * delta_time;
            if (g_ball_positions[i].y > 3.75f - BALL_SIZE/2 || g_ball_positions[i].y < -3.75f + BALL_SIZE/2) {
                g_ball_movements[i].y *= -1.0f;
            }
            
            // ---- COLLISIONS --- //
            
            // COLLISION DETECTION: player 1 paddle
            // calculate distances between ball and paddle centers
            float p1_x_distance = fabs(g_player1_position.x - g_ball_positions[i].x) - ((PADDLE_WIDTH + BALL_SIZE) / 2.0f);
            float p1_y_distance = fabs(g_player1_position.y - g_ball_positions[i].y) - ((PADDLE_HEIGHT + BALL_SIZE) / 2.0f);
            
            if (p1_x_distance < 0.0f && p1_y_distance < 0.0f) {
                g_ball_movements[i].x = fabs(g_ball_movements[i].x);
                g_ball_movements[i].y += 0.1f;
                g_ball_movements[i] = glm::normalize(g_ball_movements[i]);
            }
            
            // COLLISION DETECTION: player 2 paddle
            float p2_x_distance = fabs(g_player2_position.x - g_ball_positions[i].x) - ((PADDLE_WIDTH + BALL_SIZE) / 2.0f);
            float p2_y_distance = fabs(g_player2_position.y - g_ball_positions[i].y) - ((PADDLE_HEIGHT + BALL_SIZE) / 2.0f);
            
            if (p2_x_distance < 0.0f && p2_y_distance < 0.0f) {
                g_ball_movements[i].x = -fabs(g_ball_movements[i].x);
                g_ball_movements[i].y -= 0.1f;
                g_ball_movements[i] = glm::normalize(g_ball_movements[i]);
            }
            
            // game over check: player 2 wins
            if (g_ball_positions[i].x < -5.0f && !game_over_triggered) {
                g_game_over = true;
                g_winner = 2;
                game_over_triggered = true;
                
                g_game_over_matrix = glm::mat4(1.0f);
                g_game_over_matrix = glm::translate(g_game_over_matrix, glm::vec3(0.0f, 0.5f, 0.0f));
                g_game_over_matrix = glm::scale(g_game_over_matrix, glm::vec3(3.0f, 1.5f, 1.0f));
                
                g_winner_text_matrix = glm::mat4(1.0f);
                g_winner_text_matrix = glm::translate(g_winner_text_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
                g_winner_text_matrix = glm::scale(g_winner_text_matrix, glm::vec3(2.0f, 0.8f, 1.0f));
            }
            // game over check: player 1 wins
            else if (g_ball_positions[i].x > 5.0f && !game_over_triggered) {
                g_game_over = true;
                g_winner = 1;
                game_over_triggered = true;
                
                g_game_over_matrix = glm::mat4(1.0f);
                g_game_over_matrix = glm::translate(g_game_over_matrix, glm::vec3(0.0f, 0.5f, 0.0f));
                g_game_over_matrix = glm::scale(g_game_over_matrix, glm::vec3(3.0f, 1.5f, 1.0f));
                
                g_winner_text_matrix = glm::mat4(1.0f);
                g_winner_text_matrix = glm::translate(g_winner_text_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
                g_winner_text_matrix = glm::scale(g_winner_text_matrix, glm::vec3(2.0f, 0.8f, 1.0f));
            }
            
            g_ball_matrices[i] = glm::mat4(1.0f);
            g_ball_matrices[i] = glm::translate(g_ball_matrices[i], g_ball_positions[i]);
            g_ball_matrices[i] = glm::scale(g_ball_matrices[i], glm::vec3(BALL_SIZE, BALL_SIZE, 1.0f));
        }
        
        g_ball_position = g_ball_positions[0];
        g_ball_movement = g_ball_movements[0];
        g_ball_matrix = g_ball_matrices[0];
        
        g_background_matrix = glm::mat4(1.0f);
        g_background_matrix = glm::scale(g_background_matrix, glm::vec3(10.0f, 7.5f, 1.0f));
        
        
        // scaling and translating
        g_player1_matrix = glm::mat4(1.0f);
        g_player1_matrix = glm::translate(g_player1_matrix, g_player1_position);
        g_player1_matrix = glm::scale(g_player1_matrix, glm::vec3(PADDLE_WIDTH, PADDLE_HEIGHT, 1.0f));
        
        
        g_player2_matrix = glm::mat4(1.0f);
        g_player2_matrix = glm::translate(g_player2_matrix, g_player2_position);
        g_player2_matrix = glm::scale(g_player2_matrix, glm::vec3(PADDLE_WIDTH, PADDLE_HEIGHT, 1.0f));
    }
}

void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    draw_object(g_background_matrix, g_background_texture_id);
    
    draw_object(g_player1_matrix, g_player1_texture_id);
    draw_object(g_player2_matrix, g_player2_texture_id);
    
    
    for (int i = 0; i < g_num_balls; i++) {
        draw_object(g_ball_matrices[i], g_ball_texture_id);
    }
    
    if (g_game_over) {
        draw_object(g_game_over_matrix, g_game_over_texture_id);
        if (g_winner == 1) {
            draw_object(g_winner_text_matrix, g_pink_wins_texture_id);
        }
        else if (g_winner == 2) {
            draw_object(g_winner_text_matrix, g_orange_wins_texture_id);
        }
    }

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    initialise();

    while (g_game_status != TERMINATED)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
