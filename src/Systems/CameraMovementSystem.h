#ifndef CAMERAMOVEMENTSYSTEM_H
#define CAMERAMOVEMENTSYSTEM_H

#include <SDL2/SDL.h>

#include "../Game/Game.h"
#include "../ECS/ECS.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"

class CameraMovementSystem : public System
{
public:
    CameraMovementSystem()
    {
        RequireComponent<CameraFollowComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(SDL_Rect &camera)
    {
        for (const Entity &entity : GetSystemEntities())
        {
            const TransformComponent &transform = entity.GetComponent<TransformComponent>();

            if (transform.position.x > camera.w / 2)
                camera.x = transform.position.x - camera.w / 2;
            if (transform.position.y > camera.h / 2)
                camera.y = transform.position.y - camera.h / 2;

            camera.x = camera.x < 0 ? 0 : camera.x;
            camera.y = camera.y < 0 ? 0 : camera.y;
            camera.x = camera.x + camera.w > Game::mapWidth ? Game::mapWidth - camera.w : camera.x;
            camera.y = camera.y + camera.h > Game::mapHeight ? Game::mapHeight - camera.h : camera.y;
        }
    }
};

#endif