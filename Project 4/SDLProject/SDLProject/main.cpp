#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include <vector>

#define PLATFORM_COUNT 28
#define ENEMY_COUNT 3

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *enemies;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Mars Invader", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl"); // loading textured polygon shader
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f); // purple-blue background color
    glEnable(GL_BLEND);

    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Initialize and Load Platform Tiles //
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("platformPack_tile018.png");
    
    int x = 0;
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        
        if (i < 10) { // base platform
            state.platforms[i].position = glm::vec3(-4.5 + i, -3.25, 0);
        }
        // right platforms
        else if (i < 13) {
            x = i - 3;
            state.platforms[i].position = glm::vec3(-4.5 + x, -2.25, 0);
        }
        else if (i < 15) {
            x = i - 5;
            state.platforms[i].position = glm::vec3(-4.5 + x, -1.25, 0);
        }
        else if (i < 16) {
            x = i - 6;
            state.platforms[i].position = glm::vec3(-4.5 + x, -0.25, 0);
        }
        // left platforms
        else if (i < 19) {
           x = i - 16;
           state.platforms[i].position = glm::vec3(-4.5 +x, -2.25, 0);
        }
        else if (i < 21) {
           x = i - 19;
           state.platforms[i].position = glm::vec3(-4.5 +x, -1.25, 0);
        }
        else if (i < 22) {
           x = i - 21;
           state.platforms[i].position = glm::vec3(-4.5+x, -0.25, 0);
        }
        // middle platform
        else {
            x = i - 22;
            state.platforms[i].position = glm::vec3(-2.5 + x, 0.75, 0);
        }
        
        state.platforms[i].Update(0, NULL, NULL, 0, NULL, NULL);
    }
    
    // Initialization and Load Player (Astronaut) //
    state.player = new Entity();
    state.player->entityType = PLAYER;

    GLuint playerTextureID = LoadTexture("astronaut.png");
    state.player->textureID = playerTextureID;
    state.player->height = 1.0f;
    state.player->width = 0.5f;

    state.player->position = glm::vec3(3.5, -0.5, 0); // start position
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -2.81f, 0);
    state.player->speed = 3.0f;
    state.player->jumpPower = 3.0f;
    
    // Initializeand Load  Enemies (Aliens) //
    state.enemies = new Entity[ENEMY_COUNT];
       
    GLuint enemyTextureID;
    for (int i = 0; i < ENEMY_COUNT; i ++) {
        state.enemies[i].entityType = ENEMY;
        
        if (i == 0) {
            enemyTextureID = LoadTexture("green_alien.png");
            state.enemies[i].position = glm::vec3(-2.5, 2.75f, 0);
            state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
            state.enemies[i].aiType = PATROL;
            state.enemies[i].aiState = ACTIVE;
        }
        
        else if (i == 1) {
            enemyTextureID = LoadTexture("blue_alien.png");
            state.enemies[i].position = glm::vec3(-4.5, 1.75f, 0);
            state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
            state.enemies[i].aiType = WAITANDGO;
            state.enemies[i].aiState = IDLE;
        }
        
        else {
            enemyTextureID = LoadTexture("purple_alien.png");
            state.enemies[i].position = glm::vec3(-1, -2.75, 0);
            state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
            // in game player has to jump before triggering AIHopper in order to jump on and kill alien
            state.enemies[i].aiType = HOPPER;
            state.enemies[i].aiState = IDLE;
        }
       
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].height = 1.0f;
        state.enemies[i].width = 1.0f;
       
        state.enemies[i].movement = glm::vec3(0);
        state.enemies[i].speed = 1;
    }
}


void ProcessInput() {
    
    // reset player movement to 0 - initialize if nothing is pressed -> nothing happens
    state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player to the left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player to the right
                        break;
                        
                    case SDLK_SPACE:
                        // Player Jumps on Space
                        if (state.player->bottomCollision) {
                            state.player->jump = true;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f; // moves 1 unit left
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f; // moves 1 unit right
    }
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }
}


void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    // c++ thing
    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });

    } // end for loop
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}



#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
        
        for (int i = 0; i < ENEMY_COUNT; i++) {
                   state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // render platorms
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }
    
    // render enemies 
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(&program);
    }
    
    state.player->Render(&program);
    

    int defeated = 0;
      for (int i = 0; i < ENEMY_COUNT; i++) {
          if (state.enemies[i].playerDefeated) {
              defeated += 1;
          }
      }
      std::cout << "Enemies Defeated: " << defeated << "\n";
      if (defeated == ENEMY_COUNT) {
          state.player->enemiesDefeated = true; // if all enemies are defeated
      }

      if (state.player->enemiesDefeated == true) {
          DrawText(&program, LoadTexture("font1.png"), "You Win!", 0.5f, -0.25f, glm::vec3(-1.0, 3.0, 0));
          state.player->isActive = false;
      }
      else if (state.player-> playerDefeated == true) {
          DrawText(&program, LoadTexture("font1.png"), "You Lose, Game Over!", 0.5f, -0.25f, glm::vec3(-2.5, 3.0, 0));
          state.player->isActive = false;
      }

    
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}

