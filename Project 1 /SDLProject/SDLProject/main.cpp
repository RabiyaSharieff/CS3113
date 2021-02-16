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

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, rabiyaMatrix, paintMatrix1, paintMatrix2, paintMatrix3, projectionMatrix;

float player_x = 0;
float player_rotate = 0;

GLuint paintTextureID;
GLuint rabiyaTextureID;



// takes file path and loads image , get a texture id ...

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
    displayWindow = SDL_CreateWindow("Project 1: Simple 2D Scene",  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl"); // loading textured polygon shader
    
    viewMatrix = glm::mat4(1.0f);
    rabiyaMatrix = glm::mat4(1.0f); // model matrix set to be identity matrix, and we're never changing it!
    
    paintMatrix1 = glm::mat4(1.0f); // model matrix set to be identity matrix, and we're never changing it!
    paintMatrix2 = glm::mat4(1.0f);
    paintMatrix3 = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 0.0f, 1.0f, 1.0f); // triangle color
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // background color
    
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    paintTextureID = LoadTexture("paint.png"); // load texture
    rabiyaTextureID = LoadTexture("rabiya.png");
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}


float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
        
    player_x += 2.0f * deltaTime;
    player_rotate += 90.0f * deltaTime;
    
    rabiyaMatrix = glm::mat4(1.0f);
    
    // get rabiya object to keep going back and forth on the screen
    if (player_x >= 5.0f){
        player_x= -5.0f;
    }
    else {
        rabiyaMatrix  = glm::translate(rabiyaMatrix , glm::vec3(player_x, -2.0f, 0.0f));
    }
    rabiyaMatrix  = glm::scale(rabiyaMatrix , glm::vec3(2.01f, 2.01f, 0.0f));
    
    paintMatrix1 = glm::mat4(1.0f);
    paintMatrix1 = glm::rotate(paintMatrix1, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    paintMatrix1  = glm::scale(paintMatrix1 , glm::vec3(2.01f, 2.01f, 0.0f));
    
    paintMatrix2 = glm::mat4(1.0f);
    paintMatrix2 = glm::translate(paintMatrix2 , glm::vec3(-2.0f, 1.0f, 0.0f));
    paintMatrix2 = glm::rotate(paintMatrix2, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    paintMatrix2  = glm::scale(paintMatrix2 , glm::vec3(2.01f, 2.01f, 0.0f));
    
    paintMatrix3 = glm::mat4(1.0f);
    paintMatrix3 = glm::translate(paintMatrix3 , glm::vec3(2.0f, 1.0f, 0.0f));
    paintMatrix3 = glm::rotate(paintMatrix3, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    paintMatrix3  = glm::scale(paintMatrix3 , glm::vec3(2.01f, 2.01f, 0.0f));
    
}

void DrawChar(){
    program.SetModelMatrix(rabiyaMatrix);
    glBindTexture(GL_TEXTURE_2D, rabiyaTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
}

void Render() {
    
     glClear(GL_COLOR_BUFFER_BIT);
     
         
     float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
     float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
         
     glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
     glEnableVertexAttribArray(program.positionAttribute);
     glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
     glEnableVertexAttribArray(program.texCoordAttribute);
    
     DrawChar();
    
     program.SetModelMatrix(paintMatrix1);
     glBindTexture(GL_TEXTURE_2D, paintTextureID);
     glDrawArrays(GL_TRIANGLES, 0, 6);
       
     program.SetModelMatrix(paintMatrix2);
     glBindTexture(GL_TEXTURE_2D, paintTextureID);
     glDrawArrays(GL_TRIANGLES, 0, 6);
       
     program.SetModelMatrix(paintMatrix3);
     glBindTexture(GL_TEXTURE_2D, paintTextureID);
     glDrawArrays(GL_TRIANGLES, 0, 6);
       
    
     glDisableVertexAttribArray(program.positionAttribute);
     glDisableVertexAttribArray(program.texCoordAttribute);

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
