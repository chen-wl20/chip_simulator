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
enum conv2d_exec_status
{
	IDLE_conv2d = 0, EXEC_conv2d
};


template <class T>
class CConv2d : public sc_module {
public:
	SC_HAS_PROCESS(CConv2d);
	CConv2d(const sc_module_name& name) :
		sc_module(name) {

		// Test example

		SC_THREAD(conv2d_state_trans);
		sensitive << rst.pos() << prim_finish_reg << conv2d_prim_start.pos();
		dont_initialize();

		SC_THREAD(conv2d_prim_execution);
		sensitive << conv2d_status << rst.pos();
		dont_initialize();

	}

	/*
	State machine
	IDLE_conv2d conv2d not busy
	EXEC busy

	*/
	void conv2d_state_trans() {
		while (true) {
			if (this->rst.read()) {
				this->conv2d_status.write(IDLE_conv2d);
				this->prim_reg.write(Prim_Conv2d());
				this->conv2d_busy.write(false);
			}
			else if (this->prim_finish_reg.read()) {
				this->conv2d_status.write(IDLE_conv2d);
				this->conv2d_busy.write(false);
			}
			else if (conv2d_prim_start.read()) {
				Prim_Conv2d Prim_temp = prim_in.read();
				prim_reg.write(Prim_temp);

				this->conv2d_status.write(EXEC_conv2d);
				this->conv2d_busy.write(true);

			}
			wait();
		}

	}


	void conv2d_prim_execution() {


		while (true) {
			if ((this->conv2d_status.read() == IDLE_conv2d) || this->rst.read()) {
				this->prim_finish_reg.write(false);
			}
			else if (this->conv2d_status == EXEC_conv2d) {
				this->prim_finish_reg.write(false);

				std::cout << std::endl << "Executing Conv2d" << std::endl;

				Prim_Conv2d prim_temp = prim_reg.read();
				std::vector<float> data_in;
				std::vector<float> kernel;
				std::vector<float> result;
				this->load(prim_temp.Input_Addr, int((prim_temp.Input_Height * prim_temp.Input_Width * prim_temp.In_Channels - 1) / 8 + 1), data_in);
				//kernel��in_chָ���仯����out_ch
				this->load(prim_temp.Kernel_Addr, int((prim_temp.Kernel_Size * prim_temp.Kernel_Size * prim_temp.Out_Channels * prim_temp.In_Channels - 1) / 8 + 1), kernel);
				this->conv_2d(prim_temp.Input_Height, prim_temp.Input_Width, prim_temp.Kernel_Size, data_in, kernel, prim_temp.In_Channels, prim_temp.Out_Channels, (prim_temp.Stride + 1), result);
				int size = ceil((prim_temp.Input_Height - prim_temp.Kernel_Size) / (prim_temp.Stride + 1) + 1) * ceil((prim_temp.Input_Width - prim_temp.Kernel_Size) / (prim_temp.Stride + 1));
				this->store(prim_temp.Output_Addr, int((size - 1) / 8) + 1, result);
				this->display_matrix(ceil((prim_temp.Input_Height - prim_temp.Kernel_Size) / (prim_temp.Stride + 1) + 1), ceil((prim_temp.Input_Width - prim_temp.Kernel_Size) / (prim_temp.Stride + 1) + 1), result, 0);
				std::cout << std::endl << "Conv2d Done!" << std::endl;
				this->prim_finish_reg.write(true);
			}
			wait();
		}

	}

public:

	sc_in<Prim_Conv2d> prim_in;
	sc_in<bool> rst;
	sc_in<bool> conv2d_prim_start;
	sc_out<bool> conv2d_busy;
	sc_signal<Prim_Conv2d> prim_reg;
	sc_signal<bool> prim_finish_reg;

	sc_signal<conv2d_exec_status> conv2d_status;
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
	int get_pos_kernel(int i, int j, int c_in, int c_out, int size, int in_channels, int out_channels)
	{
		return c_out * (in_channels * size * size) + c_in * (size * size) + i * size + j;
	}
	float matrix_conv(int height, int width, int kernel_size, std::vector<float>& data_in, std::vector<float>& kernel, int start_i, int start_j, int c_in, int c_out, int in_channels, int out_channels)
	{
		double temp = 0;
		int start_pos = get_pos(c_in, start_i, start_j, height, width);
		for (int i = 0; i < kernel_size; i++)
		{
			for (int j = 0; j < kernel_size; j++)
			{
				temp += double(kernel[get_pos_kernel(i, j, c_in, c_out, kernel_size, in_channels, out_channels)]) *double( data_in[start_pos + j + i * width]);
				//std::cout <<" "<< data_in[start_pos + j + i * width] << " ";
			}
			//std::cout << std::endl;
		}
		return float(temp);
	}

	void conv_2d(int height, int width, int kernel_size, std::vector<float>& data_in, std::vector<float>& kernel, int in_channels, int out_channels, int stride, std::vector<float>& result)
	{
		float temp = 0;

		for (int c_out = 0; c_out < out_channels; c_out++)
		{

			for (int start_i = 0; start_i < height - kernel_size + 1; start_i += stride)
			{

				for (int start_j = 0; start_j < width - kernel_size + 1; start_j += stride)
				{
					temp = 0;
					for (int c_in = 0; c_in < in_channels; c_in++)
					{
						temp += matrix_conv(height, width, kernel_size, data_in, kernel, start_i, start_j, c_in, c_out, in_channels, out_channels);
					}
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
	void display_kernel(int kernel_size, std::vector<float>& data, int c_in, int c_out, int in_channels, int out_channels)
	{
		for (int i = 0; i < kernel_size; i++)
		{
			for (int j = 0; j < kernel_size; j++)
			{
				std::cout << " " << data[get_pos_kernel(i, j, c_in, c_out, kernel_size, in_channels, out_channels)] << " ";
			}
			std::cout << std::endl;
		}

	}


};