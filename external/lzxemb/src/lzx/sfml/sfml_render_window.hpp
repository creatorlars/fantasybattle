#pragma once

#include <lzx/platform/window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

namespace lzx
{
    class sfml_render_window : public window
    {
    public:
        sfml_render_window(const char *name, uint32_t width, uint32_t height, uint32_t fps, int32_t x, int32_t y);
        bool is_open() const override;
        
    protected:
        sf::RenderWindow m_window;
    };
}