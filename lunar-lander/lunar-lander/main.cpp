/**
* Author: Jenna Nandlall
* Assignment: Lunar Lander
* Date due: 2025-3-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "Entity.h"
#include <vector>
#include <ctime>
#include "cmath"
#include <iostream>

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640 * 2,
              WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED     = 0.0f,
                BG_GREEN   = 0.0f,
                BG_BLUE    = 0.1f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr char SPACECRAFT_FILEPATH[] = "spacecraft.png";
constexpr char PLATFORM_FILEPATH[] = "platform.png";
constexpr char BACKGROUND_FILEPATH[] = "background.png";
constexpr char FONTSHEET_FILEPATH[] = "font.png";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

constexpr float GRAVITY = 0.05f;
constexpr float ACCELERATION = 0.2f;
constexpr float FUEL_CONSUMPTION_RATE = 8.0f;
constexpr float MAX_FUEL = 100.0f;
constexpr float PLATFORM_MOVEMENT_SPEED = 1.0f;
constexpr float PLATFORM_MOVEMENT_RANGE = 3.0f;

// ————— GAME STATE ————— //
enum GameStatus { PLAYING, MISSION_ACCOMPLISHED, MISSION_FAILED };

// ————— STRUCTS AND ENUMS —————//
struct GameState
{
    Entity* player;
    Entity* platforms[3];
    GameStatus status;
    float time_left;
    
    float platform_direction = 1.0f;
    float platform_initial_x = -0.3f;
};

// ————— VARIABLES ————— //
GameState g_game_state;

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

// ————— FUNCTION PROTOTYPES ————— //
void initialise();
void process_input();
void update();
void render();
void shutdown();
void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text, float size, float spacing, glm::vec3 position);


GLuint load_texture(const char* filepath);

// ———— GENERAL FUNCTIONS ———— //
GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_image_free(image);
    
    return texture_id;
}

void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text, float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;
    
    for (int i = 0; i < text.size(); i++) {
        int spritesheet_index = (int) text[i];
        float offset = (size + spacing) * i;
        
        float u_coordinate = (float) (spritesheet_index % 16) / 16.0f;
        float v_coordinate = (float) (spritesheet_index / 16) / 16.0f;
        
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        
        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
        });
    }
    
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    
    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int) (text.size() * 6));
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Lunar Lander",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT,
                                        SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
    
    GLuint player_texture_id = load_texture(SPACECRAFT_FILEPATH);
    GLuint platform_texture_id = load_texture(PLATFORM_FILEPATH);
    GLuint background_texture_id = load_texture(BACKGROUND_FILEPATH);
    
    g_game_state.status = PLAYING;
    
    std::vector<GLuint> player_texture_ids = { player_texture_id };
    g_game_state.player = new Entity(player_texture_ids, 0.0f);
    g_game_state.player->set_position(glm::vec3(0.0f, 3.5f, 0.0f));
    g_game_state.player->set_scale(glm::vec3(0.5f, 0.5f, 0.0f));
    g_game_state.player->set_acceleration(glm::vec3(0.0f, -GRAVITY, 0.0f));
    g_game_state.player->set_fuel(MAX_FUEL);
    
    std::vector<GLuint> platform_texture_ids = { platform_texture_id };
    
    //platforms
    // left mountain/hill
    g_game_state.platforms[0] = new Entity(platform_texture_ids, 0.0f);
    g_game_state.platforms[0]->set_position(glm::vec3(-3.9f, -2.2f, 0.0f));
    g_game_state.platforms[0]->set_scale(glm::vec3(1.7f, 0.5f, 0.0f));

    // middle blue mountain peak
    g_game_state.platforms[1] = new Entity(platform_texture_ids, 0.0f);
    g_game_state.platforms[1]->set_position(glm::vec3(g_game_state.platform_initial_x, -0.5f, 0.0f));
    g_game_state.platforms[1]->set_scale(glm::vec3(1.7f, 0.5f, 0.0f));

    // right mountain
    g_game_state.platforms[2] = new Entity(platform_texture_ids, 0.0f);
    g_game_state.platforms[2]->set_position(glm::vec3(3.6f, -2.2f, 0.0f));
    g_game_state.platforms[2]->set_scale(glm::vec3(1.7f, 0.5f, 0.0f));
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                        g_game_is_running = false;
                        break;
                }
                
            default:
                break;
        }
    }
    
    if (g_game_state.status != PLAYING) {
        return;
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    g_game_state.player->set_acceleration(glm::vec3(0.0f, -GRAVITY, 0.0f));
    
    const float ROTATION_SPEED = 3.0f;
    
    float player_fuel = g_game_state.player->get_fuel();
    if (player_fuel > 0) {
        if (key_state[SDL_SCANCODE_UP]) {
            float rotation_radians = glm::radians(g_game_state.player->get_rotation_angle());
            
        
            float thrust_x = -ACCELERATION * sin(rotation_radians);
            float thrust_y = ACCELERATION * cos(rotation_radians);
            
            g_game_state.player->set_acceleration(glm::vec3(
                g_game_state.player->get_acceleration().x + thrust_x,
                g_game_state.player->get_acceleration().y + thrust_y,
                0.0f
            ));
            
            player_fuel -= FUEL_CONSUMPTION_RATE * FIXED_TIMESTEP;
            g_game_state.player->set_fuel(player_fuel);
        }
        
        // key states
        if (key_state[SDL_SCANCODE_LEFT]) {
            g_game_state.player->rotate(ROTATION_SPEED);
            player_fuel -= FUEL_CONSUMPTION_RATE * FIXED_TIMESTEP * 0.5f;
            g_game_state.player->set_fuel(player_fuel);
        }
        else if (key_state[SDL_SCANCODE_RIGHT]) {
            g_game_state.player->rotate(-ROTATION_SPEED);
            player_fuel -= FUEL_CONSUMPTION_RATE * FIXED_TIMESTEP * 0.5f;
            g_game_state.player->set_fuel(player_fuel);
        }
    }
    
    if (player_fuel < 0) {
        g_game_state.player->set_fuel(0);
    }
}
void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP) {
        g_accumulator = delta_time;
        return;
    }
    
    if (g_game_state.status != PLAYING) {
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        if (g_game_state.status == PLAYING) {
            g_game_state.player->update(FIXED_TIMESTEP);
            
            glm::vec3 platform_position = g_game_state.platforms[1]->get_position();
            
            float distance_from_initial = platform_position.x - g_game_state.platform_initial_x;
            
            if (distance_from_initial >= PLATFORM_MOVEMENT_RANGE / 2.0f) {
                g_game_state.platform_direction = -1.0f;
            } else if (distance_from_initial <= -PLATFORM_MOVEMENT_RANGE / 2.0f) {
                g_game_state.platform_direction = 1.0f;
            }
            
            platform_position.x += g_game_state.platform_direction * PLATFORM_MOVEMENT_SPEED * FIXED_TIMESTEP;
            g_game_state.platforms[1]->set_position(platform_position);
            
            bool on_platform = false;
            
            for (int i = 0; i < 3; i++) {
                if (g_game_state.player->check_collision_with_platform(g_game_state.platforms[i])) {
                    on_platform = true;
                    g_game_state.status = MISSION_ACCOMPLISHED;
                    break;
                }
            }
            
            if (!on_platform) {
                if (g_game_state.player->check_collision_with_boundaries(-5.0f, 5.0f, -3.75f, 3.75f)) {
                    g_game_state.status = MISSION_FAILED;
                }
                
                if (g_game_state.player->get_position().y < -3.0f) {
                    g_game_state.status = MISSION_FAILED;
                }
            }
        }
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
}
void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[] = {
        -5.0f, -3.75f,  // bottom left
        5.0f, -3.75f,   // bottom right
        5.0f, 3.75f,    // top right

        -5.0f, -3.75f,  // bottom left
        5.0f, 3.75f,    // top right
        -5.0f, 3.75f    // top left
    };

    float texCoords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };

    // setup for background drawing
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // draw background
    glm::mat4 bg_matrix = glm::mat4(1.0f);
    g_shader_program.set_model_matrix(bg_matrix);
    glBindTexture(GL_TEXTURE_2D, load_texture(BACKGROUND_FILEPATH));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    for (int i = 0; i < 3; i++) {
        if (g_game_state.platforms[i]) { 
            g_game_state.platforms[i]->render(&g_shader_program);
        }
    }
    
    g_game_state.player->render(&g_shader_program);
    
    GLuint font_texture_id = load_texture(FONTSHEET_FILEPATH);
    
    std::string fuel_text = "FUEL: " + std::to_string((int)g_game_state.player->get_fuel()) + "%";
    draw_text(&g_shader_program, font_texture_id, fuel_text, 0.3f, 0.001f, glm::vec3(-4.5f, 3.5f, 0.0f));
    
    std::string vel_x_text = "Horizontal Speed: " + std::to_string(g_game_state.player->get_velocity().x).substr(0, 4);
    draw_text(&g_shader_program, font_texture_id, vel_x_text, 0.3f, 0.001f, glm::vec3(-4.5f, 3.0f, 0.0f));
    
    std::string vel_y_text = "Vertical Speed: " + std::to_string(g_game_state.player->get_velocity().y).substr(0, 4);
    draw_text(&g_shader_program, font_texture_id, vel_y_text, 0.3f, 0.001f, glm::vec3(-4.5f, 2.7f, 0.0f));
    
    if (g_game_state.status == MISSION_ACCOMPLISHED) {
        std::string message = "MISSION ACCOMPLISHED!";
        float text_width = message.length() * 0.4f;
        float x_position = -text_width / 2.2f;
        draw_text(&g_shader_program, font_texture_id, message, 0.4f, 0.001f, glm::vec3(x_position, 0.5f, 0.0f));
    }
    else if (g_game_state.status == MISSION_FAILED) {
        std::string message = "MISSION FAILED!";
        float text_width = message.length() * 0.4f;
        float x_position = -text_width / 2.2f;
        draw_text(&g_shader_program, font_texture_id, message, 0.4f, 0.001f, glm::vec3(x_position, 0.5f, 0.0f));
    }
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    delete g_game_state.player;
    
    for (int i = 0; i < 3; i++) {
        delete g_game_state.platforms[i];
    }
}

int main(int argc, char* argv[])
{
    initialise();
    
    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
