#ifndef PROJECTILELIFECYCLESYSTEM_H
#define PROJECTILELIFECYCLESYSTEM_H

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"

class ProjectileLifecycleSystem : public System
{
public:
    ProjectileLifecycleSystem()
    {
        RequireComponent<ProjectileComponent>();
    }
    void Update()
    {
        for (auto &entity : GetSystemEntities())
        {
            auto projectile = entity.GetComponent<ProjectileComponent>();
            const Uint32 elapsed = SDL_GetTicks() - static_cast<Uint32>(projectile.startTime);
            if (static_cast<Uint32>(projectile.duration) < elapsed)
            {
                entity.Kill();
            }
        }
    }
};

#endif