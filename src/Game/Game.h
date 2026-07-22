#ifndef GAME_H
#define GAME_H

#include <memory>

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool isRunning;
    int millisecsPreviousFrame;

    std::unique_ptr<Registry> registry;

public:
    Game();
    void Initialize();
    void Run();
    void Setup();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();
    ~Game();

    int windowHeight;
    int windowWidth;
};

#endif