#include <application.hpp>
#include <lzx/platform.hpp>

application::application()
    : 
      m_gui_render_window("Controls", sf::VideoMode::getDesktopMode().width / 2, sf::VideoMode::getDesktopMode().height, (uint32_t)std::ceil(30.0), -(int)sf::VideoMode::getDesktopMode().width / 4, 0)
{
    m_gui_render_window.add_observer(*this);
}

void application::run()
{
    while (m_gui_render_window.is_open())
    {
        m_gui_render_window.update();
        lzx::sleep(lzx::milliseconds(1));
    }
}

// void application::notification(const video_simulation_renderer_event &evt)
// {
//     // LZXEMB_DBG("video_simulation_renderer_event, hramp: %f, vramp: %f, red: %f, green: %f, blue: %f\r\n", evt.hramp, evt.vramp, *evt.red, *evt.green, *evt.blue);
//     m_video_simulation_engine.update(evt.frame, evt.hramp, evt.vramp, evt.red, evt.green, evt.blue, evt.x, evt.y, evt.z, evt.img_red, evt.img_green, evt.img_blue, evt.feedback_red, evt.feedback_green, evt.feedback_blue);
// }

// void application::notification(const video_render_window_event &evt)
// {
    // LZXEMB_DBG("video_render_window_event, data: %d, width: %d, height: %d, frame: %d\r\n", (uint32_t) evt.data, evt.width, evt.height, evt.frame);
    // m_video_simulation_renderer.render_frame(evt.data, evt.width, evt.height, evt.frame);
// }

void application::notification(const imgui_render_window_event &evt)
{
    // m_video_simulation_renderer.update_gui();
    // m_video_simulation_engine.update_gui();
}
