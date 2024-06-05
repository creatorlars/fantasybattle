#define ImGui_DEFINE_MATH_OPERATORS

#include <imgui_render_window.hpp>
#include <SFML/Window/Event.hpp>
#include <ImGui-SFML.h>
#include <ImGui.h>

imgui_render_window::imgui_render_window(const char *name, uint32_t width, uint32_t height, uint32_t fps, int32_t x, int32_t y) : lzx::sfml_render_window(name, width, height, fps, x, y)
{
    ImGui::SFML::Init(m_window);
}

imgui_render_window::~imgui_render_window()
{
    ImGui::SFML::Shutdown();
}

void imgui_render_window::update()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed)
        {
            m_window.close();
        }
    }

    ImGui::SFML::Update(m_window, m_clock.restart());
    m_window.clear();
    notify_observers(imgui_render_window_event{});
    ImGui::SFML::Render(m_window);
    m_window.display();
}
