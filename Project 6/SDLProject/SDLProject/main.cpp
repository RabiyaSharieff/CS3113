#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Entity.h"
#include "Map.h"
#include "Util.h"
#include "Scene.h"

#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

#include "Win.h"
#include "Lose.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene *currentScene;
Scene *sceneList[6];

Mix_Music *music;

void SwitchToScene(int _nextScene, int _lives=3) {
    if (_nextScene == 1) {
        currentScene = new Level1(_lives);
    }
    else if (_nextScene == 2) {
        currentScene = new Level2(_lives);
    }
    else if (_nextScene == 3) {
        currentScene = new Level3(_lives);
    }
    
    // win = 4 and lose = 5
    else {
        currentScene = sceneList[_nextScene];
    }
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Picnic Mayhem", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    // load music
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("516950__m71art__jamm.wav");
    Mix_PlayMusic(music, -1);
    
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.5f, 0.0f, 0.0f); // green background color
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    sceneList[0] = new Menu();
    sceneList[1] = new Level1(3);
    sceneList[2] = new Level2(3);
    sceneList[3] = new Level3(3);
    sceneList[4] = new Win();
    sceneList[5] = new Lose();
    SwitchToScene(0);
}

void ProcessInput() {
    
    // reset player movement to 0 - initialize if nothing is pressed -> nothing happens
    currentScene->state.player->movement = glm::vec3(0);
    
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
                    case SDLK_UP:
                        // Move the player up
                        break;
                
                    case SDLK_DOWN:
                        // Move the player down
                        break;
                    case SDLK_RETURN:
                        // restarts game
                        currentScene->state.player_lives = 3;
                        SwitchToScene(1);
                        break;
                        
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_RETURN]) {
        currentScene->state.player_lives = 3;
        SwitchToScene(1);
    }
    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f; // moves 1 unit left
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f; // moves 1 unit right
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }
    else if (keys[SDL_SCANCODE_UP]) {
           currentScene->state.player->movement.y = 1.0f; // moves 1 unit right
           currentScene->state.player->animIndices = currentScene->state.player->animUp;
       }
    else if (keys[SDL_SCANCODE_DOWN]) {
           currentScene->state.player->movement.y = -1.0f; // moves 1 unit right
           currentScene->state.player->animIndices = currentScene->state.player->animDown;
       }
    
    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }

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
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;
    
    // View Matrix for scrolling
    viewMatrix = glm::mat4(1.0f);
    

    //left boundary
    if (currentScene->state.player->position.y > -12.5) {
        if (currentScene->state.player->position.x > 5) {
            if (currentScene->state.player->position.x > 12) {
                if (currentScene->state.player->position.y > -2) {
                    viewMatrix = glm::translate(viewMatrix, glm::vec3(-12, 2.1, 0));
                }
                else {
                    viewMatrix = glm::translate(viewMatrix, glm::vec3(-12, -currentScene->state.player->position.y, 0));
                }
            }
            else {
                if (currentScene->state.player->position.y > -2) {
                    viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 2.1, 0));
                }
                else {
                    viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, -currentScene->state.player->position.y, 0));
                }
            }
        }

        // right boundary
        else {
            if (currentScene->state.player->position.y > -2) {
                if (currentScene->state.player->position.x < 5) {
                    viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 2.1, 0));
                }
                else {
                    viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 2.1, 0));
                }
            }
            else {
                viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, -currentScene->state.player->position.y, 0));
            }
        }
    }
    else {
        if (currentScene->state.player->position.x < 5) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 12.75, 0));
        }
        else {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 12.75, 0));
        }
    }
    
    std::cout << "(" << currentScene->state.player->position.x << ", " << currentScene->state.player->position.y << ", " << currentScene->state.player->position.z;
    std::cout << ")\n";

}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);
    
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
        
        if (currentScene->state.nextScene >= 0) {
            if (currentScene->state.player_lives == 0) {
                SwitchToScene(5); // game over
            }
            // next level
            else {
                SwitchToScene(currentScene->state.nextScene, currentScene->state.player_lives);
            }
        }
        
        Render();
    }
    
    Shutdown();
    return 0;
}

