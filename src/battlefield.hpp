#pragma once

namespace fantasybattle
{

    class battlefield_system
    {
    public:
        battlefield_system() : m_width(8.0 * 12.0), m_height(4.0 * 12.0) {}

    private:
        double m_width;
        double m_height;
    };

}