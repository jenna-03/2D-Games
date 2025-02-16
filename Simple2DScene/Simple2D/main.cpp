/**
 * @file main.cpp
 * @author Sebastián Romero Cruz (src402@nyu.edu)
 * @brief A simple program that creates a window with OpenGL context and 
 * renders a colored triangle. This program demonstrates basic usage of SDL to
 * create a window, create an OpenGL context, and render a simple colored 
 * triangle using shaders.
 * @version 0.1
 * @date 2024-05-20
 * 
 * @copyright Copyright NYU Tandon School of Engineering (c) 2024
 *
 * Author: Jenna Nandlall
 * Assignment: Simple 2D Scene
 * Date due: 2025-02-15, 11:59pm
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 */


#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

enum AppStatus { RUNNING, TERMINATED };

// window dimensions
constexpr int WINDOW_WIDTH  = 640 * 2,
              WINDOW_HEIGHT = 480 * 2;

// background color components
constexpr float BG_RED     = 0.1922f,
                BG_BLUE    = 0.549f,
                BG_GREEN   = 0.9059f,
                BG_OPACITY = 1.0f;

// viewport—or our "camera"'s—position and dimensions
constexpr int VIEWPORT_X      = 0,
              VIEWPORT_Y      = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;


constexpr char V_SHADER_PATH[] = "Simple2D/shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "Simple2D/shaders/fragment_textured.glsl";


constexpr int NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

constexpr float MILLISECONDS_IN_SECOND = 1000.0f;
constexpr float DEGREES_PER_SECOND = 90.0f;

constexpr float ORBIT_RADIUS = 2.0f;
constexpr float ORBIT_SPEED = 1.0f;           // controls orbital movement speed
constexpr float GUDETAMA_ORBIT_RADIUS = 2.0f;
constexpr float GUDETAMA_ORBIT_SPEED = 2.0f;   // how fast Gudetama orbits around Stitch


constexpr float GROWTH_FACTOR = 0.9f;
constexpr float SHRINK_FACTOR = 0.6f;
constexpr int MAX_FRAME = 60;

int g_frame_counter = 0;
bool g_is_growing = true;


constexpr char STITCH_SPRITE[] = "stitch.png",
               GUDETAMA_SPRITE[] = "gudetama.png";

AppStatus g_app_status = RUNNING;
SDL_Window* g_display_window;

ShaderProgram g_shader_program;

glm::mat4 g_view_matrix,        // defines the position of the camera
          g_projection_matrix;  // defines the characteristics of your camera


float g_previous_ticks = 0.00;


glm::mat4 g_stitch_matrix, g_gudetama_matrix;
glm::vec3 g_stitch_position = glm::vec3(0.0f, 0.0f, 0.0f), g_gudetama_position = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_stitch_rotation = glm::vec3(0.0f, 0.0f, 0.0f),
          g_gudetama_rotation = glm::vec3(0.0f, 0.0f, 0.0f);

// initial vectors
constexpr glm::vec3 INIT_SCALE = glm::vec3(2.0f, 2.0f, 0.0f),
                    INIT_POS_STITCH = glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_POS_GUDETAMA = glm::vec3(0.0f, 0.0f, 0.0f);

GLuint g_stitch_texture_id, g_gudetama_texture_id;

GLuint load_texture(const char* filepath) {
    
    // loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    // check if path is correct
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
        LOG("STB Error: " << stbi_failure_reason());
        assert(false);
    }
    

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    

    stbi_image_free(image);
    
    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Simple 2D Scene",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    if (g_display_window == nullptr)
    {
        std::cerr << "ERROR: SDL Window could not be created.\n";
        g_app_status = TERMINATED;
        
        SDL_Quit();
        exit(1);
    }
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    g_stitch_texture_id = load_texture(STITCH_SPRITE);
    g_gudetama_texture_id = load_texture(GUDETAMA_SPRITE);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    // load shaders
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix       = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    g_stitch_matrix = glm::mat4(1.0f);
    g_gudetama_matrix = glm::mat4(1.0f);

    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
        }
    }
}

void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
        float delta_time = ticks - g_previous_ticks;
        g_previous_ticks = ticks;
        
        // create pulsing scale effect
        g_frame_counter += 1;
        if (g_frame_counter >= MAX_FRAME)
        {
            g_is_growing = !g_is_growing;
            g_frame_counter = 0;
        }

        glm::vec3 scale_vector = glm::vec3(g_is_growing ? GROWTH_FACTOR : SHRINK_FACTOR,
                                          g_is_growing ? GROWTH_FACTOR : SHRINK_FACTOR,
                                          3.0f);
        
        // update rotations
        g_stitch_rotation.y += DEGREES_PER_SECOND * delta_time;
        g_gudetama_rotation.z += DEGREES_PER_SECOND * delta_time;
    
        // calculate positions
        g_stitch_position.x = INIT_POS_STITCH.x + ORBIT_RADIUS * cos(ticks * ORBIT_SPEED);
        g_stitch_position.y = INIT_POS_STITCH.y + ORBIT_RADIUS * sin(ticks * ORBIT_SPEED);
       
        g_gudetama_position.x = INIT_POS_GUDETAMA.x + g_stitch_position.x + GUDETAMA_ORBIT_RADIUS * cos(ticks * GUDETAMA_ORBIT_SPEED);
        g_gudetama_position.y = INIT_POS_GUDETAMA.y + g_stitch_position.y + GUDETAMA_ORBIT_RADIUS * sin(ticks * GUDETAMA_ORBIT_SPEED);

        
        // reset model matrix
        g_stitch_matrix = glm::mat4(1.0f);
        g_gudetama_matrix = glm::mat4(1.0f);
        
        
        // transformations for both objects
        g_stitch_matrix = glm::translate(g_stitch_matrix, g_stitch_position);
        g_stitch_matrix = glm::rotate(g_stitch_matrix,
                               glm::radians(g_stitch_rotation.y),
                               glm::vec3(0.0f, 1.0f, 0.0f));
        g_stitch_matrix = glm::scale(g_stitch_matrix, INIT_SCALE);
        g_stitch_matrix = glm::scale(g_stitch_matrix, scale_vector);
        
        g_gudetama_matrix = glm::translate(g_gudetama_matrix, g_gudetama_position);
        g_gudetama_matrix = glm::rotate(g_gudetama_matrix,
                           glm::radians(g_gudetama_rotation.z),
                           glm::vec3(0.0f, 0.0f, 1.0f));
        g_gudetama_matrix = glm::scale(g_gudetama_matrix, INIT_SCALE);

    
}

void draw_object(glm::mat4& object_matrix, GLuint& object_texture_id) {
    g_shader_program.set_model_matrix(object_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
   
        float vertices[] = {
            -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
        };
    
        float texture_coordinates[] = {
            0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        };
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    
    draw_object(g_stitch_matrix, g_stitch_texture_id);
    draw_object(g_gudetama_matrix, g_gudetama_texture_id);
    
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }

/**
 Start here—we can see the general structure of a game loop without worrying too much about the details yet.
 */
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
