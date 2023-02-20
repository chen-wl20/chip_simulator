#pragma once
#include "systemc.h"
#include "ram.h"
#include "dram.h"
#include "Prim.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <random>
#include <ctime>
#include "config.h"

enum matrix_mul_exec_status
{
	IDLE_matrix_mul = 0, EXEC_matrix_mul
};
template <class T>
class CMatrix_Mul : public sc_module {
public:
	SC_HAS_PROCESS(CMatrix_Mul);
	CMatrix_Mul(const sc_module_name& name) :
		sc_module(name) {

		SC_THREAD(matrix_mul_state_trans);
		sensitive << rst.pos() << prim_finish_reg << matrix_mul_prim_start.pos();
		dont_initialize();

		SC_THREAD(matrix_mul_prim_execution);
		sensitive << matrix_mul_status << rst.pos();
		dont_initialize();

	}
	void matrix_mul_state_trans() {
		while (true) {
			if (this->rst.read()) {
				this->matrix_mul_status.write(IDLE_matrix_mul);
				this->prim_reg.write(Prim_Matrix_Mul());
				this->matrix_mul_busy.write(false);
			}
			else if (this->prim_finish_reg.read()) {
				this->matrix_mul_status.write(IDLE_matrix_mul);
				this->matrix_mul_busy.write(false);
			}
			else if (matrix_mul_prim_start.read()) {
				Prim_Matrix_Mul Prim_temp = prim_in.read();
				prim_reg.write(Prim_temp);
				this->matrix_mul_status.write(EXEC_matrix_mul);
				this->matrix_mul_busy.write(true);
			}
			wait();
		}
	}
	void matrix_mul_prim_execution() {
		while (true) {
			if ((this->matrix_mul_status.read() == IDLE_matrix_mul) || this->rst.read()) {
				this->prim_finish_reg.write(false);
			}
			else if (this->matrix_mul_status.read() == EXEC_matrix_mul) {
				this->prim_finish_reg.write(false);
				std::cout << std::endl << "Executing Matrix_Mul" << std::endl;
				Prim_Matrix_Mul prim_temp = prim_reg.read();
				std::vector<float> data_a;
				std::vector<float> data_b;
				//�洢ʱdata_a��data_b������������һλ����0��һ��һ�д�
				this->load(prim_temp.Left_Addr, int((prim_temp.Left_Height * prim_temp.Left_Width - 1) / 8 + 1), data_a);
				this->load(prim_temp.Right_Addr, int((prim_temp.Left_Width * prim_temp.Right_Width - 1) / 8 + 1), data_b);
				std::vector<float> result;
				std::cout << std::endl << "Matrix a:" << std::endl;
				this->display_matrix(prim_temp.Left_Height, prim_temp.Left_Width, data_a);
				std::cout << std::endl << "Matrix b:" << std::endl;
				this->display_matrix(prim_temp.Left_Width, prim_temp.Right_Width, data_b);
				std::cout << std::endl << "Result:" << std::endl;
				this->matrix_mul(prim_temp.Left_Height, prim_temp.Left_Width, prim_temp.Right_Width, data_a, data_b, result);

				this->display_matrix(prim_temp.Left_Height, prim_temp.Right_Width, result);

				this->store(prim_temp.Output_Addr, int((prim_temp.Left_Height * prim_temp.Right_Width - 1) / 8 + 1), result);

				this->prim_finish_reg.write(true);
				std::cout << std::endl << "Done Matrix_Mul!" << std::endl;

			}
			wait();
		}
	}

public:
	sc_in<Prim_Matrix_Mul> prim_in;
	sc_in<bool> rst;
	sc_in<bool> matrix_mul_prim_start;
	sc_out<bool> matrix_mul_busy;
	sc_signal<Prim_Matrix_Mul> prim_reg;
	sc_signal<bool> prim_finish_reg;

	sc_signal<matrix_mul_exec_status> matrix_mul_status;
	sc_port<ram_if<T>> scratchpad_port;

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
		//std::cout << data_length << std::endl;
		for (unsigned i = 0; i < data_length; i++) {
			for (unsigned j = 0; j < PORT_WIDTH / 32; j++) {
				store_data[i].range(31 + j * 32, 0 + j * 32) = convert_to_bv(data_tmp[i * 8 + j]);
			}
			scratchpad_port->write(scratchpad_addr + i, store_data[i]);
		}
	}
	int get_pos(int i, int j, int height, int width)
	{
		return i * width + j;
	}
	void matrix_mul(int LeftHeight, int LeftWidth, int RightWidth, std::vector<float>& data_a, std::vector<float>& data_b, std::vector<float>& result)
	{
		float temp = 0;
		for (int i = 0; i < LeftHeight; i++)
		{
			for (int j = 0; j < RightWidth; j++)
			{
				temp = 0;
				for (int k = 0; k < LeftWidth; k++)
				{
					temp += data_a[get_pos(i, k, LeftHeight, LeftWidth)] * data_b[get_pos(k, j, LeftWidth, RightWidth)];
				}
				result.push_back(temp);
			}
		}
		for (int n = 1; n <= int((LeftHeight * RightWidth - 1) / 8 + 1) * 8 - LeftHeight * RightWidth; n++)
		{
			result.push_back(0);
		}
		//std::cout << result.size() << std::endl;
	}
	void display_matrix(int Height, int Width, std::vector<float>& data)
	{
		for (int i = 0; i < Height; i++)
		{
			for (int j = 0; j < Width; j++)
			{
				std::cout << " " << data[get_pos(i, j, Height, Width)] << " ";
			}
			std::cout << std::endl;
		}

	}
};