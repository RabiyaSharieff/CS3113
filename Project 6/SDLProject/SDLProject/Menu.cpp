#include "Menu.h"

#define MENU_WIDTH 18
#define MENU_HEIGHT 17

unsigned int menu_data[] =
{
    70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,
    70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
    70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
    70, 47, 47, 47, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 70,
    70, 47, 47, 47, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 70,
    70, 47, 47, 47, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 70,
    70, 47, 47, 47, 47, 47, 47, 47, 47, 0, 0, 0, 0, 0, 0, 0, 47, 70,
    70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
    70, 47, 47, 47, 47, 70, 52, 70, 70, 70, 70, 71, 72, 0, 0, 0, 47, 70,
    70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 0, 0, 0, 47, 70,
    70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 70,
    70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 70,
    70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 70,
    70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 70,
    70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
    70, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 70,
    70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70
};

void Menu::Initialize() {
    
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("MysticChroma_Basics.png");
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 16, 16);

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
}

void Menu::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.objects, 0, state.map);
    //state.player->Update(deltaTime, state.player, state.coins, 0, state.map);
    
}

void Menu::Render(ShaderProgram *program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, "Mars Invasion", 0.6f, -0.1f, glm::vec3(2, -10, 0));
    
    Util::DrawText(program, fontTextureID, "Use Gravity to Climb and find the Portals", 0.4f, -0.21f, glm::vec3(1.1, -11.1, 0));
    Util::DrawText(program, fontTextureID, "Use Space + Arrows", 0.4f, -0.2f, glm::vec3(3.1, -11.8, 0));
        
    Util::DrawText(program, fontTextureID, "Press Enter to Start", 0.5f, -0.1f, glm::vec3(1.1, -13, 0));
    
    state.map->Render(program);
    state.player->Render(program);
}

