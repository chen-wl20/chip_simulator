#pragma once
#include "config.h"
#include "systemc.h"
#include "mem_if.h"


template<class T>
class rom : public sc_channel, public rom_if<T>
{
private:
	T* mem;
	unsigned int m_start_address;
	unsigned int m_end_address;
public:
	sc_event* sync_event;
	Trace_event_queue *Trace_event_queue_rom;
public:
	rom(sc_module_name name_, unsigned int start_address, unsigned int end_address, T*& data, sc_event* _sync_event,Trace_event_queue * _Trace_event_queue_rom)
		:m_start_address(start_address)
		, m_end_address(end_address) {
		//sc_assert(m_end_address >= m_start_addresss);
		mem = new T[m_end_address - m_start_address];
		sync_event = _sync_event;
		Trace_event_queue_rom = _Trace_event_queue_rom;
		
		for (unsigned i = start_address; i < end_address; i++)
		{
			mem[i - start_address] = data[i - start_address];
		}
	}
	~rom() {
		if (mem) { delete[] mem; }
	}

	virtual void register_port(sc_port_base& port, const char* if_typename) {

	}

	transfer_status read(unsigned address, T& data)
	{
		if (address<m_start_address || address>m_end_address) {
			data = 0;
			return TRANSFER_ERROR;
		}
		data = mem[address - m_start_address];
		sync_event->notify();
		string name = "Rom_write_event";
		string info = "Event of Rom write";
		string type = "Rom_rdwt_type";
		double time = sc_time_stamp().to_double();
		Trace_event TE_Rom_rdwt(name, info, type, time);
		Trace_event_queue_rom->trace_event_queue.push(TE_Rom_rdwt);
		wait(RAM_READ_LATENCY, SC_NS);
		return TRANSFER_OK;
	}

	bool reset() {
		for (unsigned i = 0; i < (m_end_address - m_start_address); i++)
			mem[i] = (T)0;
		return true;
	}

	inline unsigned int start_address()const {
		return m_start_address;
	}

	inline unsigned int end_address()const {
		return m_end_address;
	}

};