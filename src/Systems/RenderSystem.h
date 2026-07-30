#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Logger/Logger.h"

class RenderSystem : public System
{
public:
    RenderSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer *renderer)
    {
        for (Entity entity : GetSystemEntities())
        {
            const TransformComponent &transform = entity.GetComponent<TransformComponent>();
            const SpriteComponent &sprite = entity.GetComponent<SpriteComponent>();

            SDL_Rect objRect = {
                (int)transform.position.x,
                (int)transform.position.y,
                sprite.width,
                sprite.height,
            };

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &objRect);
        }
    }
};

#endif