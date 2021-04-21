#include "Level2.h"

#define LEVEL2_WIDTH 18
#define LEVEL2_HEIGHT 17

#define LEVEL2_ENEMY_COUNT 2

unsigned int level2_data[] =
{
    21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 21,
    21, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 21, 21, 21, 0, 21,
    21, 0, 0, 0, 21, 0, 0, 0, 21, 0, 21, 21, 0, 0, 21, 21, 0, 21,
    21, 0, 0, 21, 21, 21, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 21,
    21, 21, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21,
    21, 21, 21, 0, 0, 0, 0, 21, 21, 21, 21, 0, 21, 0, 0, 0, 0, 21,
    21, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 0, 21,
    21, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 21, 21, 21, 0, 21,
    21, 0, 0, 0, 0, 0, 0, 21, 0, 0, 21, 21, 0, 21, 21, 21, 0, 21,
    21, 0, 0, 0, 0, 21, 21, 0, 0, 21, 21, 0, 0, 0, 0, 0, 0, 21,
    21, 21, 21, 21, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 21,
    21, 0, 0, 0, 0, 21, 21, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21,
    21, 0, 0, 0, 21, 0, 0,21, 0, 0, 0, 0, 0, 0, 21, 0, 21, 21,
    21, 0, 0, 21, 0, 0, 0, 0, 0, 21, 0, 0, 0, 21, 21, 21, 21, 21,
    21, 21, 0, 0, 0, 0, 0, 0, 21, 21, 21, 0, 21, 21, 0, 0, 0, 21,
    21, 21, 21, 0, 0, 0, 0, 21, 21, 0, 0, 0, 0, 0, 0, 0, 0, 21,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 0, 21, 21, 21, 0, 0, 21
};

Level2::Level2(int _lives) {
    state.player_lives = _lives;
}

void Level2::Initialize() {
    state.nextScene = -10;
    
    GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 14, 7);
    
    // Initialize and Load Player (George)
    state.player = new Entity();
    state.player->entityType = PLAYER;
    
    state.player->position = glm::vec3(15.85, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.5f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
    // Animate george
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f;
    state.player->width = 0.35f;
    
    state.player->jumpPower = 7.0f;
    
    // Initialize and Load Enemies (aliens)
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
        
    GLuint enemyTextureID = Util::LoadTexture("purple_alien.png");;
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i ++) {
        state.enemies[i].entityType = ENEMY;
        
        if (i == 0) {
            state.enemies[i].position = glm::vec3(11.0f, -7.0f, 0);
        }
        else if (i == 1) {
            state.enemies[i].position = glm::vec3(1.5f, -9.0f, 0);
        }
        
        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
        state.enemies[i].aiType = WAITANDGO;
        state.enemies[i].aiState = IDLE;
        
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].height = 1.0f;
        state.enemies[i].width = 0.5f;
        
        state.enemies[i].movement = glm::vec3(0);
        state.enemies[i].speed = 1;
    }
}

void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    }
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {

         if(state.player->CheckCollision(&state.enemies[i])) {
             if(state.enemies[i].topCollision) { //something not working here 
                 state.enemies[i].isActive = false;
             }
             else {
                 // player gets defeated by enemy
                 loseLife();
                 state.nextScene = 2;
             }
         }
     }
    
    // Jump into Portal here to move to next level
    if ((state.player->position.x >= 14) && (state.player->position.y >= 0.5)) {
        state.nextScene = 3;
    }
    
    // lose life if player falls into pit
    else if (state.player->position.y < -15.5) {
        loseLife();
        if (state.player->lives == 0) {
            state.nextScene = 5;
        }
        else {
            state.nextScene = 2;
        }
    }
}

void Level2::Render(ShaderProgram *program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, "Level 2", 1.0f, -0.1f, glm::vec3(9.5, -10.5, 0));
    Util::DrawText(program, fontTextureID, "Climb Up! Again...", 0.4f, -0.2f, glm::vec3(9.4, -11.3, 0));
    Util::DrawText(program, fontTextureID, "Portal ->", 0.4f, -0.2f, glm::vec3(1, -1.5, 0));
    Util::DrawText(program, fontTextureID, "Portal!", 0.4f, -0.2f, glm::vec3(11.5, 1.0, 0));
    
    std::string lives = std::to_string(state.player_lives);
    std::string lives_remaining = "Lives: " + lives;
       
    
    Util::DrawText(program, fontTextureID, lives_remaining, 0.4f, -0.2f, glm::vec3(15, 1, 0));
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
    state.map->Render(program);
    state.player->Render(program);
}

