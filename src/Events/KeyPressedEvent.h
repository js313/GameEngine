#ifndef KEY_PRESSED_EVENT_H
#define KEY_PRESSED_EVENT_H

#include <SDL2/SDL.h>

#include "../EventBus/Event.h"

class KeyPressedEvent : public Event
{
public:
    SDL_Keycode keyPressed;

    KeyPressedEvent(SDL_Keycode keyPressed) : keyPressed(keyPressed) {}
};

#endif