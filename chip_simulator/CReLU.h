#pragma once
#include "systemc.h"
#include "ram.h"
#include "dram.h"
#include "Prim.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include "config.h"
enum relu_exec_status
{
	IDLE_relu = 0, EXEC_relu
};


template <class T>
class CReLU : public sc_module {
public:
	SC_HAS_PROCESS(CReLU);
	CReLU(const sc_module_name& name) :
		sc_module(name) {

		// Test example

		SC_THREAD(relu_state_trans);
		sensitive << rst.pos() << prim_finish_reg << relu_prim_start.pos();
		dont_initialize();

		SC_THREAD(relu_prim_execution);
		sensitive << relu_status << rst.pos();
		dont_initialize();


	}

	/*
	State machine
	IDLE_relu relu not busy
	EXEC_relu relu busy

	*/
	void relu_state_trans() {
		while (true) {
			if (this->rst.read()) {
				this->relu_status.write(IDLE_relu);
				this->prim_reg.write(Prim_ReLU());
				this->relu_busy.write(false);
			}
			else if (this->prim_finish_reg.read()) {
				this->relu_status.write(IDLE_relu);
				this->relu_busy.write(false);
			}
			else if (relu_prim_start.read()) {
				Prim_ReLU Prim_temp = prim_in.read();
				prim_reg.write(Prim_temp);

				this->relu_status.write(EXEC_relu);
				this->relu_busy.write(true);

			}
			wait();
		}

	}


	void relu_prim_execution() {


		while (true) {
			if ((this->relu_status.read() == IDLE_relu) || this->rst.read()) {
				this->prim_finish_reg.write(false);
			}
			else if (this->relu_status == EXEC_relu) {
				this->prim_finish_reg.write(false);

				std::cout << std::endl << "Executing Relu" << std::endl;
				Prim_ReLU prim_temp = prim_reg.read();
				std::vector<float> data_in;
				this->load(prim_temp.Input_Addr, int((prim_temp.Data_Length - 1) / 8 + 1), data_in);
				std::cout << "Input" << std::endl;
				for (size_t cnt = 0; cnt < data_in.size(); cnt++)
					std::cout << data_in[cnt] << " ";
				std::cout << std::endl;
				std::cout << "threshold:" << prim_temp.Threshold << std::endl;
				for (int i = 0; i < data_in.size(); i++) {
					if (data_in[i] < prim_temp.Threshold) {
						data_in[i] = prim_temp.Threshold;
					}
				}
				std::cout << "Output" << std::endl;
				for (size_t cnt = 0; cnt < data_in.size(); cnt++)
					std::cout << data_in[cnt] << " ";
				this->store(prim_temp.Output_Addr, int((prim_temp.Data_Length - 1) / 8 + 1), data_in);
				wait(6 + prim_temp.Data_Length, SC_NS);
				std::cout << std::endl << "Relu done!" << std::endl;
				this->prim_finish_reg.write(true);
			}
			wait();
		}

	}

public:

	sc_in<Prim_ReLU> prim_in;
	sc_in<bool> rst;
	sc_in<bool> relu_prim_start;
	sc_out<bool> relu_busy;
	sc_signal<Prim_ReLU> prim_reg;
	sc_signal<bool> prim_finish_reg;

	sc_signal<relu_exec_status> relu_status;
	sc_port<ram_if<T> > scratchpad_port;


private:

	float convert_to_float(sc_bv<32> data)
	{
		int temp = data.to_uint();
		return *reinterpret_cast<float*>(&temp);
	}

	sc_bv<32> convert_to_bv(float data)
	{
		sc_bv<32> temp = (*(uint32_t*)&data);
		return temp;

	}
	void load(unsigned scratchpad_addr, unsigned data_length, std::vector<float>& data_tmp) {

		T* load_data = new T[data_length];
		for (unsigned i = 0; i < data_length; i++) {
			scratchpad_port->read(scratchpad_addr + i, load_data[i]);

		}

		for (unsigned i = 0; i < data_length; i++) {
			for (unsigned j = 0; j < PORT_WIDTH / 32; j++) {
				float temp = convert_to_float(load_data[i].range(31 + j * 32, 0 + j * 32));
				data_tmp.push_back(temp);
				//std::cout << load_data[i].range(31 + j * 32, 0 + j * 32).to_int() << std::endl;
			}
		}
		//return data_tmp;
	}
	void store(unsigned scratchpad_addr, unsigned data_length, std::vector<float>& data_tmp) {
		T* store_data = new T[data_length];
		for (unsigned i = 0; i < data_length; i++) {
			for (unsigned j = 0; j < PORT_WIDTH / 32; j++) {
				store_data[i].range(31 + j * 32, 0 + j * 32) = convert_to_bv(data_tmp[i * 8 + j]);
			}
			scratchpad_port->write(scratchpad_addr + i, store_data[i]);
		}
	}


};
