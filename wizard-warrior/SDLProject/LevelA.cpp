#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 18
#define LEVEL_HEIGHT 8

constexpr char ATTACK1_FILEPATH[] = "assets/Fire-Wizard/Attack_1.png",
              ATTACK2_FILEPATH[] = "assets/Fire-Wizard/Attack_2.png",
              DEATH_FILEPATH[] = "assets/Fire-Wizard/Dead.png",
              ATTACK3_FILEPATH[] = "assets/Fire-Wizard/Flame_jet.png",
              HURT_FILEPATH[] = "assets/Fire-Wizard/Hurt.png",
              STANDING_FILEPATH[] = "assets/Fire-Wizard/Idle.png",
              JUMPING_FILEPATH[] = "assets/Fire-Wizard/Jump.png",
              RUN_FILEPATH[] = "assets/Fire-Wizard/Run.png",
              PLATFORM_FILEPATH[] = "assets/platformPack_tile027.png";

constexpr char BACKGROUND_FILEPATH[] = "assets/Forest.jpg";

constexpr char ENEMY_ATTACK1_FILEPATH[] = "assets/Enemy1/Attack_1.png",
              ENEMY_ATTACK2_FILEPATH[] = "assets/Enemy1/Attack_2.png",
              ENEMY_DEATH_FILEPATH[] = "assets/Enemy1/Dead.png",
              ENEMY_ATTACK3_FILEPATH[] = "assets/Enemy1/Light_charge.png",
              ENEMY_HURT_FILEPATH[] = "assets/Enemy1/Hurt.png",
              ENEMY_STANDING_FILEPATH[] = "assets/Enemy1/Idle.png",
              ENEMY_JUMPING_FILEPATH[] = "assets/Enemy1/Jump.png",
              ENEMY_RUN_FILEPATH[] = "assets/Enemy1/Run.png";


unsigned int LEVEL_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};



LevelA::~LevelA()
{
    if (m_game_state.enemies != nullptr) {
        delete [] m_game_state.enemies;
        m_game_state.enemies = nullptr;
    }
    
    if (m_game_state.player != nullptr) {
        delete m_game_state.player;
        m_game_state.player = nullptr;
    }
    
    if (m_game_state.map != nullptr) {
        delete m_game_state.map;
        m_game_state.map = nullptr;
    }
    
    if (m_game_state.jump_sfx != nullptr) {
        Mix_FreeChunk(m_game_state.jump_sfx);
        m_game_state.jump_sfx = nullptr;
    }
    
    if (m_game_state.bgm != nullptr) {
        Mix_FreeMusic(m_game_state.bgm);
        m_game_state.bgm = nullptr;
    }
    
    if (m_game_state.player_attack_sfx != nullptr) {
        Mix_FreeChunk(m_game_state.player_attack_sfx);
        m_game_state.player_attack_sfx = nullptr;
    }
    
    if (m_game_state.player_attack3_sfx != nullptr) {
        Mix_FreeChunk(m_game_state.player_attack3_sfx);
        m_game_state.player_attack3_sfx = nullptr;
    }
    
    if (m_game_state.enemy_attack_sfx != nullptr) {
        Mix_FreeChunk(m_game_state.enemy_attack_sfx);
        m_game_state.enemy_attack_sfx = nullptr;
    }
    
    if (m_game_state.enemy_hurt_sfx != nullptr) {
        Mix_FreeChunk(m_game_state.enemy_hurt_sfx);
        m_game_state.enemy_hurt_sfx = nullptr;
    }
    
    if (m_game_state.player_hurt_sfx != nullptr) {
        Mix_FreeChunk(m_game_state.player_hurt_sfx);
        m_game_state.player_hurt_sfx = nullptr;
    }
}

void LevelA::initialise()
{
    m_number_of_enemies = ENEMY_COUNT;
    
    m_background_texture_id = Utility::load_texture(BACKGROUND_FILEPATH);
    
    GLuint map_texture_id = Utility::load_texture("assets/Mossy.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 7, 7);
    
    // ---- PLAYER INITIALIZATION ----
    
    // Load all animation textures for the player
    GLuint attack1_texture_id = Utility::load_texture(ATTACK1_FILEPATH);
    GLuint attack2_texture_id = Utility::load_texture(ATTACK2_FILEPATH);
    GLuint death_texture_id = Utility::load_texture(DEATH_FILEPATH);
    GLuint attack3_texture_id = Utility::load_texture(ATTACK3_FILEPATH);
    GLuint hurt_texture_id = Utility::load_texture(HURT_FILEPATH);
    GLuint standing_texture_id = Utility::load_texture(STANDING_FILEPATH);
    GLuint jumping_texture_id = Utility::load_texture(JUMPING_FILEPATH);
    GLuint run_texture_id = Utility::load_texture(RUN_FILEPATH);
    
    glm::vec3 player_acceleration = glm::vec3(0.0f, -4.81f, 0.0f);
    m_game_state.player = new Entity(PLAYER, 4.0f, player_acceleration, 4.0f, 1.0f, 1.0f);
    
    std::vector<int> attack1_frames = {0, 1, 2, 3};
    m_game_state.player->add_animation(ATTACK1, attack1_frames, 4, attack1_texture_id);
    
    std::vector<int> attack2_frames = {0, 1, 2, 3};
    m_game_state.player->add_animation(ATTACK2, attack2_frames, 4, attack2_texture_id);
    
    std::vector<int> death_frames = {0, 1, 2, 3, 4, 5};
    m_game_state.player->add_animation(DEATH, death_frames, 6, death_texture_id);
    
    std::vector<int> attack3_frames = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    m_game_state.player->add_animation(ATTACK3, attack3_frames, 14, attack3_texture_id);
    
    std::vector<int> hurt_frames = {0, 1, 2};
    m_game_state.player->add_animation(HURT, hurt_frames, 3, hurt_texture_id);
    
    std::vector<int> standing_frames = {0, 1, 2, 3, 4, 5, 6};
    m_game_state.player->add_animation(STANDING, standing_frames, 7, standing_texture_id);
    
    std::vector<int> jumping_frames = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    m_game_state.player->add_animation(JUMPING, jumping_frames, 9, jumping_texture_id);
    
    std::vector<int> run_frames = {0, 1, 2, 3, 4, 5, 6, 7};
    m_game_state.player->add_animation(RUN, run_frames, 8, run_texture_id);
    
    m_game_state.player->set_animation_state(STANDING);
    
    m_game_state.player->set_position(glm::vec3(3.0f, 5.0f, 0.0f));
    
    m_game_state.player->set_respawn_position(glm::vec3(3.0f, 5.0f, 0.0f));
    
    m_game_state.player->set_lives(10);
    
    
    // ---- ENEMY INITIALIZATION ----
    
    GLuint enemy_attack1_texture_id = Utility::load_texture(ENEMY_ATTACK1_FILEPATH);
    GLuint enemy_attack2_texture_id = Utility::load_texture(ENEMY_ATTACK2_FILEPATH);
    GLuint enemy_death_texture_id = Utility::load_texture(ENEMY_DEATH_FILEPATH);
    GLuint enemy_attack3_texture_id = Utility::load_texture(ENEMY_ATTACK3_FILEPATH);
    GLuint enemy_hurt_texture_id = Utility::load_texture(ENEMY_HURT_FILEPATH);
    GLuint enemy_standing_texture_id = Utility::load_texture(ENEMY_STANDING_FILEPATH);
    GLuint enemy_jumping_texture_id = Utility::load_texture(ENEMY_JUMPING_FILEPATH);
    GLuint enemy_run_texture_id = Utility::load_texture(ENEMY_RUN_FILEPATH);
    
    m_game_state.enemies = new Entity[m_number_of_enemies];
    
    glm::vec3 enemy_acceleration = glm::vec3(0.0f, -8.5f, 0.0f);
    
    for (int i = 0; i < m_number_of_enemies; i++) {
        m_game_state.enemies[i] = Entity(ENEMY, 2.5f, enemy_acceleration, 12.0f, 1.0f, 1.0f);
        
        m_game_state.enemies[i].set_ai_type(GUARD);
        m_game_state.enemies[i].set_ai_state(IDLE);
    
        std::vector<int> enemy_attack1_frames = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        m_game_state.enemies[i].add_animation(ATTACK1, enemy_attack1_frames, 10, enemy_attack1_texture_id);
        
        std::vector<int> enemy_attack2_frames = {0, 1, 2, 3};
        m_game_state.enemies[i].add_animation(ATTACK2, enemy_attack2_frames, 4, enemy_attack2_texture_id);
        
        std::vector<int> enemy_death_frames = {0, 1, 2, 3, 4};
        m_game_state.enemies[i].add_animation(DEATH, enemy_death_frames, 5, enemy_death_texture_id);
        
        std::vector<int> enemy_attack3_frames = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        m_game_state.enemies[i].add_animation(ATTACK3, enemy_attack3_frames, 12, enemy_attack3_texture_id);
        
        std::vector<int> enemy_hurt_frames = {0, 1, 2};
        m_game_state.enemies[i].add_animation(HURT, enemy_hurt_frames, 3, enemy_hurt_texture_id);
        
        std::vector<int> enemy_standing_frames = {0, 1, 2, 3, 4, 5, 6};
        m_game_state.enemies[i].add_animation(STANDING, enemy_standing_frames, 7, enemy_standing_texture_id);
        
        std::vector<int> enemy_jumping_frames = {0, 1, 2, 3, 4, 5, 6, 7};
        m_game_state.enemies[i].add_animation(JUMPING, enemy_jumping_frames, 8, enemy_jumping_texture_id);
        
        std::vector<int> enemy_run_frames = {0, 1, 2, 3, 4, 5, 6, 7};
        m_game_state.enemies[i].add_animation(RUN, enemy_run_frames, 8, enemy_run_texture_id);
        
        m_game_state.enemies[i].set_animation_state(STANDING);
        
        m_game_state.enemies[i].set_lives(5);
    }

    m_game_state.enemies[0].set_position(glm::vec3(14.0f, 5.0f, 0.0f));
    m_game_state.enemies[0].set_respawn_position(glm::vec3(14.0f, 5.0f, 0.0f));
    m_game_state.enemies[0].set_scale(glm::vec3(-1.0f, 1.0f, 1.0f));
    
    if (m_number_of_enemies > 1) {
        m_game_state.enemies[1].set_position(glm::vec3(13.0f, 5.0f, 0.0f));
        m_game_state.enemies[1].set_respawn_position(glm::vec3(13.0f, 5.0f, 0.0f));
        m_game_state.enemies[1].set_scale(glm::vec3(-1.0f, 1.0f, 1.0f));
    }

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/Battle-Conflict.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(20.0f);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/Jump.wav");
    m_game_state.player_attack_sfx = Mix_LoadWAV("assets/sword.wav");
    m_game_state.player_attack3_sfx = Mix_LoadWAV("assets/flame-sound.wav");
    m_game_state.enemy_attack_sfx = Mix_LoadWAV("assets/enemy attack.wav");
    m_game_state.player_hurt_sfx = Mix_LoadWAV("assets/hurt.wav");
    m_game_state.enemy_hurt_sfx = Mix_LoadWAV("assets/hurt.wav");
    
    m_game_state.level_completed = false;
    m_game_state.game_over = false;
}

void LevelA::update(float delta_time)
{
    // Store previous player state
    bool player_was_hurt = m_game_state.player->is_hurt();
    bool player_was_respawning = m_game_state.player->is_respawning();
    int player_previous_lives = m_game_state.player->get_lives();
        
    
    bool enemies_were_hurt[10];
    bool enemies_were_respawning[10];
    float enemies_previous_y[10];
    int enemies_previous_lives[10];
    
    for (int i = 0; i < m_number_of_enemies; i++) {
        enemies_were_hurt[i] = m_game_state.enemies[i].is_hurt();
        enemies_were_respawning[i] = m_game_state.enemies[i].is_respawning();
        enemies_previous_y[i] = m_game_state.enemies[i].get_position().y;
        enemies_previous_lives[i] = m_game_state.enemies[i].get_lives();
    }
    
    m_game_state.player->update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
    
    for (int i = 0; i < m_number_of_enemies; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
    }
    
    if (!player_was_hurt && m_game_state.player->is_hurt() && !player_was_respawning && !m_game_state.player->is_respawning()) {
        Mix_PlayChannel(-1, m_game_state.player_hurt_sfx, 0);
    }

    else if (!player_was_respawning && m_game_state.player->is_respawning() &&
             m_game_state.player->get_lives() < player_previous_lives) {
        Mix_PlayChannel(-1, m_game_state.player_hurt_sfx, 0);
    }
        
    for (int i = 0; i < m_number_of_enemies; i++) {
        if (!enemies_were_hurt[i] && m_game_state.enemies[i].is_hurt() &&
            !enemies_were_respawning[i] && !m_game_state.enemies[i].is_respawning()) {
            Mix_PlayChannel(-1, m_game_state.enemy_hurt_sfx, 0);
        }
        else if (!enemies_were_respawning[i] && m_game_state.enemies[i].is_respawning() &&
                 m_game_state.enemies[i].get_lives() < enemies_previous_lives[i]) {
            Mix_PlayChannel(-1, m_game_state.enemy_hurt_sfx, 0);
        }
    }
    
    if (m_game_state.player->is_attacking()) {
        for (int i = 0; i < m_number_of_enemies; i++) {
            if (m_game_state.player->check_attack_collision(&m_game_state.enemies[i])) {
                AnimationState current_anim = m_game_state.player->get_current_animation_state();
                if (current_anim == ATTACK3) {
                    Mix_PlayChannel(-1, m_game_state.player_attack3_sfx, 0);
                }
                else {
                    Mix_PlayChannel(-1, m_game_state.player_attack_sfx, 0);
                }
            }
        }
    }
        
    for (int i = 0; i < m_number_of_enemies; i++) {
        if (m_game_state.enemies[i].is_attacking()) {
            if (m_game_state.enemies[i].check_attack_collision(m_game_state.player)) {
                Mix_PlayChannel(-1, m_game_state.enemy_attack_sfx, 0);
            }
        }
    }
    
    m_game_state.player->constrain_to_map(LEVEL_WIDTH, LEVEL_HEIGHT);
    for (int i = 0; i < m_number_of_enemies; i++) {
        m_game_state.enemies[i].constrain_to_map(LEVEL_WIDTH, LEVEL_HEIGHT);
    }
    
    if (m_game_state.player->get_lives() <= 0) {
        m_game_state.game_over = true;
    }
    
    bool all_enemies_defeated = true;
    for (int i = 0; i < m_number_of_enemies; i++) {
        if (m_game_state.enemies[i].get_lives() > 0) {
            all_enemies_defeated = false;
            break;
        }
    }
        
    if (all_enemies_defeated) {
        m_game_state.level_completed = true;
    }
}



void LevelA::render(ShaderProgram *program)
{
    if (program == nullptr) return;
    GLuint current_program_id = program->get_program_id();
    
    static bool using_lighting = false;
    static GLuint default_program_id = 0;
    static GLuint lighting_program_id = 0;
    
    if (default_program_id == 0) {
        default_program_id = current_program_id;
    }
    else if (lighting_program_id == 0 && current_program_id != default_program_id) {
        lighting_program_id = current_program_id;
    }
    
   
    using_lighting = (lighting_program_id != 0 && current_program_id == lighting_program_id);
    
    glm::mat4 model_save = glm::mat4(1.0f);
    program->set_model_matrix(model_save);
    
    if (using_lighting && default_program_id != 0) {
        glUseProgram(default_program_id);
    }
    
   
    glm::mat4 bg_model_matrix = glm::mat4(1.0f);
    
   
    float parallax_x = m_game_state.player->get_position().x * 0.2f;
    
   
    bg_model_matrix = glm::translate(bg_model_matrix, glm::vec3(-parallax_x, 0.0f, 0.0f));
    
   
    bg_model_matrix = glm::scale(bg_model_matrix, glm::vec3(70.0f, 20.0f, 1.0f));
    
   
    if (using_lighting && default_program_id != 0) {
      
        GLuint model_matrix_uniform = glGetUniformLocation(default_program_id, "modelMatrix");
        glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, &bg_model_matrix[0][0]);
    } else {
      
        program->set_model_matrix(bg_model_matrix);
    }
    
   
    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };
    
   
    float tex_coords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };
    

    glBindTexture(GL_TEXTURE_2D, m_background_texture_id);
    
    GLuint position_attribute, tex_coord_attribute;
    
    if (using_lighting && default_program_id != 0) {
        position_attribute = glGetAttribLocation(default_program_id, "position");
        tex_coord_attribute = glGetAttribLocation(default_program_id, "texCoord");
    } else {
        position_attribute = program->get_position_attribute();
        tex_coord_attribute = program->get_tex_coordinate_attribute();
    }
    
    glVertexAttribPointer(position_attribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(position_attribute);
    
    glVertexAttribPointer(tex_coord_attribute, 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(tex_coord_attribute);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDisableVertexAttribArray(position_attribute);
    glDisableVertexAttribArray(tex_coord_attribute);
    
    if (using_lighting && current_program_id != 0) {
        glUseProgram(current_program_id);
    }
    
    program->set_model_matrix(model_save);
    
    // ---- GAME ELEMENTS RENDERING ----
    m_game_state.map->render(program);
    
    if (m_game_state.player != nullptr) {
        m_game_state.player->render(program);
    }
    
    if (m_game_state.enemies != nullptr) {
        for (int i = 0; i < m_number_of_enemies; i++) {
            m_game_state.enemies[i].render(program);
        }
    }
}
