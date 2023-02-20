#pragma once
#include "mem_if.h"
#include "config.h"


class FetchUnit : public sc_module {
public:
	SC_HAS_PROCESS(FetchUnit);
	FetchUnit(sc_module_name nm,sc_event* _sync_event, Trace_event_queue * _Trace_event_queue_FetchUnit)
		: sc_module(nm)
	{
		pc = ROM_START_PC;
		sync_event = _sync_event;
		Trace_event_queue_FetchUnit = _Trace_event_queue_FetchUnit;

		SC_THREAD(read_prim);
		sensitive << reset.pos() << next_prim.pos();
		dont_initialize();

		SC_THREAD(jump);
		sensitive << reset.pos() << fetch_finish.pos() << jump2addr.pos() << from_start.pos();
		dont_initialize();
	}

	void read_prim()
	{
		while (true) {
			if (this->reset.read()) {
				prim_code.write(0);
				fetch_finish.write(false);
			}
			else if (this->next_prim.read()) {
				fetch_finish.write(false);
				sc_bv<PORT_WIDTH> prim_temp;
				if (pc < 0) {
					rom_read_port->read(pc + rom_read_port->end_address(), prim_temp);
				}
				else {
					prim_read_port->read(pc, prim_temp);
					//std::cout << prim_temp << std::endl;
				}
				//wait(1, SC_NS);s
				prim_code.write(prim_temp);
				fetch_finish.write(true);
			}
			wait();
		}
	}

	void jump()
	{
		while (true) {
			if (this->reset.read()) {
				pc = ROM_START_PC;
			}
			else if (this->from_start.read()) {
				jump_busy.write(true);
				pc = ROM_START_PC;
				sync_event->notify();
				string name = "Jump_prim_event";
				string info = "Event of Jump prim";
				string type = "Jump_prim_type";
				double time = sc_time_stamp().to_double();
				Trace_event TE_Jump_prim(name, info, type, time);
				Trace_event_queue_FetchUnit->trace_event_queue.push(TE_Jump_prim);
				wait(1, SC_NS);
				jump_busy.write(false);
			}
			else if (this->jump2addr) {
				jump_busy.write(true);
				Prim_Jump temp = fetch_prim_in.read();
				pc += temp.jump_addr - 1;
				sync_event->notify();
				string name = "Jump_prim_event";
				string info = "Event of Jump prim";
				string type = "Jump_prim_type";
				double time = sc_time_stamp().to_double();
				Trace_event TE_Jump_prim(name, info, type, time);
				Trace_event_queue_FetchUnit->trace_event_queue.push(TE_Jump_prim);
				wait(1, SC_NS);
				jump_busy.write(false);
			}
			else if (this->fetch_finish.read()) {
				pc++;
			}
			wait();
		}
	}

public:
	sc_int<16> pc;

public:
	sc_out<sc_bv<PORT_WIDTH> > prim_code;

	sc_port<mem_read_if<sc_bv<PORT_WIDTH> > > prim_read_port;
	sc_port<rom_if<sc_bv<PORT_WIDTH> > > rom_read_port;

	sc_in<bool> from_start;
	sc_in<bool> reset;
	sc_in<bool> next_prim;
	sc_in<bool> jump2addr;
	sc_in<Prim_Jump> fetch_prim_in;

	sc_out<bool> fetch_finish;
	sc_out<bool> jump_busy;
	sc_event* sync_event;
	Trace_event_queue *Trace_event_queue_FetchUnit;
	
};
