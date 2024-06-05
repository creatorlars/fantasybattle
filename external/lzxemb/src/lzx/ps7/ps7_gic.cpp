#include "ps7_gic.hpp"

namespace lzx
{

	ps7_gic::ps7_gic(const uint16_t dev_id) : m_initialized(false)
	{
		XScuGic_Config *config = XScuGic_LookupConfig(dev_id);
		if (config != nullptr)
		{
			m_initialized = !(XScuGic_CfgInitialize(&m_ScuGic, config, config->CpuBaseAddress));
			if (m_initialized)
			{
//				// LZX_DBGF("Interrupts Controller %d Init OK, CpuBaseAddr: 0x%08X, DistBaseAddr: 0x%08X", dev_id, config->CpuBaseAddress, config->DistBaseAddress);
				if (!(XScuGic_SelfTest(&m_ScuGic)))
				{
					// LZX_DBGF("Interrupts Controller %d Self Test Success", dev_id);

					Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, (void *)&m_ScuGic);
					Xil_ExceptionEnable();
					// LZX_DBGF("Interrupts Controller Enabled", dev_id);
				}
				else
				{
					// LZX_DBGF("Interrupts Controller %d Self Test Failure", dev_id);
					m_initialized = false;
				}
			}
			else
			{
				// LZX_DBGF("Interrupts Controller %d Init Error", dev_id);
			}
		}
		else
		{
			// LZX_DBGF("Interrupts Controller %d Config Lookup Error", dev_id);
		}
	}

	void ps7_gic::connect(const size_t intr_id, ps7_gic::handler func, void *ptr)
	{
		XScuGic_Connect(&m_ScuGic, intr_id, func, ptr);
	}

	void ps7_gic::disconnect(const size_t intr_id)
	{
		XScuGic_Disconnect(&m_ScuGic, intr_id);
	}

	void ps7_gic::enable(const size_t intr_id)
	{
		XScuGic_Enable(&m_ScuGic, intr_id);
	}

	void ps7_gic::disable(const size_t intr_id)
	{
		XScuGic_Disable(&m_ScuGic, intr_id);
	}

	void ps7_gic::trigger(const size_t intr_id)
	{
		XScuGic_SoftwareIntr(&m_ScuGic, intr_id, XScuGic_GetCpuID());
	}
}
