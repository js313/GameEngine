#ifndef RENDERCOLLIDERSYSTEM_H
#define RENDERCOLLIDERSYSTEM_H

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"

class RenderColliderSystem : public System
{
public:
    RenderColliderSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }

    void Update(SDL_Renderer *renderer, const SDL_Rect &camera)
    {
        for (const Entity &entity : GetSystemEntities())
        {
            const TransformComponent &transform = entity.GetComponent<TransformComponent>();
            const BoxColliderComponent &collider = entity.GetComponent<BoxColliderComponent>();

            float left = transform.position.x + collider.offset.x - camera.x;
            float top = transform.position.y + collider.offset.y - camera.y;

            SDL_Rect boundingBox = {
                static_cast<int>(left),
                static_cast<int>(top),
                collider.width,
                collider.height};

            if (collider.isColliding)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for colliding
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for not colliding
            }
            SDL_RenderDrawRect(renderer, &boundingBox);
        }
    }
};

#endif