#pragma once
#ifndef _RAM_H
#define _RAM_H
#include "config.h"
#include "systemc.h"
#include "mem_if.h"
#include <cmath>
#include "Trace_event_queue.h"


template<class T>
class ram : public sc_channel,public ram_if<T>
{
private:
	T* mem;
	unsigned int m_start_address;
	unsigned int m_end_address;

public:
	static int random_access_times;
	static float energy_consumption;
	static int total_access_latency;
	static float area;
	sc_event* sync_event;
	Trace_event_queue *Trace_event_queue_ram;

public:
	ram(sc_module_name name_, unsigned int start_address,unsigned int end_address, sc_event * _sync_event, Trace_event_queue * _Trace_event_queue_ram)
		:m_start_address(start_address)
		,m_end_address(end_address) {
		//sc_assert(m_end_address >= m_start_addresss);
		mem = new T[m_end_address - m_start_address];
		sync_event = _sync_event;
		Trace_event_queue_ram = _Trace_event_queue_ram;

		int bits = (m_end_address - m_start_address) * sizeof(T) * 8;
		area = 0.001 * std::pow(0.028, 2.07) * std::pow(bits, 0.9) * std::pow(2, 0.7) + 0.0048;
	}
	~ram() {
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
		string name = "Sram_read_event";
		string info = "Event of Sram read";
		string type = "Sram_rdwt_type";
		double time = sc_time_stamp().to_double();
		Trace_event TE_Sram_rdwt(name, info, type, time);
		Trace_event_queue_ram->trace_event_queue.push(TE_Sram_rdwt);
		wait(RAM_READ_LATENCY, SC_NS);
		random_access_times++;
		total_access_latency += RAM_READ_LATENCY;
		energy_consumption += RAM_READ_ENERGY;

		return TRANSFER_OK;
	}

	transfer_status write(unsigned address, T& data) {
		if (address<m_start_address || address>m_end_address) {
			return TRANSFER_ERROR;
		}
		mem[address - m_start_address] = data;
		sync_event->notify();
		string name = "Sram_write_event";
		string info = "Event of Sram write";
		string type = "Sram_rdwt_type";
		double time = sc_time_stamp().to_double();
		Trace_event TE_Sram_rdwt(name, info, type, time);
		Trace_event_queue_ram->trace_event_queue.push(TE_Sram_rdwt);
		wait(RAM_WRITE_LATENCY, SC_NS);
		random_access_times++;
		total_access_latency += RAM_WRITE_LATENCY;
		energy_consumption += RAM_WRITE_ENERGY;

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

template<class T>
int ram<T>::random_access_times = 0;

template<class T>
float ram<T>::area = 0;

template<class T>
float ram<T>::energy_consumption = 0;

template<class T>
int ram<T>::total_access_latency = 0;


#endif
