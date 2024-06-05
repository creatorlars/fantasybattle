#pragma once

#include <lzx/platform.hpp>
#include <xscugic.h>

namespace lzx
{
	class ps7_gic
	{
	public:
		using handler = void (*)(void *);
		void connect(const size_t id, handler func, void *ptr);
		void disconnect(const size_t id);
		void enable(const size_t id);
		void disable(const size_t id);
		void trigger(const size_t id);
		ps7_gic(const uint16_t);

	private:
		bool m_initialized;
		XScuGic m_ScuGic;
	};

}
