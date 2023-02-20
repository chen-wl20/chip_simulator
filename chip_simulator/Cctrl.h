#pragma once
#include "systemc.h"
#include "Prim.h"
#include "config.h"
#include "Trace_event.h"
#include "Trace_event_queue.h"


enum controller_status
{CORE_IDLE = 0, PROCESSING, EXECUTING};

class Cctrl : public sc_module
{
public:
	SC_HAS_PROCESS(Cctrl);
	Cctrl(const sc_module_name &name,sc_event* _sync_event, Trace_event_queue *_Trace_event_queue_ctrl) :
		sc_module(name) 
	{
		sync_event = _sync_event;
		Trace_event_queue_ctrl = _Trace_event_queue_ctrl;
		SC_THREAD(core_busy_combinational_logic);
		sensitive << mem_busy_i << jump_busy_i << memristor_busy_i<<matrix_mul_busy_i<<relu_busy_i<<conv2d_busy_i<<pooling_busy_i;
		dont_initialize();

		SC_THREAD(fetch_and_decode);
		sensitive << ctrl_status << reset.pos();
		dont_initialize();

		SC_THREAD(dispatch_prim);
		sensitive << ctrl_status << reset.pos();
		dont_initialize();

		SC_THREAD(ctrl_status_trans);
		sensitive << reset.pos();
		sensitive << core_busy.negedge_event() << fetch_finish.pos() << core_idle.neg() << to_idle.posedge_event();
		dont_initialize();

		SC_THREAD(core_busy_event);
		sensitive << reset.pos();
		sensitive << core_busy.posedge_event();


		SC_THREAD(core_idle_event);
		sensitive << reset.pos();
		sensitive << core_busy.negedge_event();

	}
	~Cctrl();


	void dispatch_prim();
	void fetch_and_decode();
	void core_busy_combinational_logic();
	void ctrl_status_trans();
	void core_busy_event();
	void core_idle_event();

public:
	sc_in<bool> reset;
	sc_in<bool> core_idle;

	// from and to decoder
	sc_in<sc_bv<4> > prim_type_code;

	// from and to fetcher
	sc_out<bool> next_prim;
	sc_out<bool> pc_jump;
	sc_out<bool> from_start;
	sc_in<bool> fetch_finish;

	// from and to execution units
	sc_out<bool> mem_prim_start;
	sc_in<bool> mem_busy_i;
	sc_in<bool> jump_busy_i;
	sc_out<bool> memristor_prim_start;
	sc_in<bool> memristor_busy_i;
	sc_out<bool> matrix_mul_prim_start;
	sc_in<bool> matrix_mul_busy_i;
	sc_out<bool> relu_prim_start;
	sc_in<bool> relu_busy_i;
	sc_out<bool> conv2d_prim_start;
	sc_in<bool> conv2d_busy_i;
	sc_out<bool> pooling_prim_start;
	sc_in<bool> pooling_busy_i;

	// controller signals
	sc_signal<bool> core_busy;
	sc_signal<bool> dispatching;
	sc_signal<controller_status> ctrl_status;
	sc_signal<bool> to_idle;
	Trace_event_queue *Trace_event_queue_ctrl;
	sc_event* sync_event;

};

