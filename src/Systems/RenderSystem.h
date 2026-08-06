#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <vector>
#include <algorithm>

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

    void Update(SDL_Renderer *renderer, std::unique_ptr<AssetStore> &assetStore)
    {
        std::vector<Entity> entities = GetSystemEntities();
        sort(entities.begin(), entities.end(), [](Entity &a, Entity &b)
             { return a.GetComponent<SpriteComponent>().zIndex < b.GetComponent<SpriteComponent>().zIndex; });
        for (Entity entity : entities)
        {
            const TransformComponent &transform = entity.GetComponent<TransformComponent>();
            const SpriteComponent &sprite = entity.GetComponent<SpriteComponent>();

            SDL_Rect srcRect = sprite.srcRect;

            SDL_Rect dstRect = {
                static_cast<int>(transform.position.x),
                static_cast<int>(transform.position.y),
                static_cast<int>(sprite.width * transform.scale.x),
                static_cast<int>(sprite.height * transform.scale.y),
            };

            SDL_RenderCopyEx(renderer, assetStore->GetTexture(sprite.assetId), &srcRect, &dstRect, transform.rotation, NULL, SDL_FLIP_NONE);
        }
    }
};

#endif