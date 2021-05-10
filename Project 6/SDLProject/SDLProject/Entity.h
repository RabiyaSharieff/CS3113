#pragma once
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
#include "Map.h"

enum EntityType {PLAYER, PLATFORM, ENEMY, COIN};
enum AIType { WALKER, WAITANDGO, PATROL };
enum AIState { IDLE, ACTIVE };

class Entity {
    public:
        EntityType entityType;
        AIType aiType;
        AIState aiState;
        
        glm::vec3 position;
        glm::vec3 movement;
        glm::vec3 acceleration;
        glm::vec3 velocity;
        
        float width = 1;
        float height = 1;
        
        float jump = false;
        float jumpPower = 0;
        
        float speed;
        
        GLuint textureID;
        
        glm::mat4 modelMatrix;
        
        int *animRight = NULL;
        int *animLeft = NULL;
        int *animUp = NULL;
        int *animDown = NULL;

        int *animIndices = NULL;
        int animFrames = 0;
        int animIndex = 0;
        float animTime = 0;
        int animCols = 0;
        int animRows = 0;
        
        bool isActive = true;
        float lives = 3;
    
        // Collision Flags
        bool bottomCollision = false;
        bool topCollision = false;
        bool rightCollision = false;
        bool leftCollision = false;
    
        bool enemiesDefeated = false;
        bool playerDefeated = false;
    
        bool wasCollected = false; // coin was collected
        bool coinsCollected = false; // player collects

        Entity();
        

        bool CheckCollision(Entity *other);
        void CheckCollisionsY(Entity *objects, int objectCount);
        void CheckCollisionsX(Entity *objects, int objectCount);
        //void CheckCollisionsEnemy(Entity *enemies, int enemyCount);
        //void CheckCollisionsCoin(Entity *coins);
        void CheckCollisionsX(Map *map);
        void CheckCollisionsY(Map *map);
        
        void AI(Entity *player);
        void AIWalker();
        void AIWaitAndGo(Entity *player);
        void AIPatrol();
    
        void Update(float deltaTime, Entity *player, Entity *objects, int objectCount, Map *map);
        void Render(ShaderProgram *program);
        void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
        
    
    
};

