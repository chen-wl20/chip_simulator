#pragma once
#include "Cctrl.h"
#include "Cdecode.h"
#include "mem_if.h"
#include "FetchUnit.h"
#include "rom.h"


class Ccontroller : sc_module
{
public:
	SC_HAS_PROCESS(Ccontroller);
	Ccontroller(const sc_module_name& nm,sc_event* _sync_event, Trace_event_queue* _Trace_event_queue_controller)
		: sc_module(nm)
	{	
		sc_bv<PORT_WIDTH>* start_prims = new sc_bv<PORT_WIDTH>[1];
		sync_event = _sync_event;
		Trace_event_queue_controller = _Trace_event_queue_controller;
		start_prims[0] = convertPrim2Code(Prim_Load(SRAM_DEPTH, 1, 0));
		start_prim_rom = new rom<sc_bv<PORT_WIDTH> >("start_prim_rom", 0, 1, start_prims,sync_event,Trace_event_queue_controller);


		decoder = new Cdecode("decoder");
		decoder->prim_code_in(prim_code);
		decoder->reset(reset);
		decoder->prim_type_code(prim_type_code);
		decoder->mem_prim_out(prim_to_mem);
		decoder->ctrl_prim_out(prim_to_ctrl);
		decoder->memristor_prim_out(prim_to_memristor);
		decoder->matrix_mul_prim_out(prim_to_matrix_mul);
		decoder->relu_prim_out(prim_to_relu);
		decoder->conv2d_prim_out(prim_to_conv2d);
		decoder->pooling_prim_out(prim_to_pooling);


		fetcher = new FetchUnit("fetcher",sync_event,Trace_event_queue_controller);
		fetcher->prim_code(prim_code);
		fetcher->prim_read_port(prim_buffer_port);
		fetcher->next_prim(next_prim);
		fetcher->reset(reset);
		fetcher->jump2addr(fetch_jump);
		fetcher->fetch_finish(fetch_finish);
		fetcher->rom_read_port(*start_prim_rom);
		fetcher->from_start(fetch_from_start);
		fetcher->jump_busy(fetch_busy);
		fetcher->fetch_prim_in(prim_to_ctrl);

		prim_ctrl = new Cctrl("prim_ctrl",sync_event,Trace_event_queue_controller);
		prim_ctrl->reset(reset);
		prim_ctrl->prim_type_code(prim_type_code);
		prim_ctrl->next_prim(next_prim);
		prim_ctrl->mem_prim_start(mem_prim_start);
		prim_ctrl->mem_busy_i(mem_busy_i);
		prim_ctrl->pc_jump(fetch_jump);
		prim_ctrl->fetch_finish(fetch_finish);
		prim_ctrl->core_idle(core_idle);
		prim_ctrl->from_start(fetch_from_start);
		prim_ctrl->jump_busy_i(fetch_busy);
		prim_ctrl->memristor_prim_start(memristor_prim_start);
		prim_ctrl->memristor_busy_i(memristor_busy_i);
		prim_ctrl->matrix_mul_prim_start(matrix_mul_prim_start);
		prim_ctrl->matrix_mul_busy_i(matrix_mul_busy_i);
		prim_ctrl->relu_prim_start(relu_prim_start);
		prim_ctrl->relu_busy_i(relu_busy_i);
		prim_ctrl->conv2d_prim_start(conv2d_prim_start);
		prim_ctrl->conv2d_busy_i(conv2d_busy_i);
		prim_ctrl->pooling_prim_start(pooling_prim_start);
		prim_ctrl->pooling_busy_i(pooling_busy_i);
	}

	~Ccontroller();

public:
	Cdecode* decoder;
	Cctrl* prim_ctrl;
	FetchUnit* fetcher;
	rom<sc_bv<PORT_WIDTH> >* start_prim_rom;
	

public:
	sc_port<mem_read_if<sc_bv<PORT_WIDTH> > > prim_buffer_port;
	sc_in<bool> reset;
	sc_out<Prim_Mem> prim_to_mem;
	sc_out<Prim_Memristor> prim_to_memristor;
	sc_out<Prim_Matrix_Mul> prim_to_matrix_mul;
	sc_out<Prim_ReLU> prim_to_relu;
	sc_out<Prim_Conv2d> prim_to_conv2d;
	sc_out<Prim_Pooling> prim_to_pooling;

	sc_out<bool> mem_prim_start;
	sc_in<bool> mem_busy_i;
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

	sc_in<bool> core_idle;

public:
	sc_signal<sc_bv<PORT_WIDTH> > prim_code;
	sc_signal<bool> next_prim;
	sc_signal<sc_bv<4> > prim_type_code;
	sc_signal<bool> fetch_jump;
	sc_signal<bool> fetch_from_start;
	sc_signal<bool> fetch_finish;
	sc_signal<Prim_Jump> prim_to_ctrl;
	sc_signal<bool> fetch_busy;
	sc_event* sync_event;
	Trace_event_queue *Trace_event_queue_controller;
};
