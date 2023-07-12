#include "imgui.h"
#include "imgui_UBO.h"
#include <TracerX/VectorMath.h>

bool ImGui::SliderAngleUBO(const char* label, TracerX::UBO<float>& v_rad, float v_degrees_min, float v_degrees_max, const char* format, ImGuiSliderFlags flags)
{
    return ImGui::SliderAngle(label, &v_rad.value, v_degrees_min, v_degrees_max, format, flags);
}

bool ImGui::DragVector3fUBO(const char* label, TracerX::UBO<sf::Vector3f>& v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return ImGui::DragFloat3(label, (float*)&v.value, v_speed, v_min, v_max, format, flags);
}

bool ImGui::DragVector3fUBONormalize(const char* label, TracerX::UBO<sf::Vector3f>& v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    bool changed = ImGui::DragFloat3(label, (float*)&v.value, v_speed, v_min, v_max, format, flags);
    if (changed)
    {
        v.value = TracerX::normalized(v.value);
    }

    return changed;
}

bool ImGui::DragFloatUBO(const char* label, TracerX::UBO<float>& v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return ImGui::DragFloat(label, &v.value, v_speed, v_min, v_max, format, flags);
}

bool ImGui::DragIntUBO(const char* label, TracerX::UBO<int>& v, float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return ImGui::DragInt(label, &v.value, v_speed, v_min, v_max, format, flags);
}