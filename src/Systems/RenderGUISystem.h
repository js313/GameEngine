#ifndef RENDER_GUI_SYSTEM_H
#define RENDER_GUI_SYSTEM_H

#include <SDL2/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>

#include "../ECS/ECS.h"

class RenderGUISystem : public System
{
public:
    RenderGUISystem() = default;

    void Update(SDL_Renderer *renderer, const SDL_Rect &camera)
    {
        ImGui::NewFrame();

        if (ImGui::Begin("Spawn enemies"))
        {
            ImGui::Text("This is a simple GUI window.");
        }
        ImGui::End();

        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());
    }
};

#endif