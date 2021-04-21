#include "Level1.h"

#define LEVEL1_WIDTH 18
#define LEVEL1_HEIGHT 17

#define LEVEL1_ENEMY_COUNT 1

unsigned int level1_data[] =
{
    19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0,
    19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 19, 19, 19, 19,
    19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 19,
    19, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 19,
    19, 0, 0, 0, 0, 0, 0, 0, 19, 19, 19, 0, 0, 0, 0, 0, 0, 19,
    19, 19, 19, 19, 0, 0, 0, 19, 19, 19, 19, 19, 19, 19, 0, 0, 0, 19,
    19, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0, 19,
    19, 0, 0, 19, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 19, 0, 19,
    19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 19, 0, 19, 19, 19, 0, 19,
    19, 19, 0, 0, 19, 19, 0, 19, 19, 19, 19, 19, 0, 0, 0, 0, 0, 19,
    19, 19, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19,
    19, 19, 19, 19, 19, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19,
    19, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 19, 0, 0, 19,
    19, 0, 0, 0, 0, 0, 0, 0, 0, 19, 19, 0, 0, 19, 19, 0, 0, 19,
    19, 0, 0, 0, 19, 19, 0, 0, 19, 19, 19, 0, 19, 19, 0, 0, 0, 19,
    19, 19, 19, 19, 19, 0, 0, 19, 19, 19, 19, 0, 0, 0, 0, 0, 0, 19,
    19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 0, 19, 19, 19, 0, 0, 19
};

Level1::Level1(int _lives) {
    state.player_lives = _lives;
}

void Level1::Initialize() {
    
    state.nextScene = -10;
    
    GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 14, 7);
    
    // Initialize and Load Player (George)
    state.player = new Entity();
    state.player->entityType = PLAYER;
    
    state.player->position = glm::vec3(6, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.5f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};
    
    // Animate george
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
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].position = glm::vec3(9, -9, 0);
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    
    state.enemies[0].textureID = Util::LoadTexture("purple_alien.png");
    state.enemies[0].height = 1.0f;
    state.enemies[0].width = 0.5f;
    
    state.enemies[0].movement = glm::vec3(0);
    state.enemies[0].speed = 1;
}

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    }
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {

        if(state.player->CheckCollision(&state.enemies[i])) {
            if(state.enemies[i].topCollision) { // something not working here 
                state.enemies[i].isActive = false;
            }
            else {
                // player gets defeated by enemy
                loseLife();
                state.nextScene = 1;
            }
        }
    }

    // Jump into Portal here to move to next level
    if ((state.player->position.x >= 15) && (state.player->position.y >= 0.5)) {
        state.nextScene = 2;
    }
    
    // lose life if player falls into pit
    else if (state.player->position.y < -15.5) {
        loseLife();
        if (state.player->lives == 0) {
            state.nextScene = 5;
        }
        else {
            state.nextScene = 1;
        }
    }
}

void Level1::Render(ShaderProgram *program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, "Level 1", 1.0f, -0.1f, glm::vec3(1.25, -13, 0));
    Util::DrawText(program, fontTextureID, "Climb Up!", 0.4f, -0.2f, glm::vec3(8.7, -10, 0));
    Util::DrawText(program, fontTextureID, "Portal ->", 0.4f, -0.2f, glm::vec3(3, -3, 0));
    Util::DrawText(program, fontTextureID, "Jump into the Portal!", 0.4f, -0.2f, glm::vec3(10, 0.5, 0));
    
    std::string lives = std::to_string(state.player_lives);
    std::string lives_remaining = "Lives: " + lives;
    
    Util::DrawText(program, fontTextureID, lives_remaining, 0.4f, -0.2f, glm::vec3(3, 1.0, 0));
    
    // render enemies
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
    
    state.map->Render(program);
    state.player->Render(program);
    
}

