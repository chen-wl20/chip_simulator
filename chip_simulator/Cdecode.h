#pragma once
#include "systemc.h"
#include "Prim.h"
#include "config.h"


class Cdecode: public sc_module
{
public:
	SC_HAS_PROCESS(Cdecode);
	Cdecode(const sc_module_name& name) :
		sc_module(name) {
		SC_THREAD(decode_prim);
		sensitive << prim_code_in;
		sensitive << reset.pos();
		dont_initialize();

	}
	void decode_prim();
;	~Cdecode();

public:
	sc_in<sc_bv<PORT_WIDTH> > prim_code_in;
	sc_in<bool> reset;

	sc_out<sc_bv<4> > prim_type_code;

	sc_out<Prim_Mem> mem_prim_out;
	sc_out<Prim_Jump> ctrl_prim_out;
	

	sc_out<Prim_Memristor> memristor_prim_out;
	sc_out<Prim_Matrix_Mul> matrix_mul_prim_out;
	sc_out<Prim_ReLU> relu_prim_out;
	sc_out<Prim_Conv2d> conv2d_prim_out;
	sc_out<Prim_Pooling> pooling_prim_out;
};

