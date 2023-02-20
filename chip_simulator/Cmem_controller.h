#pragma once
#include "ram.h"
#include "dram.h"
#include "Prim.h"

enum mem_controller_status 
{ IDLE = 0, LOAD, STORE };

template <class T>
class Cmem_controller : public sc_channel, public ram_if<T> {
public:
	SC_HAS_PROCESS(Cmem_controller);
	Cmem_controller(sc_module_name nm, unsigned start_address, unsigned end_address, sc_event* _sync_event,Trace_event_queue * _Trace_event_queue_Cmem)
		: sc_module(nm), m_start_address(start_address), m_end_address(end_address)
	{
		sc_assert(end_address <= 0b1111111111111111);
		sync_event = _sync_event;
		Trace_event_queue_Cmem = _Trace_event_queue_Cmem;
		Scratchpad0 = new ram<T >("Scratchpad0", m_start_address, m_end_address, sync_event, Trace_event_queue_Cmem);

		SC_THREAD(mem_state_trans);
		sensitive << rst.pos() << prim_finish_reg << mem_prim_start.pos();
		dont_initialize();

		SC_THREAD(mem_prim_execution);
		sensitive << mem_status << rst.pos();
		dont_initialize();
	}

	transfer_status read(unsigned int address, T& data)
	{
		return Scratchpad0->read(address, data);
	}
	transfer_status write(unsigned int address, T& data) 
	{
		return Scratchpad0->write(address, data);
	}
	bool reset() {
		return Scratchpad0->reset();
	}

	void mem_state_trans() {
		while (true) {
			if (this->rst.read()) {
				this->mem_status.write(IDLE);
				this->prim_reg.write(Prim_Mem());
				this->mem_busy.write(false);
			}
			else if (this->prim_finish_reg.read()) {
				this->mem_status.write(IDLE);
				this->mem_busy.write(false);
			}
			else if (mem_prim_start.read()) {
				Prim_Mem Prim_temp = prim_in.read();
				prim_reg.write(Prim_temp);
				if (Prim_temp.type_name.compare(std::string("Load")) == 0) {
					this->mem_status.write(LOAD);
					this->mem_busy.write(true);
				}
				else if (Prim_temp.type_name.compare(std::string("Store")) == 0) {
					this->mem_status.write(STORE);
					this->mem_busy.write(true);
				}
			}
			wait();
		}
	}

	void mem_prim_execution() {
		while (true) {
			if ((this->mem_status.read() == IDLE) || this->rst.read()) {
				this->prim_finish_reg.write(false);
			}
			else if (this->mem_status == LOAD) {
				this->prim_finish_reg.write(false);
				Prim_Mem prim_temp = prim_reg.read();
				this->load(prim_temp.Scrachpad_Addr, prim_temp.Data_Length, prim_temp.Dram_Addr);
				//wait(6 + prim_temp.Data_Length, SC_NS);
				this->prim_finish_reg.write(true);
			}
			else if (this->mem_status == STORE) {
				this->prim_finish_reg.write(false);
				Prim_Mem prim_temp = prim_reg.read();
				this->store(prim_temp.Scrachpad_Addr, prim_temp.Data_Length, prim_temp.Dram_Addr);
				//wait(6 + prim_temp.Data_Length, SC_NS);
				this->prim_finish_reg.write(true);
			}
			wait();
		}
	}

	inline unsigned int start_address()const {
		return m_start_address;
	}

	inline unsigned int end_address()const {
		return m_end_address;
	}

private:
	void load(unsigned scratchpad_addr, unsigned data_length, unsigned dram_addr)
	{
		T* load_data = nullptr;
		dram_port->burst_read(dram_addr, data_length, load_data);
		if (scratchpad_addr < this->m_end_address) {
			for (unsigned i = 0; i < data_length; i++) {
				Scratchpad0->write(scratchpad_addr + i, load_data[i]);
				
			}
		}
		else {
			for (unsigned i = 0; i < data_length; i++) {
				prim_buffer_port->write(scratchpad_addr - (this->m_end_address) + i, load_data[i]);

				//std::cout << load_data[i] << endl;
			}
		}
	}

	void store(unsigned scratchpad_addr, unsigned data_length, unsigned dram_addr)
	{
		T* store_data = new T[data_length];
		for (unsigned i = 0; i < data_length; i++) {
			Scratchpad0->read(scratchpad_addr + i, store_data[i]);
		}
		dram_port->burst_write(dram_addr, data_length, store_data);
	}

public:
	sc_port<burst_ram_if<T> > dram_port;
	sc_in<Prim_Mem> prim_in;
	sc_in<bool> rst;
	sc_in<bool> mem_prim_start;
	sc_out<bool> mem_busy;
	sc_port<mem_write_if<T> > prim_buffer_port;

	sc_signal<mem_controller_status> mem_status;
	sc_signal<Prim_Mem> prim_reg;
	sc_signal<bool> prim_finish_reg;
	

public:
	ram<T>* Scratchpad0;
	unsigned int m_start_address;
	unsigned int m_end_address;
	sc_event* sync_event;
	Trace_event_queue *Trace_event_queue_Cmem;

};
