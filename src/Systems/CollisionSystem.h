#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include <SDL2/SDL.h>

#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"

class CollisionSystem : public System
{
public:
    CollisionSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }

    void Update()
    {
        auto entities = GetSystemEntities();
        for (const Entity &entity : entities)
        {
            entity.GetComponent<BoxColliderComponent>().isColliding = false;
        }
        for (int i = 0; i < (int)entities.size(); i++)
        {
            const Entity &entityA = entities[i];
            for (int j = i + 1; j < (int)entities.size(); j++)
            {
                const Entity &entityB = entities[j];

                const TransformComponent &transformA = entityA.GetComponent<TransformComponent>();
                const BoxColliderComponent &colliderA = entityA.GetComponent<BoxColliderComponent>();

                const TransformComponent &transformB = entityB.GetComponent<TransformComponent>();
                const BoxColliderComponent &colliderB = entityB.GetComponent<BoxColliderComponent>();

                float leftA = transformA.position.x + colliderA.offset.x;
                float topA = transformA.position.y + colliderA.offset.y;

                float leftB = transformB.position.x + colliderB.offset.x;
                float topB = transformB.position.y + colliderB.offset.y;

                // Check for collision
                if (CheckAABBCollision(leftA, topA, colliderA.width, colliderA.height, leftB, topB, colliderB.width, colliderB.height))
                {
                    Logger::Log("Collision detected between Entity " + std::to_string(entityA.GetId()) + " and Entity " + std::to_string(entityB.GetId()));
                    entityA.GetComponent<BoxColliderComponent>().isColliding = true;
                    entityB.GetComponent<BoxColliderComponent>().isColliding = true;
                }
            }
        }
    }

    bool CheckAABBCollision(int aX, int aY, int aWidth, int aHeight, int bX, int bY, int bWidth, int bHeight)
    {
        return (aX < bX + bWidth &&
                aX + aWidth > bX &&
                aY < bY + bHeight &&
                aY + aHeight > bY);
    }
};

#endif