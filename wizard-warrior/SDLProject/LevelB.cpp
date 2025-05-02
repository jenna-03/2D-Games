#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 30
#define LEVEL_HEIGHT 8
extern glm::mat4 g_view_matrix;

constexpr char ATTACK1_FILEPATH[] = "assets/Fire-Wizard/Attack_1.png",
              ATTACK2_FILEPATH[] = "assets/Fire-Wizard/Attack_2.png",
              DEATH_FILEPATH[] = "assets/Fire-Wizard/Dead.png",
              ATTACK3_FILEPATH[] = "assets/Fire-Wizard/Flame_jet.png",
              HURT_FILEPATH[] = "assets/Fire-Wizard/Hurt.png",
              STANDING_FILEPATH[] = "assets/Fire-Wizard/Idle.png",
              JUMPING_FILEPATH[] = "assets/Fire-Wizard/Jump.png",
              RUN_FILEPATH[] = "assets/Fire-Wizard/Run.png",
              PLATFORM_FILEPATH[] = "assets/platformPack_tile027.png";

constexpr char BACKGROUND_FILEPATH[] = "assets/blue.png";

constexpr char ENEMY2_ATTACK1_FILEPATH[] = "assets/Enemy2/Attack_1.png",
              ENEMY2_ATTACK2_FILEPATH[] = "assets/Enemy2/Attack_2.png",
              ENEMY2_DEATH_FILEPATH[] = "assets/Enemy2/Dead.png",
              ENEMY2_ATTACK3_FILEPATH[] = "assets/Enemy2/Magic_sphere.png",
              ENEMY2_HURT_FILEPATH[] = "assets/Enemy2/Hurt.png",
              ENEMY2_STANDING_FILEPATH[] = "assets/Enemy2/Idle.png",
              ENEMY2_JUMPING_FILEPATH[] = "assets/Enemy2/Jump.png",
              ENEMY2_RUN_FILEPATH[] = "assets/Enemy2/Run.png";


unsigned int LEVEL_B_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 23, 24, 0, 0, 22, 23, 23, 23, 23, 23, 23, 23, 23, 24, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 22, 23, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

LevelB::~LevelB()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
        
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
    Mix_FreeChunk(m_game_state.player_attack_sfx);
    Mix_FreeChunk(m_game_state.player_attack3_sfx);
    Mix_FreeChunk(m_game_state.enemy_attack_sfx);
    Mix_FreeChunk(m_game_state.enemy_hurt_sfx);
    Mix_FreeChunk(m_game_state.player_hurt_sfx);
}

void LevelB::initialise()
{
    m_number_of_enemies = ENEMY_COUNT;
    m_background_texture_id = Utility::load_texture(BACKGROUND_FILEPATH);
    
    GLuint map_texture_id = Utility::load_texture("assets/Mossy.png");
    
    bool direction_right = true;

    for(int y = 0; y < LEVEL_HEIGHT; ++y) {
        for(int x = 0; x < LEVEL_WIDTH; ++x) {
            unsigned int &tile = LEVEL_B_DATA[y * LEVEL_WIDTH + x];
            if (tile == 25) {
                MovingPlatform mp;
                mp.position = glm::vec3(x * 1.0f, -y * 1.0f, 0.0f);
                mp.left_x = mp.position.x - 1.0f;
                mp.right_x = mp.position.x + 1.0f;
                
                mp.speed = direction_right ? 1.0f : -1.0f;
                direction_right = !direction_right;
                mp.tile_idx = tile - 1;
                m_platforms.push_back(mp);

                tile = 0;
            }
        }
    }
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_B_DATA, map_texture_id, 1.0f, 7, 7);
    

    // ---- PLAYER INITIALIZATION ----
    
    
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

    m_game_state.player->set_position(glm::vec3(2.0f, 4.0f, 0.0f));
    
    m_game_state.player->set_respawn_position(glm::vec3(2.0f, 4.0f, 0.0f));
    
    m_game_state.player->set_lives(10);
    
    
    // ---- MAIN ENEMY INITIALIZATION ----
    
    GLuint enemy_attack1_texture_id = Utility::load_texture(ENEMY2_ATTACK1_FILEPATH);
    GLuint enemy_attack2_texture_id = Utility::load_texture(ENEMY2_ATTACK2_FILEPATH);
    GLuint enemy_death_texture_id = Utility::load_texture(ENEMY2_DEATH_FILEPATH);
    GLuint enemy_attack3_texture_id = Utility::load_texture(ENEMY2_ATTACK3_FILEPATH);
    GLuint enemy_hurt_texture_id = Utility::load_texture(ENEMY2_HURT_FILEPATH);
    GLuint enemy_standing_texture_id = Utility::load_texture(ENEMY2_STANDING_FILEPATH);
    GLuint enemy_jumping_texture_id = Utility::load_texture(ENEMY2_JUMPING_FILEPATH);
    GLuint enemy_run_texture_id = Utility::load_texture(ENEMY2_RUN_FILEPATH);
    
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    
    glm::vec3 enemy_acceleration = glm::vec3(0.0f, -8.5f, 0.0f);
    
    m_game_state.enemies[0] = Entity(ENEMY, 2.5f, enemy_acceleration, 12.0f, 1.0f, 1.0f);
    
    m_game_state.enemies[0].set_ai_type(GUARD);
    m_game_state.enemies[0].set_ai_state(IDLE);
    
    std::vector<int> enemy_attack1_frames = {0, 1, 2, 3, 4, 5, 6};
    m_game_state.enemies[0].add_animation(ATTACK1, enemy_attack1_frames, 7, enemy_attack1_texture_id);
    
    std::vector<int> enemy_attack2_frames = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    m_game_state.enemies[0].add_animation(ATTACK2, enemy_attack2_frames, 9, enemy_attack2_texture_id);
    
    std::vector<int> enemy_death_frames = {0, 1, 2, 3};
    m_game_state.enemies[0].add_animation(DEATH, enemy_death_frames, 4, enemy_death_texture_id);
    
    std::vector<int> enemy_attack3_frames = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    m_game_state.enemies[0].add_animation(ATTACK3, enemy_attack3_frames, 16, enemy_attack3_texture_id);
    
    std::vector<int> enemy_hurt_frames = {0, 1, 2, 3};
    m_game_state.enemies[0].add_animation(HURT, enemy_hurt_frames, 4, enemy_hurt_texture_id);
    
    std::vector<int> enemy_standing_frames = {0, 1, 2, 3, 4, 5, 6, 7};
    m_game_state.enemies[0].add_animation(STANDING, enemy_standing_frames, 8, enemy_standing_texture_id);
    
    std::vector<int> enemy_jumping_frames = {0, 1, 2, 3, 4, 5, 6, 7};
    m_game_state.enemies[0].add_animation(JUMPING, enemy_jumping_frames, 8, enemy_jumping_texture_id);
    
    std::vector<int> enemy_run_frames = {0, 1, 2, 3, 4, 5, 6, 7};
    m_game_state.enemies[0].add_animation(RUN, enemy_run_frames, 8, enemy_run_texture_id);
    
    m_game_state.enemies[0].set_animation_state(STANDING);
    
    m_game_state.enemies[0].set_position(glm::vec3(22.0f, 6.0f, 0.0f));
    m_game_state.enemies[0].set_respawn_position(glm::vec3(22.0f, 6.0f, 0.0f));
    m_game_state.enemies[0].set_scale(glm::vec3(-1.0f, 1.0f, 1.0f));
    m_game_state.enemies[0].set_lives(5);

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

void LevelB::update_camera(float delta_time)
{
   
    glm::vec3 player_position = m_game_state.player->get_position();
    glm::vec3 camera_position = glm::vec3(player_position.x, player_position.y, 0.0f);
    
    float current_zoom = 1.0f;
    float visible_width = 10.0f / current_zoom;
    float half_visible_width = visible_width / 2.0f;
    
    float left_boundary = half_visible_width;
    float right_boundary = LEVEL_WIDTH - half_visible_width;
    
    if (camera_position.x < left_boundary) {
        camera_position.x = left_boundary;
    }
    if (camera_position.x > right_boundary) {
        camera_position.x = right_boundary;
    }
    
    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-camera_position.x, 3.75f, 0.0f));
}

void LevelB::check_platform_collisions() {
    Entity* player = m_game_state.player;
    float player_y_bottom = player->get_position().y - (player->get_height() / 2.0f);
    
    for (auto &mp : m_platforms) {
        float platform_x = mp.position.x;
        float platform_y = mp.position.y;
        float platform_width = 1.0f;
        float platform_height = 1.0f;
        
        float player_x = player->get_position().x;
        float player_y = player->get_position().y;
        float player_width = player->get_width();
        float player_height = player->get_height();
        
        bool x_overlap = fabs(player_x - platform_x) < (player_width + platform_width) / 2.0f;
        bool landing_on_platform = player->get_velocity().y < 0 &&
                                   player_y > platform_y &&
                                   player_y_bottom <= platform_y + (platform_height / 2.0f) &&
                                   player_y_bottom >= platform_y - (platform_height / 2.0f);
        
        if (x_overlap && landing_on_platform) {
            player->set_position(glm::vec3(player_x, platform_y + (platform_height / 2.0f) + (player_height / 2.0f), 0.0f));
            player->set_velocity(glm::vec3(player->get_velocity().x, 0, 0));
            
            float new_x = player_x + (mp.speed * 0.016666f);
            player->set_position(glm::vec3(new_x, player->get_position().y, 0.0f));
            player->set_collided_bottom_true();
        }
    }
}

void LevelB::update_light_position() {
    if (m_current_program == nullptr) return;
    
    glm::vec3 player_position = m_game_state.player->get_position();
    m_current_program->set_light_position(glm::vec2(player_position.x, player_position.y));
    
}

void LevelB::update(float delta_time)
{

    for (auto &mp : m_platforms) {
        mp.position.x += mp.speed * delta_time;
        if (mp.position.x < mp.left_x || mp.position.x > mp.right_x) {
            mp.speed = -mp.speed;
        }
    }


    bool player_was_hurt = m_game_state.player->is_hurt();
    bool player_was_respawning = m_game_state.player->is_respawning();
    int player_previous_lives = m_game_state.player->get_lives();
        
    bool enemy_was_hurt = m_game_state.enemies[0].is_hurt();
    bool enemy_was_respawning = m_game_state.enemies[0].is_respawning();
    int enemy_previous_lives = m_game_state.enemies[0].get_lives();
    
    m_game_state.player->update(delta_time, m_game_state.player, m_moving_platforms, m_platform_count, m_game_state.map);
    check_platform_collisions();
    
    m_game_state.enemies[0].update(delta_time, m_game_state.player, m_moving_platforms, m_platform_count, m_game_state.map);
    
    if (!player_was_hurt && m_game_state.player->is_hurt() && !player_was_respawning && !m_game_state.player->is_respawning()) {
        Mix_PlayChannel(-1, m_game_state.player_hurt_sfx, 0);
    }
   
    else if (!player_was_respawning && m_game_state.player->is_respawning() &&
             m_game_state.player->get_lives() < player_previous_lives) {
        Mix_PlayChannel(-1, m_game_state.player_hurt_sfx, 0);
    }
        

    if (!enemy_was_hurt && m_game_state.enemies[0].is_hurt() &&
        !enemy_was_respawning && !m_game_state.enemies[0].is_respawning()) {
        Mix_PlayChannel(-1, m_game_state.enemy_hurt_sfx, 0);
    }
   
    else if (!enemy_was_respawning && m_game_state.enemies[0].is_respawning() &&
             m_game_state.enemies[0].get_lives() < enemy_previous_lives) {
        Mix_PlayChannel(-1, m_game_state.enemy_hurt_sfx, 0);
    }
    
   
    if (m_game_state.player->is_attacking()) {
        if (m_game_state.player->check_attack_collision(&m_game_state.enemies[0])) {
            AnimationState current_anim = m_game_state.player->get_current_animation_state();
            if (current_anim == ATTACK3) {
                Mix_PlayChannel(-1, m_game_state.player_attack3_sfx, 0);
            }
            else {
                Mix_PlayChannel(-1, m_game_state.player_attack_sfx, 0);
            }
        }
    }
        
    if (m_game_state.enemies[0].is_attacking()) {
        if (m_game_state.enemies[0].check_attack_collision(m_game_state.player)) {
            Mix_PlayChannel(-1, m_game_state.enemy_attack_sfx, 0);
        }
    }
    
    m_game_state.player->constrain_to_map(LEVEL_WIDTH, LEVEL_HEIGHT);
    m_game_state.enemies[0].constrain_to_map(LEVEL_WIDTH, LEVEL_HEIGHT);
    
    if (m_game_state.player->get_lives() <= 0) {
        m_game_state.game_over = true;
    }
    
    
    bool main_enemy_defeated = (m_game_state.enemies[0].get_lives() <= 0);
    if (main_enemy_defeated) {
        m_game_state.level_completed = true;
    }
    
    update_camera(delta_time);

    if (m_current_program != nullptr) {
        update_light_position();
    }
}
void LevelB::drawMovingPlatforms(ShaderProgram *program) {
    const int   cols = 7, rows = 7;
    const float uSize = 1.0f/cols, vSize = 1.0f/rows;

    
    glm::vec4 current_tint(0.2f, 0.5f, 1.0f, 1.0f);
    program->set_tint_color(current_tint.r, current_tint.g, current_tint.b, current_tint.a);
    glUseProgram(program->get_program_id());

    for (auto &mp : m_platforms) {
        int idx = mp.tile_idx;
        float u = (idx % cols) * uSize;
        float v = (idx / cols) * vSize;

        // quad centered at origin, scaled to tile size
        float half = 0.5f;
        float verts[] = {
            -half,  half,   half, -half,  -half, -half,
            -half,  half,   half,  half,   half, -half
        };
        float tex[] = {
            u,        v+vSize,   u+uSize, v,    u,      v,
            u,        v+vSize,   u+uSize, v+vSize, u+uSize, v
        };

        glm::mat4 M = glm::translate(glm::mat4(1.0f), mp.position);
        M = glm::scale(M, glm::vec3(1.0f, -1.0f, 1.0f));
        program->set_model_matrix(M);
        program->set_tint_color(current_tint.r, current_tint.g, current_tint.b, current_tint.a);

        glBindTexture(GL_TEXTURE_2D, m_game_state.map->get_texture_id());
        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, GL_FALSE, 0, verts);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, GL_FALSE, 0, tex);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    }

    
    program->set_model_matrix(glm::mat4(1.0f));
}
void LevelB::render(ShaderProgram *program)
{
    m_current_program = program;
    
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
    
    if (using_lighting) {
        update_light_position();
    }

    glm::mat4 consistent_proj_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    program->set_projection_matrix(consistent_proj_matrix);
    glm::mat4 model_save = glm::mat4(1.0f);
    program->set_model_matrix(model_save);
    glm::mat4 view_save = g_view_matrix;
    
    GLuint current_program = program->get_program_id();

    if (using_lighting) {
        glUseProgram(default_program_id);
    }

    if (using_lighting) {
        GLuint tint_uniform = glGetUniformLocation(default_program_id, "tintColor");
        glUniform4f(tint_uniform, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    else {
        program->set_tint_color(1.0f, 1.0f, 1.0f, 1.0f);
    }

    glm::mat4 bg_view_matrix = glm::mat4(1.0f);
    glm::mat4 bg_proj_matrix = glm::ortho(-10.0f, 10.0f, -7.5f, 7.5f, -1.0f, 1.0f);

    glm::mat4 bg_model_matrix = glm::mat4(1.0f);

    // Use player's position for parallax (multiply by a small factor for subtle effect)
    float parallax_x = 0.0f;
    if (m_game_state.player != nullptr) {
        parallax_x = m_game_state.player->get_position().x * 0.2f;
    }

    bg_model_matrix = glm::translate(bg_model_matrix, glm::vec3(-parallax_x, 0.0f, 0.0f));
    bg_model_matrix = glm::scale(bg_model_matrix, glm::vec3(40.0f, 15.0f, 1.0f));

    if (using_lighting) {
        GLuint model_matrix_uniform = glGetUniformLocation(default_program_id, "modelMatrix");
        GLuint view_matrix_uniform = glGetUniformLocation(default_program_id, "viewMatrix");
        GLuint proj_matrix_uniform = glGetUniformLocation(default_program_id, "projectionMatrix");
        
        glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, &bg_model_matrix[0][0]);
        glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, &bg_view_matrix[0][0]);
        glUniformMatrix4fv(proj_matrix_uniform, 1, GL_FALSE, &bg_proj_matrix[0][0]);
    }
    
    else{
        program->set_model_matrix(bg_model_matrix);
        program->set_view_matrix(bg_view_matrix);
        program->set_projection_matrix(bg_proj_matrix);
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

    if (using_lighting) {
        position_attribute = glGetAttribLocation(default_program_id, "position");
        tex_coord_attribute = glGetAttribLocation(default_program_id, "texCoord");
    }
    
    else {
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

    if (using_lighting) {
        glUseProgram(current_program);
    }

    if (!using_lighting) {
        program->set_view_matrix(view_save);
        program->set_projection_matrix(consistent_proj_matrix);
    }
    else {
        program->set_projection_matrix(consistent_proj_matrix);
        program->set_view_matrix(view_save);
    }
    
    // ---- GAME ELEMENTS RENDERING ----
    program->set_tint_color(0.2f, 0.5f, 1.0f, 1.0f);
    m_game_state.map->render(program);
    drawMovingPlatforms(program);
    program->set_tint_color(1.0f, 1.0f, 1.0f, 1.0f);
    m_game_state.player->render(program);
    
    if (m_game_state.enemies[0].is_active()) {
        m_game_state.enemies[0].render(program);
    }
}
