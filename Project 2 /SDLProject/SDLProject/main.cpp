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

#include <iostream>
using namespace std;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, player1_Matrix, player2_Matrix, ballMatrix, projectionMatrix;


// Ball Initilaization
// Start at 0,0,0
glm::vec3 ball_position = glm::vec3(0,0,0);
// Don't go anywhere (yet).
glm::vec3 ball_movement = glm::vec3(0,0,0);
glm::vec3 ball_scale = glm::vec3(0.4f, 0.4f, 1.0f);
float ball_height = 1.0f * ball_scale.y;
float ball_width = 1.0f * ball_scale.x;

float ball_speed = 3.0f;
float ball_rotate = 1.0f;
bool ball_reversed = false;


// Player 1 Initalization
glm::vec3 p1_position = glm::vec3(-4.6, 0, 0);
glm::vec3 p1_movement = glm::vec3(0, 0, 0);
float p1_speed = 3.85f;

// Player 2 Initalization
glm::vec3 p2_position = glm::vec3(4.6, 0, 0);
glm::vec3 p2_movement = glm::vec3(0, 0, 0);
float p2_speed = 3.85f;

// Both Paddles
glm::vec3 paddles_scale = glm::vec3(0.70f, 2.70f, 1.0f);
float paddles_height = 1.0f * paddles_scale.y;
float paddles_width = 1.0f * paddles_scale.x;


// textures 
GLuint player1TextureID;
GLuint player2TextureID;
GLuint ballTextureID;

// ball reaches top of screen
bool topScreen(glm::vec3 position,float top, float height) {
    return !((position.y + (height / 2.0f)) < top);
}

// ball reaches bottom of screen
bool bottomScreen(glm::vec3 position, float bottom, float height) {
    return !((position.y - (height / 2.0f)) > bottom);
}



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
    displayWindow = SDL_CreateWindow("Project 2: Pong",  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl"); // loading textured polygon shader
    
    viewMatrix = glm::mat4(1.0f);
    ballMatrix =  glm::mat4(1.0f);
    player1_Matrix =  glm::mat4(1.0f);
    player2_Matrix =  glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // background color
    
    glEnable(GL_BLEND);
    
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    // load textures
    player1TextureID = LoadTexture("player1.png"); // load texture
    player2TextureID = LoadTexture("player2.png");
    ballTextureID = LoadTexture("ball.png");
}

void ProcessInput() {
    
    // reset player movement to 0 - initialize if nothing is pressed -> nothing happens
    p1_movement = glm::vec3(0);
    p2_movement =glm::vec3(0);
    
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
                            // Move the player left
                            break;
                        case SDLK_RIGHT:
                            // Move the player right
                            break;
                        case SDLK_SPACE:
                            // Some sort of action
                            break;
                    }
                    break; // SDL_KEYDOWN
            }
        }
        
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        // BALL
        if (keys[SDL_SCANCODE_SPACE]) {
            int slope = rand(); // initial angle of movement of ball
            ball_movement.x = 1.0f * slope;
            ball_movement.y = 1.0f * slope;
        }
        if (glm::length(ball_movement) > 1.0f) {
            ball_movement = glm::normalize(ball_movement);
        }
        
        // PLAYER 1 ( W and S Keys)
        if ((keys[SDL_SCANCODE_W]) && !topScreen(p1_position, 3.7f, paddles_height)) {
            p1_movement.y = 1.0f;
        }
        else if ((keys[SDL_SCANCODE_S]) && !bottomScreen(p1_position, -3.7f, paddles_height)) {
            p1_movement.y = -1.0f;
        }
        // dont move fatser if holding down both keys
        if (glm::length(p1_movement) > 1.0f) {
            p1_movement = glm::normalize(p1_movement);
        }
        
        // PLAYER 2 ( Up and Down Keys)
        if ((keys[SDL_SCANCODE_UP]) && !topScreen(p2_position, 3.7f, paddles_height)) {
            p2_movement.y = 1.0f;
        }
        else if ((keys[SDL_SCANCODE_DOWN]) && !bottomScreen(p2_position, -3.7f, paddles_height)) {
            p2_movement.y = -1.0f;
        }
        if (glm::length(p2_movement) > 1.0f) {
            p2_movement = glm::normalize(p2_movement);
        }
}


bool isCollison(glm::vec3 ball_position, glm::vec3 p_position) {
    
    // BALL
    float x_ball = ball_position.x;
    float y_ball = ball_position.y;
    float w_ball = ball_width - 0.2f;
    float h_ball = ball_height - 0.2f;

    // PLAYERS
    float x_players = p_position.x;
    float y_players = p_position.y;
    float w_players = paddles_width;
    float h_players = paddles_height;
    
    float x_dist = fabs(x_players - x_ball) - ((w_ball + w_players) / 2);
    float y_dist = fabs(y_players - y_ball) - ((h_ball + h_players) / 2);
    
    // Box to Box : x and y distance need to be <0  to be colliding
    return ((x_dist < 0) && (y_dist < 0)); // --> returns true if colliding
}


int winner = -1; // initialize - if exited out of game
int p1_score, p2_score = 0;

bool isPaddleTouch(glm::vec3 ball_position) {
    float right = 4.6f;
    float left = -4.6f;
    
    if (ball_position.x > right) {
        p1_score += 1;
        if (p1_score == 1)
            winner = 1;
        return true;
    }
    else if (ball_position.x < left) {
        p2_score += 1;
        if (p2_score == 1)
            winner = 2;
        return true;
    }
    return false;
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    if (isPaddleTouch(ball_position)) {
           if ((p1_score == 1) || (p2_score == 1))
               gameIsRunning = false;
           else
               ball_position = glm::vec3(0, 0, 0);
    }
       
   // BALL
   ballMatrix= glm::mat4(1.0f);
   
   // Ball touching a wall
   if ((topScreen(ball_position,3.7f,  ball_height) || bottomScreen(ball_position, -3.7f, ball_height))) {
       ball_movement.y *= -1.0f;
   }
   // Ball touching paddle
   else if ((isCollison(ball_position, p1_position)) || (isCollison(ball_position, p2_position))) {
       ball_movement.x *= -1.0f;
   }
   
    // Add (direction * units per second * elapsed time)
   ball_position += ball_movement * ball_speed * deltaTime;
   ballMatrix = glm::translate( ballMatrix, ball_position);
   ballMatrix = glm::scale( ballMatrix, ball_scale);


    // PLAYER 1
    // Add (direction * units per second * elapsed time)
    p1_position += p1_movement * p1_speed * deltaTime;
    player1_Matrix = glm::mat4(1.0f);
    player1_Matrix = glm::translate(player1_Matrix, p1_position);
    player1_Matrix = glm::scale(player1_Matrix, paddles_scale);



    // PLAYER 2
    // Add (direction * units per second * elapsed time)
    p2_position += p2_movement * p2_speed * deltaTime;
    player2_Matrix = glm::mat4(1.0f);
    player2_Matrix = glm::translate(player2_Matrix, p2_position);
    player2_Matrix = glm::scale(player2_Matrix, paddles_scale);

}

void Render() {
    
     glClear(GL_COLOR_BUFFER_BIT);
     
         
     float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
     float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
         
     glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
     glEnableVertexAttribArray(program.positionAttribute);
     glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
     glEnableVertexAttribArray(program.texCoordAttribute);
    
    
     program.SetModelMatrix(player1_Matrix);
     glBindTexture(GL_TEXTURE_2D, player1TextureID);
     glDrawArrays(GL_TRIANGLES, 0, 6);
       
     program.SetModelMatrix(player2_Matrix);
     glBindTexture(GL_TEXTURE_2D, player2TextureID);
     glDrawArrays(GL_TRIANGLES, 0, 6);
       
     program.SetModelMatrix(ballMatrix);
     glBindTexture(GL_TEXTURE_2D, ballTextureID);
     glDrawArrays(GL_TRIANGLES, 0, 6);
       
    
     glDisableVertexAttribArray(program.positionAttribute);
     glDisableVertexAttribArray(program.texCoordAttribute);

     SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    cout << "\n--------------------------------\n";
    cout << "\t\t Player " << winner << " wins!\n";
    cout << "\t\t\t Game over!\n";
    cout << "--------------------------------\n\n";
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

