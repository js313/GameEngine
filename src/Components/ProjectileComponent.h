#ifndef PROJECTILECOMPONENT_H
#define PROJECTILECOMPONENT_H

#include <SDL2/SDL.h>

struct ProjectileComponent
{
    bool isFriendly;
    int hitPercentage;
    int duration;
    int startTime;

public:
    ProjectileComponent(bool isFriendly = false, int hitPercentage = 0, int duration = 0)
    {
        this->isFriendly = isFriendly;
        this->hitPercentage = hitPercentage;
        this->duration = duration;
        this->startTime = SDL_GetTicks();
    }
};

#endif