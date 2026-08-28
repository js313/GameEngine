#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>

#include "Game.h"
#include "../Logger/Logger.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlledComponent.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardMovementSystem.h"

Game::Game()
{
    isRunning = false;
    millisecsPreviousFrame = 0;
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    eventBus = std::make_unique<EventBus>();
    isDebug = false;
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
    windowWidth = 800;  // displayMode.w;
    windowHeight = 600; // displayMode.h;

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

void Game::Setup()
{
    LoadLevel(1);
}

void Game::LoadLevel(int level)
{
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>();
    registry->AddSystem<RenderColliderSystem>();
    registry->AddSystem<DamageSystem>();
    registry->AddSystem<KeyboardMovementSystem>();

    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
    assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
    assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

    // load jungle.map as a 2D integer array
    std::ifstream mapFile("./assets/tilemaps/jungle.map");
    if (!mapFile.is_open())
    {
        Logger::Err("Error opening jungle.map");
    }

    int tileSize = 32;
    double tileScale = 1.0;
    int mapNumCols = 25;
    int mapNumRows = 20;

    for (int y = 0; y < mapNumRows; y++)
    {
        for (int x = 0; x < mapNumCols; x++)
        {
            // very smart as, as the tileMap image is 10 across so the numbers like '21' represent 2nd row and 1st column
            // so we need to get the first digit for the y coordinate and second digit for the x coordinate
            // but only works if: 1. the tileMap image is 10 across, 2. the map file is formatted correctly with no spaces and only double digit numbers
            int srcRectY = mapFile.get() - '0';
            int srcRectX = mapFile.get() - '0';
            mapFile.ignore(); // ignore the comma
            Entity tileEntity = registry->CreateEntity();
            tileEntity.AddComponent<TransformComponent>(glm::vec2(x * (tileSize * tileScale), y * (tileSize * tileScale)), glm::vec2(tileScale, tileScale), 0.0);
            tileEntity.AddComponent<SpriteComponent>("tilemap-image", 0, tileSize, tileSize, srcRectX * tileSize, srcRectY * tileSize);
        }
    }

    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 0.0));
    tank.AddComponent<SpriteComponent>("tank-image", 1, 32, 32);
    tank.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0.0, 0.0));

    Entity truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(100.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(-10.0, 0.0));
    truck.AddComponent<SpriteComponent>("truck-image", 1, 32, 32);
    truck.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0.0, 0.0));

    Entity chopper = registry->CreateEntity();
    chopper.AddComponent<TransformComponent>(glm::vec2(50.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 2, 32, 32, 0, 1 * 32);
    chopper.AddComponent<AnimationComponent>(2, 10, true);
    chopper.AddComponent<KeyboardControlledComponent>(glm::vec2(0, -50.0), glm::vec2(50.0, 0), glm::vec2(0, 50.0), glm::vec2(-50.0, 0));

    Entity radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 42.0, 10.0), glm::vec2(0.5, 0.5), 0.0);
    radar.AddComponent<SpriteComponent>("radar-image", 1, 64, 64);
    radar.AddComponent<AnimationComponent>(8, 5, true);
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

    // Reset and re-subscribe to events every frame
    eventBus->Reset();

    registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<KeyboardMovementSystem>().SubscribeToEvents(eventBus);

    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<CollisionSystem>().Update(eventBus);
    registry->GetSystem<DamageSystem>().Update();

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
        case SDL_KEYUP:
            eventBus->Publish<KeyPressedEvent>(sdlEvent.key.keysym.sym);
            if (sdlEvent.key.keysym.sym == SDLK_d)
                isDebug = !isDebug;
            break;
        default:
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

    registry->GetSystem<RenderSystem>().Update(renderer, assetStore);
    if (isDebug)
        registry->GetSystem<RenderColliderSystem>().Update(renderer);

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