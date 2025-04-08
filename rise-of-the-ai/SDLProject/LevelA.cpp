/**
* Author: Jenna Nandlall
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 42  
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH[] = "assets/Owlet_Monster_Walk_6.png",
           PLATFORM_FILEPATH[]    = "assets/platformPack_tile027.png",
           ENEMY_FILEPATH[]       = "assets/cyborg.png",
           ENEMY2_FILEPATH[]      = "assets/punk.png",
           FONT_FILEPATH[]        = "assets/font1.png",
           BACKGROUND_FILEPATH[]  = "assets/blue.png";

unsigned int LEVEL_A_DATA[] =
{
    17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 24, 0, 0, 0, 0, 0, 0, 0, 24, 24, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 24, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 24, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6,
    17, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 0, 6, 6, 6, 0, 0, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 6, 6, 6, 15, 15, 15, 15, 15,
    17, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 0, 0, 15, 15, 15, 0, 0, 0, 0, 6, 15, 15, 15, 15, 0, 0, 0, 0, 0, 6, 15, 15, 15, 15, 15, 15, 15, 15,
    17, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeChunk(m_game_state.hurt_sfx);
    Mix_FreeChunk(m_game_state.win_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelA::initialise()
{
    
    ENEMY_COUNT = 5;
   
    m_game_state.lives = 3;
    m_game_state.game_over = false;
    
    
    m_player_hurt = false;
    m_hurt_animation_time = 0.0f;
    m_blink_count = 0;
    
   
    m_level_complete = false;
    
    // Load background texture
    m_background_texture_id = Utility::load_texture(BACKGROUND_FILEPATH);
    m_background_width = 42.0f;
    m_background_height = 8.0f;
    
    
    m_normal_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    m_hurt_texture_id = Utility::load_texture("assets/Owlet_Monster_Hurt_4.png");
    
    GLuint map_texture_id = Utility::load_texture("assets/world_tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_A_DATA, map_texture_id, 1.0f, 9, 3);
    
    GLuint player_texture_id = m_normal_texture_id;

    
    int player_walking_animation[4][6] =
    {
        { 0, 1, 2, 3, 4, 5 },  // LEFT - all 6 frames for normal walking
        { 0, 1, 2, 3, 4, 5 },  // RIGHT
        { 0, 1, 2, 3, 4, 5 },  // UP
        { 0, 1, 2, 3, 4, 5 }   // DOWN
    };

    int hurt_walking_animation[4][6] =
    {
        { 1, 1, 1, 1, 1, 1 },  // LEFT
        { 1, 1, 1, 1, 1, 1 },  // RIGHT
        { 1, 1, 1, 1, 1, 1 },  // UP
        { 1, 1, 1, 1, 1, 1 }   // DOWN
    };
    
    // Store animation indices for later use
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++) {
            m_normal_walking[i][j] = player_walking_animation[i][j];
            m_hurt_walking[i][j] = hurt_walking_animation[i][j];
        }
    }

    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);

    // For player:
    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        4.5f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        6,                         // animation frame amount
        0,                         // current animation index
        6,                         // animation column amount
        1,                         // animation row amount
        1.0f,                      // width
        1.0f,                      // height
        PLAYER
    );
    
    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    m_game_state.player->set_jumping_power(4.5f);
    
    // Enemies' setup
    GLuint cyborg_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    GLuint punk_texture_id = Utility::load_texture(ENEMY2_FILEPATH);

    int enemy_walking_animation[4][6] = {
        { 0, 1, 2, 3, 4, 5 },
        { 0, 1, 2, 3, 4, 5 },
        { 0, 1, 2, 3, 4, 5 },
        { 0, 1, 2, 3, 4, 5 }
    };

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        GLuint enemy_texture_id = (i % 2 == 0) ? cyborg_texture_id : punk_texture_id;
        m_game_state.enemies[i] = Entity(
            enemy_texture_id,
            1.2f,
            glm::vec3(0.0f, -9.81f, 0.0f),
            0.0f,
            enemy_walking_animation,
            0.0f,
            6,
            0,
            6,
            1,
            1.5f,
            1.5f,
            ENEMY
        );
        
        m_game_state.enemies[i].set_movement(glm::vec3(-1.0f, 0.0f, 0.0f));
        m_game_state.enemies[i].set_facing_left(true);
        m_game_state.enemies[i].activate();
    }

    m_game_state.enemies[0].set_position(glm::vec3(2.0f, 0.75f, 0.0f));
    m_game_state.enemies[0].set_patrol_point(glm::vec3(8.0f, 0.75f, 0.0f));
    m_game_state.enemies[0].set_ai_type(GUARD);
    m_game_state.enemies[0].set_ai_state(IDLE);
    m_game_state.enemies[0].set_speed(1.0f);
    
    m_game_state.enemies[1].set_position(glm::vec3(11.0f, 5.75f, 0.0f));
    m_game_state.enemies[1].set_patrol_point(glm::vec3(11.0f, 5.75f, 0.0f));
    m_game_state.enemies[1].set_ai_type(WALKER);
    m_game_state.enemies[1].set_ai_state(WALKING);
    
    m_game_state.enemies[2].set_position(glm::vec3(18.0f, 5.75f, 0.0f));
    m_game_state.enemies[2].set_patrol_point(glm::vec3(18.0f, 5.75f, 0.0f));
    m_game_state.enemies[2].set_ai_type(WALKER);
    m_game_state.enemies[2].set_ai_state(WALKING);
    
    m_game_state.enemies[3].set_position(glm::vec3(25.0f, 3.75f, 0.0f));
    m_game_state.enemies[3].set_patrol_point(glm::vec3(25.0f, 3.75f, 0.0f));
    m_game_state.enemies[3].set_ai_type(WALKER);
    m_game_state.enemies[3].set_ai_state(WALKING);
    
    m_game_state.enemies[4].set_position(glm::vec3(38.0f, 4.75f, 0.0f));
    m_game_state.enemies[4].set_patrol_point(glm::vec3(38.0f, 4.75f, 0.0f));
    m_game_state.enemies[4].set_ai_type(GUARD);
    m_game_state.enemies[4].set_ai_state(IDLE);
    m_game_state.enemies[4].set_speed(1.2f);

    m_number_of_enemies = ENEMY_COUNT;

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/time_for_adventure.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(64);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/jump.wav");
    m_game_state.hurt_sfx = Mix_LoadWAV("assets/hurt.wav");
    m_game_state.win_sfx = Mix_LoadWAV("assets/win.wav");
    
    m_last_injury_time = 0.0f;
    m_has_size_getters = true;
}

// Helper functions to switch sprites
void LevelA::switch_to_hurt_sprite() {
    int current_direction = m_game_state.player->is_facing_left() ? LEFT : RIGHT;
    m_game_state.player->set_texture_id(m_hurt_texture_id);
    m_game_state.player->set_walking(m_hurt_walking);
    m_game_state.player->set_animation_cols(4);
    m_game_state.player->set_animation_rows(1);
    m_game_state.player->set_animation_index(0);
    if (current_direction == LEFT)
        m_game_state.player->face_left();
    else
        m_game_state.player->face_right();
}

void LevelA::switch_to_normal_sprite() {
    int current_direction = m_game_state.player->is_facing_left() ? LEFT : RIGHT;
    m_game_state.player->set_texture_id(m_normal_texture_id);
    m_game_state.player->set_walking(m_normal_walking);
    m_game_state.player->set_animation_cols(6);
    m_game_state.player->set_animation_rows(1);
    m_game_state.player->set_animation_index(0);
    if (current_direction == LEFT)
        m_game_state.player->face_left();
    else
        m_game_state.player->face_right();
}

void LevelA::update(float delta_time)
{
    if (m_game_state.game_over) return;
    
    if (m_player_hurt) {
        m_hurt_animation_time += delta_time;
        if (m_hurt_animation_time >= BLINK_DURATION) {
            m_hurt_animation_time = 0.0f;
            m_blink_count++;
            if (m_blink_count % 2 == 0)
                switch_to_normal_sprite();
            else
                switch_to_hurt_sprite();
            
            if (m_blink_count >= NUM_BLINKS * 2) {
                m_player_hurt = false;
                m_blink_count = 0;
                switch_to_normal_sprite();
            }
        }
    }
    
    glm::vec3 old_position = m_game_state.player->get_position();
    m_game_state.player->update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].ai_activate(m_game_state.player);
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
    }
    
    float current_time = (float)SDL_GetTicks() / 1000.0f;
    
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
    
        if (m_game_state.player->check_collision(&m_game_state.enemies[i]))
        {
            if (current_time - m_last_injury_time > INVINCIBILITY_TIME) {
                m_game_state.lives--;
                m_last_injury_time = current_time;
                m_player_hurt = true;
                m_hurt_animation_time = 0.0f;
                m_blink_count = 1;
                switch_to_hurt_sprite();
                Mix_PlayChannel(-1, m_game_state.hurt_sfx, 0);
                if (m_game_state.lives <= 0)
                    m_game_state.game_over = true;
            }
            m_game_state.player->set_position(old_position);
        }
    }
    
    if (!m_level_complete && m_game_state.player->get_position().y < -10.0f) {
        m_level_complete = true;
        Mix_PlayChannel(-1, m_game_state.win_sfx, 0);
        // TODO: Add next level transition logic here.
    }
}
void LevelA::render(ShaderProgram *g_shader_program)
{
    
    glm::mat4 model_matrix = glm::mat4(1.0f);
    
    
    model_matrix = glm::translate(model_matrix, glm::vec3(21.0f, -4.0f, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(70.0f, 12.0f, 1.0f)); // Full level size
    
    g_shader_program->set_model_matrix(model_matrix);
    
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
    
    glVertexAttribPointer(g_shader_program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program->get_position_attribute());
    glVertexAttribPointer(g_shader_program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(g_shader_program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(g_shader_program->get_position_attribute());
    glDisableVertexAttribArray(g_shader_program->get_tex_coordinate_attribute());
    
    
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < m_number_of_enemies; i++)
        m_game_state.enemies[i].render(g_shader_program);
}
