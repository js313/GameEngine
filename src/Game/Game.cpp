#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

#include "Game.h"
#include "../Logger/Logger.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

Game::Game()
{
    isRunning = false;
    millisecsPreviousFrame = 0;
    registry = std::make_unique<Registry>();
    Logger::Log("Game constructor called");
}

Game::~Game()
{
    Logger::Log("Game destructor called");
}

void Game::Initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Logger::Err("Error initializing SDL");
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, // did not set to displayMode.w as players with larger screens would have had unfair advantage as they could see more of the area
        600, // same for displayMode.h,
        SDL_WINDOW_BORDERLESS);

    if (window == NULL)
    {
        Logger::Err("Error creating SDL window\n");
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL)
    {
        Logger::Err("Error creating SDL renderer\n");
    }
    isRunning = true;

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN); // from fake full screen to actual full screen, by changine the video mode to full screen
}

glm::vec2 playerPosition, playerVelocity;

void Game::Setup()
{
    playerPosition = glm::vec2(10.0, 20.0);
    playerVelocity = glm::vec2(100.0, 0.0);

    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();

    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 10.0));
    tank.AddComponent<SpriteComponent>(32, 32);

    Entity truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 10.0));
    truck.AddComponent<SpriteComponent>(10, 10);
}

void Game::Update()
{
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
    {
        SDL_Delay(timeToWait);
    }
    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0; // in secs
    millisecsPreviousFrame = SDL_GetTicks();

    playerPosition += playerVelocity * glm::vec2(deltaTime, deltaTime);

    registry->GetSystem<MovementSystem>().Update(deltaTime);

    registry->Update(); // update the registry at the end of the frame
}

void Game::Run()
{
    Setup();
    while (isRunning)
    {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput()
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
                isRunning = false;
            break;
        }
    }
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // SDL_Rect player = {10, 10, 20, 20};
    // SDL_RenderFillRect(renderer, &player);

    // SDL_Surface *surface = IMG_Load("./assets/images/tank-tiger-right.png");
    // SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    // SDL_FreeSurface(surface);

    // // destination rectangle(location on the rendered window) where we want to place the texture
    // SDL_Rect destinationRectangle = {static_cast<int>(playerPosition.x), static_cast<int>(playerPosition.y), 32, 32};
    // SDL_RenderCopy(renderer, texture, NULL, &destinationRectangle);
    // SDL_DestroyTexture(texture);

    registry->GetSystem<RenderSystem>().Update(renderer);

    // SDL has concept of 2 buffers and we always draw on back buffer, here we swap those buffers to finally display it on screen
    // This is done to prevent glictches as the whole screen needs to refresh when asomething is drawn on screen
    SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}