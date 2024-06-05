#pragma once

namespace lzx
{
    class reactor
    {
    public:
    	virtual ~reactor() {}
        virtual void update() = 0;
    };
}
