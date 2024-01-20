#include "imgui.h"
#include "imgui_UBO.h"
#include <TracerX/VectorMath.h>

bool ImGui::SliderAngleUBO(const char* label, TracerX::UBO<float>& v_rad, float v_degrees_min, float v_degrees_max, const char* format, ImGuiSliderFlags flags)
{
    float value = v_rad.get();
    bool modified = ImGui::SliderAngle(label, &value, v_degrees_min, v_degrees_max, format, flags);
    if (modified)
    {
        v_rad.set(value);
    }

    return modified;
}

bool ImGui::DragVector3fUBO(const char* label, TracerX::UBO<sf::Vector3f>& v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    sf::Vector3f value = v.get();
    bool modified = ImGui::DragFloat3(label, (float*)&value, v_speed, v_min, v_max, format, flags);
    if (modified)
    {
        v.set(value);
    }

    return modified;
}

bool ImGui::DragVector3fUBONormalize(const char* label, TracerX::UBO<sf::Vector3f>& v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    sf::Vector3f value = v.get();
    bool modified = ImGui::DragFloat3(label, (float*)&value, v_speed, v_min, v_max, format, flags);
    if (modified)
    {
        v.set(TracerX::normalized(value));
    }

    return modified;
}

bool ImGui::DragFloatUBO(const char* label, TracerX::UBO<float>& v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    float value = v.get();
    bool modified = ImGui::DragFloat(label, &value, v_speed, v_min, v_max, format, flags);
    if (modified)
    {
        v.set(value);
    }

    return modified;
}

bool ImGui::DragIntUBO(const char* label, TracerX::UBO<int>& v, float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    int value = v.get();
    bool modified = ImGui::DragInt(label, &value, v_speed, v_min, v_max, format, flags);
    if (modified)
    {
        v.set(value);
    }

    return modified;
}

bool ImGui::ColorEdit3UBO(const char* label, TracerX::UBO<sf::Vector3f>& col, ImGuiColorEditFlags flags)
{
    sf::Vector3f value = col.get();
    bool modified = ImGui::ColorEdit3(label, (float*)&value, flags);
    if (modified)
    {
        col.set(value);
    }

    return modified;
}

bool ImGui::CheckboxUBO(const char* label, TracerX::UBO<bool>& v)
{
    bool value = v.get();
    bool modified = ImGui::Checkbox(label, &value);
    if (modified)
    {
        v.set(value);
    }

    return modified;
}