#include "Level3.h"

#define LEVEL3_WIDTH 18
#define LEVEL3_HEIGHT 17

#define LEVEL3_ENEMY_COUNT 3

unsigned int level3_data[] =
{
   70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 70, 52, 70, 70, 70, 70, 71, 72, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 0, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 0, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
   70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70
};

Level3::Level3(int _lives) {
    state.player_lives = _lives;
}

void Level3::Initialize() {
    state.nextScene = -10;
    
    GLuint mapTextureID = Util::LoadTexture("MysticChroma_Basics.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 16, 16);
    
    // Initialize and Load Player
    state.player = new Entity();
    state.player->entityType = PLAYER;

    state.player->position = glm::vec3(5, -13, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 5.0f;
    state.player->textureID = Util::LoadTexture("ninja_m.png");
    
    // Animate Player
    state.player->animRight = new int[3] {3, 4, 5};
    state.player->animLeft = new int[3] {9, 10, 11};
    state.player->animUp = new int[3] {0, 1, 2};
    state.player->animDown = new int[3] {6, 7, 8};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 3;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 3;
    state.player->animRows = 4;

    state.player->height = 0.8f;
    state.player->width = 0.35f;

    state.player->jumpPower = 7.0f;
    
    // Initialize and Load Enemies (aliens)
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
        
    GLuint enemyTextureID = Util::LoadTexture("purple_alien.png");;
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i ++) {
        state.enemies[i].entityType = ENEMY;
        
        if (i == 0) {
            state.enemies[i].position = glm::vec3(3.7, -3.5f, 0);
        }
        else if (i == 1) {
            state.enemies[i].position = glm::vec3(4.0f, -10.0f, 0);
        }
        else {
            state.enemies[i].position = glm::vec3(11.5f, -15.0f, 0);
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

void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    }
    
//    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
//
//         if(state.player->CheckCollision(&state.enemies[i])) {
//             if(state.enemies[i].topCollision) { // something not working here 
//                 state.enemies[i].isActive = false;
//             }
//             else {
//                 // player gets defeated by enemy
//                 loseLife();
//                 state.nextScene = 3;
//             }
//         }
//     }
    
    // Jump into Portal here to move to winner scene
    if ((state.player->position.x >= 7.5) && (state.player->position.y <= -10.2) && (state.player->position.x <= 8.5) && (state.player->position.y >= -11.2)) {
        state.nextScene = 4;
    }
    
    // lose life if player falls into pit
    else if (state.player->position.y < -15.5) {
        loseLife();
        if (state.player->lives == 0) {
            state.nextScene = 5;
        }
        else {
            state.nextScene = 3;
        }
    }
}

void Level3::Render(ShaderProgram *program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, "Level 3", 1.0f, -0.1f, glm::vec3(10, -12, 0));
    Util::DrawText(program, fontTextureID, "Watch out for the Aliens...",  0.4f, -0.2f, glm::vec3(10, -13, 0));
    Util::DrawText(program, fontTextureID, "Last One!",  0.4f, -0.2f, glm::vec3(11, 1.05, 0));
    
    std::string lives = std::to_string(state.player_lives);
    std::string lives_remaining = "Lives: " + lives;
    
    
    Util::DrawText(program, fontTextureID, lives_remaining, 0.4f, -0.2f, glm::vec3(15, 1, 0));
    
    
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}

