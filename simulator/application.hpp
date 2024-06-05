#pragma once

#include <imgui_render_window.hpp>

class application : public etl::observer<const imgui_render_window_event &>
{
public:
    application();
    void run();

private:
    void notification(const imgui_render_window_event &);

    imgui_render_window m_gui_render_window;
};