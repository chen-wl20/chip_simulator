#pragma once

#include "systemc.h"
#include "config.h"
#include "mem_if.h"


template<class T>
class dram : public sc_channel, public burst_ram_if<T>
{
private:
	T* mem;
	unsigned int m_start_address;
	unsigned int m_end_address;

public:
	static int random_access_times;
	static int burst_access_times;
	static float energy_consumption;
	static int total_access_latency;
	static float area;
	sc_event* sync_event;
	Trace_event_queue *Trace_event_queue_dram;

public:
	dram(sc_module_name name_, unsigned int start_address, unsigned int end_address, sc_event* _sync_event, Trace_event_queue* _Trace_event_queue_dram)
		:m_start_address(start_address)
		, m_end_address(end_address) {
		//sc_assert(m_end_address >= m_start_addresss);
		mem = new T[m_end_address - m_start_address];
		sync_event = _sync_event;
		Trace_event_queue_dram = _Trace_event_queue_dram;
		int bits = (m_end_address - m_start_address) * sizeof(T) * 8;
		area = 1.0 / 6.0 * (0.001 * std::pow(0.028, 2.07) * std::pow(bits, 0.9) * std::pow(2, 0.7) + 0.0048);
	}
	~dram() {
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
		string name = "Dram_read_event";
		string info = "Event of Dram read";
		string type = "Dram_rdwt_type";
		double time = sc_time_stamp().to_double();
		Trace_event TE_Dram_rdwt(name, info, type, time);
		Trace_event_queue_dram->trace_event_queue.push(TE_Dram_rdwt);
		wait(DRAM_READ_LATENCY, SC_NS);
		random_access_times++;
		total_access_latency += DRAM_READ_LATENCY;
		energy_consumption += DRAM_READ_ENERGY + DRAM_TRANSFER_ENERGY;
		return TRANSFER_OK;
	}
	transfer_status write(unsigned address, T& data) {
		if (address<m_start_address || address>m_end_address) {
			return TRANSFER_ERROR;
		}
		mem[address - m_start_address] = data;
		sync_event->notify();
		string name = "Dram_write_event";
		string info = "Event of Dram write";
		string type = "Dram_rdwt_type";
		double time = sc_time_stamp().to_double();
		Trace_event TE_Dram_rdwt(name, info, type, time);
		Trace_event_queue_dram->trace_event_queue.push(TE_Dram_rdwt);

		wait(DRAM_WRITE_LATENCY, SC_NS);
		random_access_times++;
		total_access_latency += DRAM_WRITE_LATENCY;
		energy_consumption += DRAM_WRITE_ENERGY + DRAM_TRANSFER_ENERGY;

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

	transfer_status burst_read(unsigned int address, unsigned int length, T* &data)
	{
		if (address<m_start_address || address+length>m_end_address) {
			data = NULL;
			return TRANSFER_ERROR;
		}
		data = new T[length];
		for (unsigned i = 0; i < length; i++) {
			data[i] = mem[address + i];
		}
		sync_event->notify();
		string name = "Dram_read_burst_event";
		string info = "Event of Dram burst read";
		string type = "Dram_burst_rdwt_type";
		double time = sc_time_stamp().to_double();
		Trace_event TE_Dram_rdwt(name, info, type, time);
		Trace_event_queue_dram->trace_event_queue.push(TE_Dram_rdwt);

		wait(DRAM_READ_LATENCY + length * DRAM_BURST_READ_LATENCY, SC_NS);
		burst_access_times++;
		total_access_latency += DRAM_READ_LATENCY + length * DRAM_BURST_READ_LATENCY;
		energy_consumption += DRAM_READ_ENERGY + (length - 1) / ((int)DRAM_LINE) * DRAM_READ_ENERGY * DRAM_ENERGY_FACTOR + length * DRAM_TRANSFER_ENERGY;

		return TRANSFER_OK;
	}

	transfer_status burst_write(unsigned int address, unsigned int length, T* &data)
	{
		if (address<m_start_address || address + length>m_end_address) {
			return TRANSFER_ERROR;
		}

		for (unsigned i = 0; i < length; i++) {
			mem[address + i] = data[i];
		}

		sync_event->notify();
		string name = "Dram_write_bust_event";
		string info = "Event of Dram burst write";
		string type = "Dram_burst_rdwt_type";
		double time = sc_time_stamp().to_double();
		Trace_event TE_Dram_rdwt(name, info, type, time);
		Trace_event_queue_dram->trace_event_queue.push(TE_Dram_rdwt);

		wait(DRAM_WRITE_LATENCY + length * DRAM_BURST_WRITE_LATENCY, SC_NS);
		burst_access_times++;
		total_access_latency += DRAM_WRITE_LATENCY + length * DRAM_BURST_WRITE_LATENCY;
		energy_consumption += DRAM_WRITE_ENERGY + (length - 1) / ((int)DRAM_LINE) * DRAM_WRITE_ENERGY * DRAM_ENERGY_FACTOR + length * DRAM_TRANSFER_ENERGY;

		return TRANSFER_OK;
	}

};


template<class T>
int dram<T>::random_access_times = 0;

template<class T>
int dram<T>::burst_access_times = 0;

template<class T>
float dram<T>::energy_consumption = 0;

template<class T>
int dram<T>::total_access_latency = 0;

template<class T>
float dram<T>::area = 0;
