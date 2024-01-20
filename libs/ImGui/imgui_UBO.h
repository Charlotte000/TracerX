#pragma once

#include <SFML/System.hpp>
#include <TracerX/UBO.h>

namespace ImGui
{

IMGUI_API bool SliderAngleUBO(const char* label, TracerX::UBO<float>& v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char* format = "%.0f deg", ImGuiSliderFlags flags = 0);

IMGUI_API bool DragVector3fUBO(const char* label, TracerX::UBO<sf::Vector3f>& v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);

IMGUI_API bool DragVector3fUBONormalize(const char* label, TracerX::UBO<sf::Vector3f>& v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);

IMGUI_API bool DragFloatUBO(const char* label, TracerX::UBO<float>& v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);

IMGUI_API bool DragIntUBO(const char* label, TracerX::UBO<int>& v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);

IMGUI_API bool ColorEdit3UBO(const char* label, TracerX::UBO<sf::Vector3f>& col, ImGuiColorEditFlags flags = 0);

IMGUI_API bool CheckboxUBO(const char* label, TracerX::UBO<bool>& v);

}