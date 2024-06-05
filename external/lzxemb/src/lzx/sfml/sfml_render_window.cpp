#include "sfml_render_window.hpp"

namespace lzx
{

    sfml_render_window::sfml_render_window(const char *name, uint32_t width, uint32_t height, uint32_t fps, int32_t x, int32_t y) : m_window(sf::VideoMode(width, height), name)
    {
        m_window.setPosition(m_window.getPosition() + sf::Vector2i(x, y));
        m_window.setFramerateLimit(fps);
    }

    bool sfml_render_window::is_open() const
    {
        return m_window.isOpen();
    }

}