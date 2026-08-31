#ifndef GAME_H
#define GAME_H

#include <memory>

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect camera;
    bool isRunning;
    int millisecsPreviousFrame;
    bool isDebug;

    std::unique_ptr<Registry> registry;
    std::unique_ptr<AssetStore> assetStore;
    std::unique_ptr<EventBus> eventBus;

public:
    Game();
    void Initialize();
    void Run();
    void Setup();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();
    void LoadLevel(int level);
    ~Game();

    static int windowHeight;
    static int windowWidth;
    static int mapHeight;
    static int mapWidth;
};

#endif