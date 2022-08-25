#pragma once

#include "imgui.h"

namespace MyImGui {
    /*"bool selected" carry the selection state(read - only).    
    Selectable() is clicked is returns true so you can modify your selection state. 
    size.x==0.0: use remaining width, size.x>0.0: specify width. size.y==0.0: use label height, size.y>0.0: specify height
    */
    IMGUI_API bool SelectableCustomColor(const char* label, bool selected = false, ImU32 selected_color = ImGui::GetColorU32(ImGuiCol_HeaderActive), 
        ImU32 hovered_color = ImGui::GetColorU32(ImGuiCol_HeaderHovered), ImU32 default_color = ImGui::GetColorU32({0, 0, 0, 0}), ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
}