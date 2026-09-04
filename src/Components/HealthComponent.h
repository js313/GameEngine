#ifndef HEALTHCOMPONENT_H
#define HEALTHCOMPONENT_H

struct HealthComponent
{
    int healthPercent;

    HealthComponent(int healthPercent = 0)
    {
        this->healthPercent = healthPercent;
    }
};

#endif