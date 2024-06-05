#pragma once

#include <lzx/utility/parameter.hpp>
#include <lzx/utility/model.hpp>
#include <etl/type_traits.h>
#include <imgui.h>
#include <imgui_knobs.h>

namespace ImGuiParameter
{
    inline void Update(lzx::parameter &p)
    {
        if (p.has_keys())
        {
            if (ImGui::BeginCombo(p.id(), p.key()))
            {
                for (int it = 0; it < p.keys()->size(); it++)
                {
                    if (ImGui::Selectable(p.keys()->at(it)))
                    {
                        if (p.contains<int>())
                        {
                            p.set<int>(it);
                        }
                        else if (p.contains<bool>())
                        {
                            p.set<bool>(it != 0);
                        }
                    }
                }
                ImGui::EndCombo();
            }
        }
        else if (p.contains<float>())
        {
            float v = p.get<float>();
            // if (ImGuiKnobs::Knob(p.id(), &v, p.min<float>(), p.max<float>(), p.step(), "%1.3f", ImGuiKnobVariant_Tick, 0.0f, 0, 10))
            if (ImGui::SliderFloat(p.id(), &v, p.min<float>(), p.max<float>(), "%1.3f"))
            {
                p.set(v);
            }
        }
        else if (p.contains<bool>())
        {
            bool v = p.get<bool>();
            if (ImGui::Checkbox(p.id(), &v))
            {
                p.set<bool>(v);
            }
        }
        else if (p.contains<int>())
        {
            int v = p.get<int>();
            // if (ImGuiKnobs::KnobInt(p.id(), &v, p.min<int>(), p.max<int>(), p.step(), "%d", ImGuiKnobVariant_Tick, 0.0f, 0, 10))
            if (ImGui::SliderInt(p.id(), &v, p.min<int>(), p.max<int>()))
            {
                p.set<int>(v);
            }
        }
    }

    template <size_t Size>
    void Update(lzx::model<Size> &m)
    {
        for (size_t it = 0; it < m.size(); it++)
        {
            Update(m.at(it));
        }
    }
}