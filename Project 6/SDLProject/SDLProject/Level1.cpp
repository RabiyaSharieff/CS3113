#include "Level1.h"

#define LEVEL1_WIDTH 18
#define LEVEL1_HEIGHT 17

#define LEVEL1_ENEMY_COUNT 1
#define LEVEL1_OBJECT_COUNT 2

unsigned int level1_data[] =
{
    70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 0, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
       70, 47, 47, 47, 47, 47, 47, 0, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 0, 47, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
       70, 47, 47, 47, 47, 70, 52, 70, 70, 70, 70, 71, 72, 47, 47, 47, 47, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
       70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
       70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70
};

Level1::Level1(int _lives) {
    state.player_lives = _lives;
}

void Level1::Initialize() {
    
    state.nextScene = -10;
    
  GLuint mapTextureID = Util::LoadTexture("MysticChroma_Basics.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 16, 16);
    
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
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    state.enemies[0].entityType = ENEMY;
    
    state.enemies[0].position = glm::vec3(9, -13, 0);
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].aiType = PATROL; 
    state.enemies[0].aiState = ACTIVE;
    
    state.enemies[0].textureID = Util::LoadTexture("purple_alien.png");
    state.enemies[0].height = 1.0f;
    state.enemies[0].width = 0.5f;
    
    state.enemies[0].movement = glm::vec3(0);
    state.enemies[0].speed = 1;
    
    
    // Initialize and Load Coins
    
    state.objects = new Entity[LEVEL1_OBJECT_COUNT];
    
    state.objects[0].position = glm::vec3(2, -10, 0);
    state.objects[0].textureID = Util::LoadTexture("coin.png");
    state.objects[0].height = 1.0f;
    state.objects[0].width = 0.5f;
    state.objects[0].entityType = COIN;
    
//    state.coins = new Entity[LEVEL1_COIN_COUNT];
//    state.coins[0].entityType = COIN;
//
//    state.coins[0].position = glm::vec3(2, -10, 0);
//
//    state.coins[0].textureID = Util::LoadTexture("coin.png");
//    state.coins[0].height = 1.0f;
//    state.coins[0].width = 0.5f;
    

}

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.map);
    
    
    
    if (state.objects[LEVEL1_OBJECT_COUNT-1].CheckCollision(&state.enemies[0])) {
        std::cout << "COLLISION\n";
        //Mix_PlayChannel(-1, sploosh, 0);
        loseLife();
        //state.objects[LEVEL1_OBJECT_COUNT - 1].previouslyCollided = false;
        state.nextScene = 1;
        }

    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.map);
        std::cout << "Enemy: (" << state.enemies[i].position.x << ", ";
        std::cout << state.enemies[0].position.y << ") \n";
    }
    
    for (int i = 0; i < LEVEL1_OBJECT_COUNT; i++) {
           state.objects[i].Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.map);
       }
    
//    int defeated = 0;
//    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
//        if (state.enemies[i].playerDefeated) {
//            defeated += 1;
//        }
//    }
//    
//    if (defeated == LEVEL1_ENEMY_COUNT) {
//        state.player->enemiesDefeated = true; // if all enemies are defeated
//    }
//    
//    if (state.player->enemiesDefeated == true) {
//        std::cout << "Yay defeated all enemies on level!";
//        //code to now allow going though portal
//    }
//    else if (state.player-> playerDefeated == true) {
//        std::cout << "Oh no enemy collison";
//        loseLife();
//        state.nextScene = 1;
//    }
//    
//    // coin logic
//    int collected = 0;
//    for (int i = 0; i < LEVEL1_OBJECT_COUNT; i++) {
//        if (state.objects[i].wasCollected) { //check to see that all coins were collected
//            collected += 1;
//        }
//    }
//        
//    if (collected == LEVEL1_OBJECT_COUNT) {
//        state.player->coinsCollected = true; // if all coins collected
//         std::cout << "All COINS collected on level!";
//    }
      
    
    // Jump into Portal here to move to next level
    if ((state.player->position.x >= 15.4) && (state.player->position.y >= -1.2)) {
        state.nextScene = 2;
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
    
    state.map->Render(program);
    state.player->Render(program);
    
    // render enemies
   for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
       state.enemies[i].Render(program);
   }
    
     // render coins
    for (int i = 0; i < LEVEL1_OBJECT_COUNT; i++) {
        state.objects[i].Render(program);
    }
    
}

