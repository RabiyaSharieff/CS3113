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

#define PLATFORM_COUNT 13
#define OBSTACLE_COUNT 3

struct GameState {
    Entity *player;
    Entity *platforms;
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
    displayWindow = SDL_CreateWindow("Lava Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
    
    // Player (Spaceship) Initialization
    state.player = new Entity();
    state.player->position = glm::vec3(0, 4.5f, 0); // spaceship decends from top of screen
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.7f, 0);
    state.player->speed = 1.3f;
    state.player->textureID = LoadTexture("playerShip1_green.png");
        
    // Initialize and Load Tile/Obstacle Textures
    state.platforms = new Entity[PLATFORM_COUNT + OBSTACLE_COUNT];
    GLuint waterTextureID = LoadTexture("platformPack_tile005.png");
    GLuint lavaTextureID = LoadTexture("platformPack_tile018.png");
    GLuint obstacleTextureID = LoadTexture("platformPack_tile024.png");
    
    float lava_x = -4.5f; // dont need y pos for lava since it stays the same
    float obstacle_x = -3.2f; // obstacle 1 starts at this x position
    float obstacle_y = 1.4f; // obstacle 1 starts at this  y position
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        
        // obstacles
        if (i > 9) {
            state.platforms[i].textureID = obstacleTextureID;
            state.platforms[i].position = glm::vec3(obstacle_x, obstacle_y, 0);
            obstacle_x += 3;
            obstacle_y -= 1;
        }
        
        // water tiles
        else if (i > 4 && i < 8) {
            state.platforms[i].textureID = waterTextureID;
            state.platforms[i].position = glm::vec3(lava_x, -3.25, 0);
            state.platforms[i].height = 0.35;
            lava_x += 1;
        }
        // lava tiles
        else {
            state.platforms[i].textureID = lavaTextureID;
            state.platforms[i].position = glm::vec3(lava_x, -3.25, 0);
            lava_x += 1;
        }
    }
    
    for (int i = 0; i < (PLATFORM_COUNT + OBSTACLE_COUNT); i++ ){
        state.platforms[i].Update(0, NULL, 0);
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
                        
                    //case SDLK_SPACE:
                        // Some sort of action
                        //break; // dont use
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    // Move Player (spaceship) if no collisions with tiles
    if (state.player->bottomCollision == false) {
        if (keys[SDL_SCANCODE_LEFT]) {
            state.player->movement.x = -1.0f;
            state.player->acceleration.y = -0.5f;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            state.player->movement.x = 1.0f;
            state.player->acceleration.y = -0.5f;
        }
        
        if (glm::length(state.player->movement) > 1.0f) {
            state.player->movement = glm::normalize(state.player->movement);
        }
    }
    
    // Check  Player (spaceship) for collisons water tiles (safe)
    else if ((state.player->CheckCollision(&state.platforms[5])) || (state.player->CheckCollision(&state.platforms[6])) || (state.player->CheckCollision(&state.platforms[7]))) {
        
        state.player->waterCollision = true;
        std::cout << "Ship is safe!\n";
    }
    
    // collision with lava or obstacle tiles
    else {
        state.player->isSafe = false;
        std::cout << "Game Over!\n";
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
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }
    
    // Display message on screen using DrawText
    if (state.player->waterCollision == true) {
        DrawText(&program, LoadTexture("font1.png"), "Mission Successful!", 0.8f, -0.3f, glm::vec3(-4.35f,0, 0));
        state.player->isActive = false;
    }
    else if (state.player->isSafe == false) {
        DrawText(&program, LoadTexture("font1.png"), "Mission Failed!", 0.8f, -0.3f, glm::vec3(-4.35f, 0, 0));
        state.player->isActive = false;
    }
    
    state.player->Render(&program);
    
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

