#ifndef RENDERTEXTSYSTEM_H
#define RENDERTEXTSYSTEM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../ECS/ECS.h"
#include "../Components/TextLabelComponent.h"

class RenderTextSystem : public System
{
public:
    RenderTextSystem()
    {
        RequireComponent<TextLabelComponent>();
    }

    void Update(std::unique_ptr<AssetStore> &assetStore, SDL_Renderer *renderer, const SDL_Rect &camera)
    {
        for (auto entity : GetSystemEntities())
        {
            auto &textLabel = entity.GetComponent<TextLabelComponent>();

            SDL_Surface *surface = TTF_RenderText_Blended(assetStore->GetFont(textLabel.assetId), textLabel.text.c_str(), textLabel.color);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            SDL_Rect destinationRectangle;
            destinationRectangle.x = static_cast<int>(textLabel.position.x) - (textLabel.isFixed ? 0 : camera.x);
            destinationRectangle.y = static_cast<int>(textLabel.position.y) - (textLabel.isFixed ? 0 : camera.y);
            SDL_QueryTexture(texture, NULL, NULL, &destinationRectangle.w, &destinationRectangle.h);

            SDL_RenderCopy(renderer, texture, NULL, &destinationRectangle);
            SDL_DestroyTexture(texture);
        }
    }
};

#endif