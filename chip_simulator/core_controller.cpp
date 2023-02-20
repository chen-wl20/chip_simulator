#include "Cmem_controller.h"
#include "Ccontroller.h"
#include "controller_tb.h"
#include<iostream>
#include "CMemristor.h"
#include "Event_engine.h"
#include "dram.h"
#include "CMatrix_Mul.h"
#include "CReLU.h"
#include "CConv2d.h"
#include "CPooling.h"
int sc_main(int argc, char* argv[])
{
	sc_signal<bool> mem_ctrl_busy;
	sc_signal<bool> memristor_ctrl_busy;
	sc_signal<bool> matrix_mul_ctrl_busy;
	sc_signal<bool> relu_ctrl_busy;
	sc_signal<bool> conv2d_ctrl_busy;
	sc_signal<bool> pooling_ctrl_busy;


	sc_signal<bool> reset;
	sc_signal<Prim_Mem> prim_signal;
	sc_signal<Prim_Memristor> prim_memristor_signal;
	sc_signal<Prim_Matrix_Mul>prim_matrix_mul_signal;
	sc_signal<Prim_ReLU> prim_relu_signal;
	sc_signal<Prim_Conv2d> prim_conv2d_signal;
	sc_signal<Prim_Pooling> prim_pooling_signal;

	sc_signal<bool> mem_exe_start;
	sc_signal<bool> memristor_exe_start;
	sc_signal<bool> matrix_mul_exe_start;
	sc_signal<bool> relu_exe_start;
	sc_signal<bool> conv2d_exe_start;
	sc_signal<bool> pooling_exe_start;

	sc_signal<bool> core_idle;

	sc_event* sync_event;
	sync_event = new sc_event;


	Trace_event_queue* trace_event_queue_tb;
	trace_event_queue_tb = new Trace_event_queue;

	dram<sc_bv<PORT_WIDTH> > offchip_dram("offchip_dram", 0, 10000,sync_event, trace_event_queue_tb);
	ram<sc_bv<PORT_WIDTH> > prim_buffer("prim_buffer", 0, 100,sync_event,trace_event_queue_tb);
	
	Event_engine Event_engine_chip("my_event_engine");
	Event_engine_chip.sync_event = sync_event;
	Event_engine_chip.Trace_event_queue_clock_engine = trace_event_queue_tb;

	Cmem_controller<sc_bv<PORT_WIDTH> > mem_ctrl("mem_crtl", 0, SRAM_DEPTH, sync_event,trace_event_queue_tb);
	mem_ctrl.dram_port(offchip_dram);
	mem_ctrl.rst(reset);
	mem_ctrl.prim_in(prim_signal);
	mem_ctrl.mem_prim_start(mem_exe_start);
	mem_ctrl.mem_busy(mem_ctrl_busy);
	mem_ctrl.prim_buffer_port(prim_buffer);

	
	CMemristor<sc_bv<PORT_WIDTH> > CMemristor_1("CMemristor_1");
	CMemristor_1.rst(reset);
	CMemristor_1.memristor_prim_start(memristor_exe_start);
	CMemristor_1.prim_in(prim_memristor_signal);
	CMemristor_1.memristor_busy(memristor_ctrl_busy);
	CMemristor_1.scratchpad_port(*mem_ctrl.Scratchpad0);

	CMatrix_Mul<sc_bv<PORT_WIDTH>> CMatrix_Mul_1("CMatrix_Mul_1");
	CMatrix_Mul_1.rst(reset);
	CMatrix_Mul_1.matrix_mul_prim_start(matrix_mul_exe_start);
	CMatrix_Mul_1.prim_in(prim_matrix_mul_signal);
	CMatrix_Mul_1.matrix_mul_busy(matrix_mul_ctrl_busy);
	CMatrix_Mul_1.scratchpad_port(*mem_ctrl.Scratchpad0);

	CReLU <sc_bv<PORT_WIDTH>> CReLU_1("CReLU_1");
	CReLU_1.rst(reset);
	CReLU_1.relu_prim_start(relu_exe_start);
	CReLU_1.prim_in(prim_relu_signal);
	CReLU_1.relu_busy(relu_ctrl_busy);
	CReLU_1.scratchpad_port(*mem_ctrl.Scratchpad0);

	CConv2d <sc_bv<PORT_WIDTH>> CConv2d_1("CConv2d_1");
	CConv2d_1.rst(reset);
	CConv2d_1.conv2d_prim_start(conv2d_exe_start);
	CConv2d_1.prim_in(prim_conv2d_signal);
	CConv2d_1.conv2d_busy(conv2d_ctrl_busy);
	CConv2d_1.scratchpad_port(*mem_ctrl.Scratchpad0);

	CPooling <sc_bv<PORT_WIDTH>> CPooling_1("CPooling_1");
	CPooling_1.rst(reset);
	CPooling_1.pooling_prim_start(pooling_exe_start);
	CPooling_1.prim_in(prim_pooling_signal);
	CPooling_1.pooling_busy(pooling_ctrl_busy);
	CPooling_1.scratchpad_port(*mem_ctrl.Scratchpad0);

	Ccontroller core_ctrl("core_ctrl",sync_event,trace_event_queue_tb);
	core_ctrl.mem_busy_i(mem_ctrl_busy);
	core_ctrl.reset(reset);
	core_ctrl.prim_buffer_port(prim_buffer);
	core_ctrl.prim_to_mem(prim_signal);
	core_ctrl.mem_prim_start(mem_exe_start);
	core_ctrl.core_idle(core_idle);
	core_ctrl.memristor_prim_start(memristor_exe_start);
	core_ctrl.memristor_busy_i(memristor_ctrl_busy);
	core_ctrl.prim_to_memristor(prim_memristor_signal);
	core_ctrl.matrix_mul_prim_start(matrix_mul_exe_start);
	core_ctrl.matrix_mul_busy_i(matrix_mul_ctrl_busy);
	core_ctrl.prim_to_matrix_mul(prim_matrix_mul_signal);
	core_ctrl.relu_prim_start(relu_exe_start);
	core_ctrl.relu_busy_i(relu_ctrl_busy);
	core_ctrl.prim_to_relu(prim_relu_signal);
	core_ctrl.conv2d_prim_start(conv2d_exe_start);
	core_ctrl.conv2d_busy_i(conv2d_ctrl_busy);
	core_ctrl.prim_to_conv2d(prim_conv2d_signal);
	core_ctrl.pooling_prim_start(pooling_exe_start);
	core_ctrl.pooling_busy_i(pooling_ctrl_busy);
	core_ctrl.prim_to_pooling(prim_pooling_signal);

	//core_ctrl.prim_ctrl->Trace_event_queue_ctrl = trace_event_queue_tb;
	//core_ctrl.prim_ctrl->sync_event = sync_event;

	controller_tb tb("tb");
	tb.rst(reset);
	tb.idle(core_idle);
	tb.dram_port(offchip_dram);
	tb.prim_buffer_port(prim_buffer);
	
	sc_trace_file* tf = sc_create_vcd_trace_file("controller");
	sc_trace(tf, reset, "reset");
	sc_trace(tf, mem_exe_start, "mem_exe_start");
	sc_trace(tf, core_idle, "core_idle");
	sc_trace(tf, mem_ctrl_busy, "mem_busy");
	sc_trace(tf, CMemristor_1.memristor_prim_start, "memristor_prim_start");
	sc_trace(tf, CMemristor_1.memristor_busy, "memristor_busy");
	sc_trace(tf, core_ctrl.prim_type_code, "prim_type_code");
	sc_trace(tf, core_ctrl.prim_ctrl->core_busy, "core_busy");
	sc_trace(tf, CMatrix_Mul_1.matrix_mul_prim_start, "matrix_prim_start");
	sc_trace(tf, CMatrix_Mul_1.matrix_mul_busy, "matrix_busy");
	sc_trace(tf, CReLU_1.relu_prim_start, "relu_prim_start");
	sc_trace(tf, CReLU_1.relu_busy, "relu_busy");
	sc_trace(tf, CConv2d_1.conv2d_prim_start, "conv2d_prim_start");
	sc_trace(tf, CConv2d_1.conv2d_busy, "conv2d_busy");
	sc_trace(tf, CPooling_1.pooling_prim_start, "pooling_prim_start");
	sc_trace(tf, CPooling_1.pooling_busy, "pooling_busy");

	sc_trace(tf, core_ctrl.prim_ctrl->pc_jump, "jump_sig");
	sc_trace(tf, core_ctrl.prim_ctrl->jump_busy_i, "jump_finish_sig");
	sc_trace(tf, core_ctrl.prim_ctrl->to_idle, "convert2idle");

	//// 预先写好数据
	//sc_bv<128> old_data = 0x1;
	//std::cout << "old: " << old_data << std::endl;
	//offchip_dram.write(20, old_data);
	sc_start();
	std::cout << "DRAM random access times: " << dram<sc_bv<PORT_WIDTH> >::random_access_times << std::endl;
	std::cout << "DRAM burst access times: " << dram<sc_bv<PORT_WIDTH> >::burst_access_times << std::endl;
	std::cout << "DRAM area: " << dram<sc_bv<PORT_WIDTH> >::area << std::endl;
	std::cout << "DRAM refresh energy: " << dram<sc_bv<PORT_WIDTH> >::area * tb.simulater_time * DRAM_REFRESH_POWER<< std::endl;
	std::cout << "DRAM total access latency: " << dram<sc_bv<PORT_WIDTH> >::total_access_latency << std::endl;
	std::cout << "DRAM energy consumption: " << dram<sc_bv<PORT_WIDTH> >::energy_consumption << std::endl;
	std::cout << "SRAM random access times: " << ram<sc_bv<PORT_WIDTH> >::random_access_times << std::endl;
	std::cout << "SRAM area: " << ram<sc_bv<PORT_WIDTH> >::area << std::endl;
	std::cout << "SRAM static energy: " << ram<sc_bv<PORT_WIDTH> >::area * tb.simulater_time * RAM_STATIC_POWER << std::endl;
	std::cout << "SRAM total access latency: " << ram<sc_bv<PORT_WIDTH> >::total_access_latency << std::endl;
	std::cout << "SRAM energy consumption: " << ram<sc_bv<PORT_WIDTH> >::energy_consumption << std::endl;
	//// 查看写回数据是否正确
	//offchip_dram.read(5, old_data);
	//std::cout << "after: " << old_data << std::endl;
	
	while (trace_event_queue_tb->trace_event_queue.size() != 0)
	{
		if (trace_event_queue_tb->trace_event_queue.front().type == "core_busy_type") {
			cout << "Event time " << trace_event_queue_tb->trace_event_queue.front().time << endl;
			cout << "Event info " << trace_event_queue_tb->trace_event_queue.front().info << endl;
			trace_event_queue_tb->trace_event_queue.pop();
		}else
		{
			cout << "Event time " << trace_event_queue_tb->trace_event_queue.front().time << endl;
			cout << "Event info " << trace_event_queue_tb->trace_event_queue.front().info << endl;
			trace_event_queue_tb->trace_event_queue.pop();
		}

	}

	cout << "Core busy time " << Event_engine_chip.core_busy_time << "ps" << endl;
	cout << "Core idle time " << Event_engine_chip.core_idle_time << "ps" << endl;
	delete sync_event;

	//sc_stop();

	

	sc_close_vcd_trace_file(tf);
	delete trace_event_queue_tb;

	return 0;
}


