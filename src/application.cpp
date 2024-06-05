#include <application.hpp>

application::application()
    // : 
    //   m_gui_render_window("Controls", sf::VideoMode::getDesktopMode().width / 2, sf::VideoMode::getDesktopMode().height, (uint32_t)std::ceil(30.0), -(int)sf::VideoMode::getDesktopMode().width / 4, 0)
{
    // m_gui_render_window.add_observer(*this);
}

void application::run()
{
    // while (m_gui_render_window.is_open())
    // {
    //     m_gui_render_window.update();
    //     lzx::sleep(lzx::milliseconds(1));
    // }
}


// #include <sfml_render_window.hpp>
// #include <SFML/Graphics/RenderWindow.hpp>
// #include <SFML/System/Clock.hpp>
// #include <etl/observer.h>

// struct imgui_render_window_event
// {
// };

// class imgui_render_window : public lzx::sfml_render_window, public etl::observable<etl::observer<const imgui_render_window_event &>, 4>
// {
// public:
//     imgui_render_window(const char *name, uint32_t width, uint32_t height, uint32_t fps, int32_t x, int32_t y);
//     ~imgui_render_window();
//     void update();
    
// private:
//     sf::Clock m_clock;
// };

// #define ImGui_DEFINE_MATH_OPERATORS

// #include <imgui_render_window.hpp>
// #include <SFML/Window/Event.hpp>
// #include <ImGui-SFML.h>
// #include <ImGui.h>

// imgui_render_window::imgui_render_window(const char *name, uint32_t width, uint32_t height, uint32_t fps, int32_t x, int32_t y) : lzx::sfml_render_window(name, width, height, fps, x, y)
// {
//     ImGui::SFML::Init(m_window);
// }

// imgui_render_window::~imgui_render_window()
// {
//     ImGui::SFML::Shutdown();
// }

// void imgui_render_window::update()
// {
//     sf::Event event;
//     while (m_window.pollEvent(event))
//     {
//         ImGui::SFML::ProcessEvent(event);
//         if (event.type == sf::Event::Closed)
//         {
//             m_window.close();
//         }
//     }

//     ImGui::SFML::Update(m_window, m_clock.restart());
//     m_window.clear();
//     notify_observers(imgui_render_window_event{});
//     ImGui::SFML::Render(m_window);
//     m_window.display();
// }
