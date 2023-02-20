#pragma once
#include "systemc.h"
#include "ram.h"
#include "dram.h"
#include "Prim.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include "config.h"
enum pooling_exec_status
{
	IDLE_pooling = 0, EXEC_pooling
};


template <class T>
class CPooling : public sc_module {
public:
	SC_HAS_PROCESS(CPooling);
	CPooling(const sc_module_name& name) :
		sc_module(name) {

		// Test example

		SC_THREAD(pooling_state_trans);
		sensitive << rst.pos() << prim_finish_reg << pooling_prim_start.pos();
		dont_initialize();

		SC_THREAD(pooling_prim_execution);
		sensitive << pooling_status << rst.pos();
		dont_initialize();


	}



	/*
	State machine
	IDLE_pooling pooling not busy
	EXEC busy

	*/
	void pooling_state_trans() {
		while (true) {
			if (this->rst.read()) {
				this->pooling_status.write(IDLE_pooling);
				this->prim_reg.write(Prim_Pooling());
				this->pooling_busy.write(false);
			}
			else if (this->prim_finish_reg.read()) {
				this->pooling_status.write(IDLE_pooling);
				this->pooling_busy.write(false);
			}
			else if (pooling_prim_start.read()) {
				Prim_Pooling Prim_temp = prim_in.read();
				prim_reg.write(Prim_temp);

				this->pooling_status.write(EXEC_pooling);
				this->pooling_busy.write(true);

			}
			wait();
		}

	}


	void pooling_prim_execution() {


		while (true) {
			if ((this->pooling_status.read() == IDLE_pooling) || this->rst.read()) {
				this->prim_finish_reg.write(false);
			}
			else if (this->pooling_status == EXEC_pooling) {
				this->prim_finish_reg.write(false);

				std::cout << std::endl << "Executing Pooling" << std::endl;
				Prim_Pooling prim_temp = prim_reg.read();
				std::vector<float> data_in;
				std::vector<float> result;


				this->load(prim_temp.Input_Addr, int((prim_temp.Input_Height * prim_temp.Input_Width * prim_temp.In_Channels - 1) / 8 + 1), data_in);
				this->maxpool_2d(prim_temp.Input_Height, prim_temp.Input_Width, prim_temp.Kernel_Size, data_in, prim_temp.In_Channels, (prim_temp.Stride + 1), result);
				
				int size = ceil((prim_temp.Input_Height - prim_temp.Kernel_Size) / (prim_temp.Stride + 1) + 1) * ceil((prim_temp.Input_Width - prim_temp.Kernel_Size) / (prim_temp.Stride + 1) + 1);
				this->display_matrix(ceil((prim_temp.Input_Height - prim_temp.Kernel_Size) / (prim_temp.Stride + 1) + 1), ceil((prim_temp.Input_Width - prim_temp.Kernel_Size) / (prim_temp.Stride + 1) + 1), result, 0);
				std::cout << "channel_1" << std::endl;
				this->display_matrix(ceil((prim_temp.Input_Height - prim_temp.Kernel_Size) / (prim_temp.Stride + 1) + 1), ceil((prim_temp.Input_Width - prim_temp.Kernel_Size) / (prim_temp.Stride + 1) + 1), result, 1);

				this->store(prim_temp.Output_Addr, int((size - 1) / 8) + 1, result);
				
				std::cout << std::endl << "Pooling Done" << std::endl;
				this->prim_finish_reg.write(true);
			}
			wait();
		}

	}

public:

	sc_in<Prim_Pooling> prim_in;
	sc_in<bool> rst;
	sc_in<bool> pooling_prim_start;
	sc_out<bool> pooling_busy;
	sc_signal<Prim_Pooling> prim_reg;
	sc_signal<bool> prim_finish_reg;

	sc_signal<pooling_exec_status> pooling_status;
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

	int get_pos(int c_in, int i, int j, int height, int width)
	{
		return c_in * height * width + i * width + j;

	}

	float matrix_maxpool(int height, int width, int kernel_size, std::vector<float>& data_in, int start_i, int start_j, int c, int channels)
	{
		float temp = 0;
		//std::cout << kernel_size << std::endl;
		float temp2 = 0;
		int start_pos = get_pos(c, start_i, start_j, height, width);

		for (int i = 0; i < kernel_size; i++)
		{
			for (int j = 0; j < kernel_size; j++)
			{
				temp2 = data_in[start_pos + j + i * width];
				if ((i == 0 && j == 0) || (temp2 > temp))
				{
					temp = temp2;
				}
				//std::cout <<" "<< data_in[start_pos + j + i * width] << " ";
			}
			//std::cout << std::endl;
		}
		//std::cout <<temp<< std::endl;
		return temp;
	}

	void maxpool_2d(int height, int width, int kernel_size, std::vector<float>& data_in, int channels, int stride, std::vector<float>& result)
	{
		float temp = 0;

		for (int c = 0; c < channels; c++)
		{

			for (int start_i = 0; start_i < height - kernel_size + 1; start_i += stride)
			{

				for (int start_j = 0; start_j < width - kernel_size + 1; start_j += stride)
				{
					temp = matrix_maxpool(height, width, kernel_size, data_in, start_i, start_j, c, channels);
					result.push_back(temp);
				}
			}

		}
		int output_size = result.size();
		for (int n = 1; n <= int((output_size - 1) / 8 + 1) * 8 - output_size; n++)
		{
			result.push_back(0);
		}

	}
	void display_matrix(int Height, int Width, std::vector<float>& data, int c_in)
	{
		for (int i = 0; i < Height; i++)
		{
			for (int j = 0; j < Width; j++)
			{
				std::cout << " " << data[get_pos(c_in, i, j, Height, Width)] << " ";
			}
			std::cout << std::endl;
		}

	}


};