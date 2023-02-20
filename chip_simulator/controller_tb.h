#pragma once
#include "Ccontroller.h"
#include "dram.h"
#include "Trace_event_queue.h"
#include <iostream>
#include <fstream>

class controller_tb : public sc_module {
public:
	SC_HAS_PROCESS(controller_tb);
	controller_tb(sc_module_name nm)
		: sc_module(nm)
	{
		SC_THREAD(test_mem_ctrl);
	}

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

	void test_mem_ctrl() {
		/*{	//load x7 and writeto dram;
			int datalen = 0;
			float num[84];
			ifstream file("../x7.txt");
			while (!file.eof()) {
				file >> num[datalen++];
			}
			sc_bv<PORT_WIDTH>* data = new sc_bv<PORT_WIDTH>[11];//(sc_bv<PORT_WIDTH>*)malloc(sizeof(sc_bv<PORT_WIDTH>)*11);
			for (int i = 0; i <int((84 - 1) / 8 + 1); i++) {
				sc_bv<PORT_WIDTH> temp;
				for (int j = 0; j < 8; j++) {
					if (j + i * 8 < 84) {
						temp.range(31 + j * 32, j * 32) = convert_to_bv(num[i * 8 + j]);
					}
					else {
						temp.range(31 + j * 32, j * 32) = sc_bv<32>(0);
					}
				}
				data[i] = temp;
			}
			dram_port->burst_write(100, 11, data);
			delete[] data;
		}*/
		/*{
			//load x6 and write to dram;
			int length = 120;
			int datalen = 0;
			float num[120];
			ifstream file("../x6.txt");
			while (!file.eof()) {
				file >> num[datalen++];
			}
			sc_bv<PORT_WIDTH>* data = new sc_bv<PORT_WIDTH>[15];//(sc_bv<PORT_WIDTH>*)malloc(sizeof(sc_bv<PORT_WIDTH>) * 105);
			for (int i = 0; i <int((length - 1) / 8 + 1); i++) {
				sc_bv<PORT_WIDTH> temp;
				for (int j = 0; j < 8; j++) {
					if (j + i * 8 < length) {
						temp.range(31 + j * 32, j * 32) = convert_to_bv(num[i * 8 + j]);
					}
					else {
						temp.range(31 + j * 32, j * 32) = sc_bv<32>(0);
					}
				}
				data[i] = temp;
			}
			dram_port->burst_write(112, 15, data);
			delete[] data;
		}*/
		{
			//load x5 and write to dram;
			std::cout << "reading x5" << std::endl;
			int length = 400;
			int datalen = 0;
			float num[400];
			ifstream file("../x5.txt");
			while (!file.eof()) {
				file >> num[datalen++];
			}
			file.close();
			sc_bv<PORT_WIDTH>* data = new sc_bv<PORT_WIDTH>[50];//(sc_bv<PORT_WIDTH>*)malloc(sizeof(sc_bv<PORT_WIDTH>) * 105);
			for (int i = 0; i <int((length - 1) / 8 + 1); i++) {
				sc_bv<PORT_WIDTH> temp;
				for (int j = 0; j < 8; j++) {
					if (j + i * 8 < length) {
						temp.range(31 + j * 32, j * 32) = convert_to_bv(num[i * 8 + j]);
					}
					else {
						temp.range(31 + j * 32, j * 32) = sc_bv<32>(0);
					}
				}
				data[i] = temp;
			}
			dram_port->burst_write(128, 50, data);
			
			std::cout << "length for x5 " << datalen << std::endl;
			std::cout << "data"<<num[0] << std::endl;
			std::cout << "data" << convert_to_float(data[0].range(31, 0)) << std::endl;
			delete[] data;
		}
		/*
		{
			//load x4 and write to dram;
			std::cout << "reading x4" << std::endl;
			int length = 1600;
			int datalen = 0;
			float num[1600];
			ifstream file("../x4.txt");
			while (!file.eof()) {
				file >> num[datalen++];
			}
			file.close();
			sc_bv<PORT_WIDTH>* data = new sc_bv<PORT_WIDTH>[200];//(sc_bv<PORT_WIDTH>*)malloc(sizeof(sc_bv<PORT_WIDTH>) * 105);
			for (int i = 0; i <int((length - 1) / 8 + 1); i++) {
				sc_bv<PORT_WIDTH> temp;
				for (int j = 0; j < 8; j++) {
					if (j + i * 8 < length) {
						temp.range(31 + j * 32, j * 32) = convert_to_bv(num[i * 8 + j]);
					}
					else {
						temp.range(31 + j * 32, j * 32) = sc_bv<32>(0);
					}
				}
				data[i] = temp;
			}
			dram_port->burst_write(178, 200, data);

			std::cout << "length for x4 " << datalen << std::endl;
			std::cout << "data" << num[0] << std::endl;
			std::cout << "data" << convert_to_float(data[0].range(31, 0)) << std::endl;
			delete[] data;

		}*/
		{
			//load img and write to dram;
			std::cout << "reading img" << std::endl;
			int length = 1024;
			int datalen = 0;
			float num[1024];
			ifstream file("../img.txt");
			while (!file.eof()) {
				file >> num[datalen++];
			}
			file.close();
			sc_bv<PORT_WIDTH>* data = new sc_bv<PORT_WIDTH>[128];//(sc_bv<PORT_WIDTH>*)malloc(sizeof(sc_bv<PORT_WIDTH>) * 105);
			for (int i = 0; i <int((length - 1) / 8 + 1); i++) {
				sc_bv<PORT_WIDTH> temp;
				for (int j = 0; j < 8; j++) {
					if (j + i * 8 < length) {
						temp.range(31 + j * 32, j * 32) = convert_to_bv(num[i * 8 + j]);
					}
					else {
						temp.range(31 + j * 32, j * 32) = sc_bv<32>(0);
					}
				}
				data[i] = temp;
			}
			dram_port->burst_write(179, 128, data);

			std::cout << "length for img " << datalen << std::endl;
			std::cout << "data" << num[0] << std::endl;
			std::cout << "data" << convert_to_float(data[0].range(31, 0)) << std::endl;
			delete[] data;
		}
		{
			//load img and write to dram;
			std::cout << "reading conv1" << std::endl;
			int length = 150;
			int datalen = 0;
			float num[150];
			ifstream file("../conv1.txt");
			while (!file.eof()) {
				file >> num[datalen++];
			}
			file.close();
			sc_bv<PORT_WIDTH>* data = new sc_bv<PORT_WIDTH>[19];//(sc_bv<PORT_WIDTH>*)malloc(sizeof(sc_bv<PORT_WIDTH>) * 105);
			for (int i = 0; i <int((length - 1) / 8 + 1); i++) {
				sc_bv<PORT_WIDTH> temp;
				for (int j = 0; j < 8; j++) {
					if (j + i * 8 < length) {
						temp.range(31 + j * 32, j * 32) = convert_to_bv(num[i * 8 + j]);
					}
					else {
						temp.range(31 + j * 32, j * 32) = sc_bv<32>(0);
					}
				}
				data[i] = temp;
			}
			dram_port->burst_write(1900, 19, data);

			delete[] data;
		}
		{
			//load conv2 and write to dram;
			std::cout << "reading conv2" << std::endl;
			int length = 2400;
			int datalen = 0;
			float num[2400];
			ifstream file("../conv2.txt");
			while (!file.eof()) {
				file >> num[datalen++];
			}
			file.close();
			sc_bv<PORT_WIDTH>* data = new sc_bv<PORT_WIDTH>[300];//(sc_bv<PORT_WIDTH>*)malloc(sizeof(sc_bv<PORT_WIDTH>) * 105);
			for (int i = 0; i <int((length - 1) / 8 + 1); i++) {
				sc_bv<PORT_WIDTH> temp;
				for (int j = 0; j < 8; j++) {
					if (j + i * 8 < length) {
						temp.range(31 + j * 32, j * 32) = convert_to_bv(num[i * 8 + j]);
					}
					else {
						temp.range(31 + j * 32, j * 32) = sc_bv<32>(0);
					}
				}
				data[i] = temp;
			}
			dram_port->burst_write(2000, 300, data);
			delete[] data;
		}
		{
			//load fc1 and write to dram
			std::cout << "reading fc1 " << std::endl;
			int length = 12000;
			int datalen = 0;
			float num[12000];
			ifstream file("../fc1.txt");
			sc_bv<PORT_WIDTH>* data = new sc_bv<PORT_WIDTH>[1500];
			for (int k = 0; k < 4; k++) {
				for (int i = 0; i < 12000; i++) {
					file >> num[i];
				}
				for (int i = 0; i <int((length - 1) / 8 + 1); i++) {
					sc_bv<PORT_WIDTH> temp;
					for (int j = 0; j < 8; j++) {
						if (j + i * 8 < length) {
							temp.range(31 + j * 32, j * 32) = convert_to_bv(num[i * 8 + j]);
						}
						else {
							temp.range(31 + j * 32, j * 32) = sc_bv<32>(0);
						}
					}
					data[i] = temp;
				}
				dram_port->burst_write(2400+1500*k, 1500, data);
			}
			file.close();
			delete[] data;
			/*std::cout << "reading fc1" << std::endl;
			int length = 48000;
			int datalen = 0;
			float num[48000];
			ifstream file("../fc1.txt");
			while (!file.eof()) {
				file >> num[datalen++];
			}
			sc_bv<PORT_WIDTH>* data = new sc_bv<PORT_WIDTH>[6000];//(sc_bv<PORT_WIDTH>*)malloc(sizeof(sc_bv<PORT_WIDTH>) * 105);
			for (int i = 0; i <int((length - 1) / 8 + 1); i++) {
				sc_bv<PORT_WIDTH> temp;
				for (int j = 0; j < 8; j++) {
					if (j + i * 8 < length) {
						temp.range(31 + j * 32, j * 32) = convert_to_bv(num[i * 8 + j]);
					}
					else {
						temp.range(31 + j * 32, j * 32) = sc_bv<32>(0);
					}
				}
				data[i] = temp;
			}
			dram_port->burst_write(2400, 6000, data);
			file.close();
			delete[] data;*/
		}
		{	
			//load fc2 and write to dram
			std::cout << "reading fc2" << std::endl;
			int length = 10080;
			int datalen = 0;
			float num[10080];
			ifstream file("../fc2.txt");
			while (!file.eof()) {
				file >> num[datalen++];
			}
			sc_bv<PORT_WIDTH>* data = new sc_bv<PORT_WIDTH>[1260];//(sc_bv<PORT_WIDTH>*)malloc(sizeof(sc_bv<PORT_WIDTH>) * 105);
			for (int i = 0; i <int((length - 1) / 8 + 1); i++) {
				sc_bv<PORT_WIDTH> temp;
				for (int j = 0; j < 8; j++) {
					if (j + i * 8 < length) {
						temp.range(31 + j * 32, j * 32) = convert_to_bv(num[i * 8 + j]);
					}
					else {
						temp.range(31 + j * 32, j * 32) = sc_bv<32>(0);
					}
				}
				data[i] = temp;
			}
			dram_port->burst_write(8500, 1260, data);
			file.close();
			delete[] data;
		}
		{
			//load fc3 and write to dram
			std::cout << "reading fc3" << std::endl;
			int length = 840;
			int datalen = 0;
			float num[840];
			ifstream file("../fc3.txt");
			while (!file.eof()) {
				file >> num[datalen++];
			}
			sc_bv<PORT_WIDTH>* data = new sc_bv<PORT_WIDTH>[105];//(sc_bv<PORT_WIDTH>*)malloc(sizeof(sc_bv<PORT_WIDTH>) * 105);
			for (int i = 0; i <int((length - 1) / 8 + 1); i++) {
				sc_bv<PORT_WIDTH> temp;
				for (int j = 0; j < 8; j++) {
					if (j + i * 8 < length) {
						temp.range(31 + j * 32, j * 32) = convert_to_bv(num[i * 8 + j]);
					}
					else {
						temp.range(31 + j * 32, j * 32) = sc_bv<32>(0);
					}
				}
				data[i] = temp;
			}
			dram_port->burst_write(9800, 105, data);
			file.close();
			delete[] data;
		}
		/*
		sc_bv<PORT_WIDTH> prim0 = convertPrim2Code(Prim_Load(1001, 34, 1));
		dram_port->write(0, prim0);

		sc_bv<PORT_WIDTH> prim1 = convertPrim2Code(Prim_Load(0, 200, 178));
		dram_port->write(1, prim1);

		sc_bv<PORT_WIDTH> prim_pool = convertPrim2Code(Prim_Pooling(0, 10, 10, 16, 16, 2, 0, 1, 0, 0));
		dram_port->write(2, prim_pool);
		
		for (int i = 0; i < 7; i++) {
			sc_bv<PORT_WIDTH> prim_3 = convertPrim2Code(Prim_Load(51, 800, 2400+800*i));
			dram_port->write(3+3*i, prim_3);
			sc_bv<PORT_WIDTH> prim_4 = convertPrim2Code(Prim_Matrix_Mul(51, 16, 400, 0, 1, 0, 900+2*i));
			dram_port->write(4+3*i, prim_4);
			sc_bv<PORT_WIDTH> prim_5 = convertPrim2Code(Prim_ReLU(900+2*i, 16, 0, 900+2*i));
			dram_port->write(5+3*i, prim_5);
		}

		sc_bv<PORT_WIDTH> prim2 = convertPrim2Code(Prim_Load(51, 400, 8000));
		dram_port->write(24, prim2);
		sc_bv<PORT_WIDTH> prim3 = convertPrim2Code(Prim_Matrix_Mul(51, 8, 400, 0, 1, 0, 914));
		dram_port->write(25, prim3);
		sc_bv<PORT_WIDTH> prim4 = convertPrim2Code(Prim_ReLU(914, 8, 0, 914));
		dram_port->write(26, prim4);


		sc_bv<PORT_WIDTH> prim5 = convertPrim2Code(Prim_Load(16, 600, 8500));
		dram_port->write(27, prim5);
		sc_bv<PORT_WIDTH> prim6 = convertPrim2Code(Prim_Matrix_Mul(16, 40, 120, 900, 1, 0, 700));
		dram_port->write(28, prim6);
		sc_bv<PORT_WIDTH> prim7 = convertPrim2Code(Prim_ReLU(700, 40, 0, 700));
		dram_port->write(29, prim7);
		sc_bv<PORT_WIDTH> prim8 = convertPrim2Code(Prim_Load(16, 660, 9100));
		dram_port->write(30, prim8);
		sc_bv<PORT_WIDTH> prim9 = convertPrim2Code(Prim_Matrix_Mul(16, 44, 120, 900, 1, 0, 705));
		dram_port->write(31, prim9);
		sc_bv<PORT_WIDTH> prim10 = convertPrim2Code(Prim_ReLU(705, 44, 0, 705));
		dram_port->write(32, prim10);


		sc_bv<PORT_WIDTH> prim11 = convertPrim2Code(Prim_Load(16, 105, 9800));
		dram_port->write(33, prim11);
		sc_bv<PORT_WIDTH> prim12 = convertPrim2Code(Prim_Matrix_Mul(16, 10, 84, 700, 1, 680));
		dram_port->write(34, prim12);
		*/
		
		sc_bv<PORT_WIDTH> prim0 = convertPrim2Code(Prim_Load(1001, 40, 1));
		dram_port->write(0, prim0);
		sc_bv<PORT_WIDTH> prim1 = convertPrim2Code(Prim_Load(0, 128, 179));
		dram_port->write(1, prim1);
		sc_bv<PORT_WIDTH> prim_conv11 = convertPrim2Code(Prim_Load(129, 19, 1900));
		dram_port->write(2, prim_conv11);
		sc_bv<PORT_WIDTH> prim_conv12 = convertPrim2Code(Prim_Conv2d(0, 32, 32, 129, 1, 6, 5, 0, 0, 0, 0, 0));
		dram_port->write(3, prim_conv12);
		sc_bv<PORT_WIDTH> prim_relu1 = convertPrim2Code(Prim_ReLU(0, 4704, 0, 0));
		dram_port->write(4, prim_relu1);
		sc_bv<PORT_WIDTH> prim_maxpool1 = convertPrim2Code(Prim_Pooling(0, 28, 28, 6, 6, 2, 0, 1, 0, 0));
		dram_port->write(5, prim_maxpool1);
		sc_bv<PORT_WIDTH> prim_load_conv2 = convertPrim2Code(Prim_Load(129, 300, 2000));
		dram_port->write(6, prim_load_conv2);
		sc_bv<PORT_WIDTH> prim_conv2 = convertPrim2Code(Prim_Conv2d(0, 14, 14, 129, 6, 16, 5, 0, 0, 0, 0, 0));
		dram_port->write(7, prim_conv2);
		sc_bv<PORT_WIDTH> prim_relu2 = convertPrim2Code(Prim_ReLU(0, 1600, 0, 0));
		dram_port->write(8, prim_relu2);
		sc_bv<PORT_WIDTH> prim_maxpool2 = convertPrim2Code(Prim_Pooling(0, 10, 10, 16, 16, 2, 0, 1, 0, 0));
		dram_port->write(9, prim_maxpool2);
		for (int i = 0; i < 7; i++) {
			sc_bv<PORT_WIDTH> prim2 = convertPrim2Code(Prim_Load(51, 800, 2400 + 800 * i));
			dram_port->write(9 + 3 * i, prim2);
			sc_bv<PORT_WIDTH> prim3 = convertPrim2Code(Prim_Matrix_Mul(51, 16, 400, 0, 1, 0, 900 + 2*i));
			dram_port->write(10 + 3 * i, prim3);
			sc_bv<PORT_WIDTH> prim4 = convertPrim2Code(Prim_ReLU(900 + 2*i, 16, 0, 900 + 2*i));
			dram_port->write(11 + 3 * i, prim4);
		}
		sc_bv<PORT_WIDTH> prim2 = convertPrim2Code(Prim_Load(51, 400, 8000));
		dram_port->write(30, prim2);
		sc_bv<PORT_WIDTH> prim3 = convertPrim2Code(Prim_Matrix_Mul(51, 8, 400, 0, 1, 0, 914));
		dram_port->write(31, prim3);
		sc_bv<PORT_WIDTH> prim4 = convertPrim2Code(Prim_ReLU(914, 8, 0, 914));
		dram_port->write(32, prim4);


		sc_bv<PORT_WIDTH> prim5 = convertPrim2Code(Prim_Load(16, 600, 8500));
		dram_port->write(33, prim5);
		sc_bv<PORT_WIDTH> prim6 = convertPrim2Code(Prim_Matrix_Mul(16, 40, 120, 900, 1, 0, 700));
		dram_port->write(34, prim6);
		sc_bv<PORT_WIDTH> prim7 = convertPrim2Code(Prim_ReLU(700, 40, 0, 700));
		dram_port->write(35, prim7);
		sc_bv<PORT_WIDTH> prim8 = convertPrim2Code(Prim_Load(16, 660, 9100));
		dram_port->write(36, prim8);
		sc_bv<PORT_WIDTH> prim9 = convertPrim2Code(Prim_Matrix_Mul(16, 44, 120, 900, 1, 0, 705));
		dram_port->write(37, prim9);
		sc_bv<PORT_WIDTH> prim10 = convertPrim2Code(Prim_ReLU(705, 44, 0, 705));
		dram_port->write(38, prim10);
		sc_bv<PORT_WIDTH> prim11 = convertPrim2Code(Prim_Load(16, 105, 9800));
		dram_port->write(39, prim11);
		sc_bv<PORT_WIDTH> prim12 = convertPrim2Code(Prim_Matrix_Mul(16, 10, 84, 700, 1, 680));
		dram_port->write(40, prim12);


		/*
		sc_bv<PORT_WIDTH> prim0 = convertPrim2Code(Prim_Load(1001, 54, 1));
		dram_port->write(0, prim0);
		sc_bv<PORT_WIDTH> prim1 = convertPrim2Code(Prim_Load(0, 50, 128));
		dram_port->write(1, prim1);
		for (int i = 0; i < 14; i++) {
			sc_bv<PORT_WIDTH> prim2 = convertPrim2Code(Prim_Load(51, 400, 2400 + 400 * i));
			dram_port->write(2 + 3 * i, prim2);
			sc_bv<PORT_WIDTH> prim3 = convertPrim2Code(Prim_Matrix_Mul(51, 8, 400, 0, 1, 0, 900 +  i));
			dram_port->write(3 + 3 * i, prim3);
			sc_bv<PORT_WIDTH> prim4 = convertPrim2Code(Prim_ReLU(900 + i, 8, 0, 900 + i));
			dram_port->write(4 + 3 * i, prim4);
		}
		sc_bv<PORT_WIDTH> prim2 = convertPrim2Code(Prim_Load(51, 400, 8000));
		dram_port->write(44, prim2);
		sc_bv<PORT_WIDTH> prim3 = convertPrim2Code(Prim_Matrix_Mul(51, 8, 400, 0, 1, 0, 914));
		dram_port->write(45, prim3);
		sc_bv<PORT_WIDTH> prim4 = convertPrim2Code(Prim_ReLU(914, 8, 0, 914));
		dram_port->write(46, prim4);


		sc_bv<PORT_WIDTH> prim5 = convertPrim2Code(Prim_Load(16, 600, 8500));
		dram_port->write(47, prim5);
		sc_bv<PORT_WIDTH> prim6 = convertPrim2Code(Prim_Matrix_Mul(16, 40, 120, 900, 1, 0, 700));
		dram_port->write(48, prim6);
		sc_bv<PORT_WIDTH> prim7 = convertPrim2Code(Prim_ReLU(700, 40, 0, 700));
		dram_port->write(49, prim7);
		sc_bv<PORT_WIDTH> prim8 = convertPrim2Code(Prim_Load(16, 660, 9100));
		dram_port->write(50, prim8);
		sc_bv<PORT_WIDTH> prim9 = convertPrim2Code(Prim_Matrix_Mul(16, 44, 120, 900, 1, 0, 705));
		dram_port->write(51, prim9);
		sc_bv<PORT_WIDTH> prim10 = convertPrim2Code(Prim_ReLU(705, 44, 0, 705));
		dram_port->write(52, prim10);
		sc_bv<PORT_WIDTH> prim11 = convertPrim2Code(Prim_Load(16, 105, 9800));
		dram_port->write(53, prim11);
		sc_bv<PORT_WIDTH> prim12 = convertPrim2Code(Prim_Matrix_Mul(16, 10, 84, 700, 1, 680));
		dram_port->write(54, prim12);
		*/
		/*
		//// Ԥ��д������
		sc_bv<PORT_WIDTH> old_data = 0x99999999;
		for (int i = 0; i < 8; i++) {
			old_data.range(i * 32 + 31, i * 32) = convert_to_bv(num[i]);
		}
		std::cout << "old: " << old_data << std::endl;
		dram_port->write(20, old_data);

		dram_port->read(5, old_data);
		std::cout << "before: " << old_data << std::endl;

		//// ָ��1����dram��ȡ����
		sc_bv<PORT_WIDTH> prim0 = convertPrim2Code(Prim_Load(1001, 2, 1));
		dram_port->write(0, prim0);
		sc_bv<PORT_WIDTH> prim1 = convertPrim2Code(Prim_Load(0, 1, 20));
		dram_port->write(1, prim1);
		//// ָ��2��������д��dram
		//sc_bv<PORT_WIDTH> prim2 = convertPrim2Code(Prim_Store(0, 1, 5));
		//dram_port->write(2, prim2);
		//sc_bv<PORT_WIDTH> prim2 = convertPrim2Code(Prim_ReLU(0, 1, 15, 0));
		//dram_port->write(1, prim2);


		sc_bv<PORT_WIDTH> prim3 = convertPrim2Code(Prim_Store(0, 1, 5));
		dram_port->write(2, prim3);

		//// ָ��3���ظ���������ָ��
		//sc_bv<PORT_WIDTH> prim4 = convertPrim2Code(Prim_Jump(-2));
		//dram_port->write(2, prim4);
		*/
		/*
		sc_bv<PORT_WIDTH> data_a = convert_to_bv(15);
		data_a.range(63, 32) = convert_to_bv(11);
		data_a.range(95, 64) = convert_to_bv(10);
		data_a.range(127, 96) = convert_to_bv(9);
		sc_bv<PORT_WIDTH> data_b = convert_to_bv(0);
		data_b.range(63, 32) = convert_to_bv(1);
		data_b.range(95, 64) = convert_to_bv(1);
		data_b.range(127, 96) = convert_to_bv(0);
		dram_port->write(20, data_a);
		dram_port->write(30, data_b);
		//��ָ��
		sc_bv<PORT_WIDTH> prim1 = convertPrim2Code(Prim_Load(1001, 3, 1));
		dram_port->write(0, prim1);
		sc_bv<PORT_WIDTH> prim2 = convertPrim2Code(Prim_Load(0, 1, 20));//a 0
		dram_port->write(1, prim2);
		sc_bv<PORT_WIDTH> prim3 = convertPrim2Code(Prim_Load(2, 1, 30));//b 0
		dram_port->write(2, prim3);
		sc_bv<PORT_WIDTH> prim4 = convertPrim2Code(Prim_Matrix_Mul(0, 3, 5, 2, 6, 0, 20));//b 0
		dram_port->write(3, prim4);
		*/
		idle.write(true);
		rst.write(false);
		wait(2, SC_NS);
		rst.write(true);
		wait(10, SC_NS);
		rst.write(false);
		wait(1, SC_NS);
		idle.write(false);


		wait(20000000000000, SC_NS);

		 //�鿴д�������Ƿ���ȷ
		//sc_bv<PORT_WIDTH> old_data;
		/*
		for (int i = 0; i < 8; i++) {
			std::cout << convert_to_float(old_data.range(31 + 32 * i, 32 * i)) << std::endl;
		}
		*/

		simulater_time = sc_time_stamp().to_double();
		cout << "Simulation finished in time  " << simulater_time << endl;
		sc_stop();

	}

public:
	double simulater_time;
	sc_out<bool> rst;
	sc_out<bool> idle;
	sc_port<burst_ram_if<sc_bv<PORT_WIDTH> > > dram_port;
	sc_port<mem_write_if<sc_bv<PORT_WIDTH> > > prim_buffer_port;
};·