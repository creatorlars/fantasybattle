#pragma once

#include <sfml_render_window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <etl/observer.h>

struct imgui_render_window_event
{
};

class imgui_render_window : public lzx::sfml_render_window, public etl::observable<etl::observer<const imgui_render_window_event &>, 4>
{
public:
    imgui_render_window(const char *name, uint32_t width, uint32_t height, uint32_t fps, int32_t x, int32_t y);
    ~imgui_render_window();
    void update();
    
private:
    sf::Clock m_clock;
};
