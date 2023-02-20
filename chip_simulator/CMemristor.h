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



static const int maxConductanceBits = 4;
static const int maxCellBits = 8;   // 8 or 4
static const int conductanceFactor = pow(2, 4);
static const int defaultDeviceConductance = 1;
static const double minDeviceConductance = 1;
static const double maxDeviceConductance = pow(2, 4);

static const int defaultArraySize = 12;
static const int minArraySize = 1;
static const int maxArraySize = 512;

static const int maxInputVBits = 8;
static const int minInputV = 0;
static const int maxInputV = pow(2, maxInputVBits) - 1;

static const int maxOutputVBits = 32; // current out bits
static const int maxOutputV = pow(2, maxOutputVBits - 1) - 1;
static const int minOutputV = -pow(2, maxOutputVBits - 1);

static const double readVoltage = 1;

/* non-ideal properties */

static int seednum = time(0);

static const bool deviceVar = false;
static const double deviceVarSigma = 0.05;

static const bool circuitNoise = false;
static const double circuitNoiseSigma = 0.005;

static const bool ADCQuantization = false;
static const int ADCQuantBits = 12;
static const int maxADCOut = pow(2, ADCQuantBits) - 1;








/*
	Cell class,
	1 Cell contains 4 device, G+ and G-, g+ and g-(double), currently support method: CellConductanceProgram
*/
class Cell
{
public:
	// int x, y;
	double cellConductance;
	double cellConductancePlusMSB, cellConductanceMinusMSB, cellConductancePlusLSB, cellConductanceMinusLSB;

	void CellConductanceCal(void);
	Cell(double cellConductancePlusMSB, double cellConductanceMinusMSB, double cellConductancePlusLSB, double cellConductanceMinusLSB);    // Initialize device manually, default: defaultDeviceConductance
	void CellConductanceProgram(int targetCellConductance);     // Program device conductances according to the target conductance, default: 0

};

void Cell::CellConductanceCal(void)
{
	cellConductance = conductanceFactor * (cellConductancePlusMSB - cellConductanceMinusLSB)
		+ (cellConductancePlusLSB - cellConductanceMinusLSB);
}

Cell::Cell(double cellConductancePlusMSB = defaultDeviceConductance, double cellConductanceMinusMSB = defaultDeviceConductance,
	double cellConductancePlusLSB = defaultDeviceConductance, double cellConductanceMinusLSB = defaultDeviceConductance)
{
	// TODO: check
	this->cellConductancePlusMSB = cellConductancePlusMSB;
	this->cellConductanceMinusMSB = cellConductanceMinusMSB;
	this->cellConductancePlusLSB = cellConductancePlusLSB;
	this->cellConductanceMinusLSB = cellConductanceMinusLSB;
	CellConductanceCal();
}

void Cell::CellConductanceProgram(int targetCellConductance = 0)
{
	// TODO: check targetCellConductance in (max - min) range
	// Current: Naive programming method
	if (targetCellConductance >= 0)
	{
		cellConductanceMinusMSB = defaultDeviceConductance;
		cellConductanceMinusLSB = defaultDeviceConductance;
		cellConductancePlusMSB = targetCellConductance / conductanceFactor + defaultDeviceConductance;
		cellConductancePlusLSB = targetCellConductance % conductanceFactor + defaultDeviceConductance;
	}
	else
	{
		cellConductancePlusMSB = defaultDeviceConductance;
		cellConductancePlusLSB = defaultDeviceConductance;
		cellConductanceMinusMSB = defaultDeviceConductance - targetCellConductance / conductanceFactor;
		cellConductanceMinusLSB = defaultDeviceConductance - targetCellConductance % conductanceFactor;
	}

	if (deviceVar == true)
	{
		std::default_random_engine gen(seednum++);
		std::normal_distribution<double> dis(0, deviceVarSigma);
		cellConductancePlusMSB *= 1 + dis(gen);
		cellConductancePlusLSB *= 1 + dis(gen);
		cellConductanceMinusMSB *= 1 + dis(gen);
		cellConductanceMinusLSB *= 1 + dis(gen);
	}

	CellConductanceCal();
}


/*
	Array class
	currently support method: ArrayProgram, ArrayVMM
	arrayColSize: check
*/
class Array
{
public:
	int arrayRowSize, arrayColSize;
	std::vector<std::vector<Cell>> cellInArray;   // 2-dim array, store cells

	Array(int arrayRowSize, int arrayColSize);  // Initialize array size and cellInArray, default: defaultDeviceConductance
	void ArrayProgram(std::vector<std::vector<int>> targetArrayConductance);   // Program the array according to 2-dim input vector
	std::vector<std::vector<double>> ArrayVMM(std::vector<double> inputVector);   // VMM operation

};

Array::Array(int arrayRowSize = defaultArraySize, int arrayColSize = defaultArraySize)
{
	// TODO: check
	this->arrayRowSize = arrayRowSize;
	this->arrayColSize = arrayColSize;

	std::vector<Cell> tempvec;   // temp vector for each row;

	for (int cntRow = 0; cntRow < arrayRowSize; cntRow++)
	{
		for (int cntCol = 0; cntCol < arrayColSize; cntCol++)
		{
			Cell cell;
			tempvec.push_back(cell);
		}
		cellInArray.push_back(tempvec);
		tempvec.clear();
	}
}


void Array::ArrayProgram(std::vector<std::vector<int>> targetArrayConductance)
{
	// TODO: check size
	for (int cntRow = 0; cntRow < arrayRowSize; cntRow++)
	{
		for (int cntCol = 0; cntCol < arrayColSize; cntCol++)
		{
			cellInArray[cntRow][cntCol].CellConductanceProgram(targetArrayConductance[cntRow][cntCol]);
		}
	}
}



std::vector<std::vector<double>> Array::ArrayVMM(std::vector<double> inputVector)   // inputVector:(1, m)*(m, n) -> outputVector:(1, n*4), order: G+,G-,g+,g-
{
	// TODO: check size
	std::vector<std::vector<double>> outputVector;

	for (int cntCol = 0; cntCol < arrayColSize; cntCol++)
	{
		std::vector<double> tempvec(4, 0);

		for (int cntRow = 0; cntRow < arrayRowSize; cntRow++)
		{
			tempvec[0] += (cellInArray[cntRow][cntCol].cellConductancePlusMSB * inputVector[cntRow]);
			tempvec[1] += (cellInArray[cntRow][cntCol].cellConductanceMinusMSB * inputVector[cntRow]);
			tempvec[2] += (cellInArray[cntRow][cntCol].cellConductancePlusLSB * inputVector[cntRow]);
			tempvec[3] += (cellInArray[cntRow][cntCol].cellConductanceMinusLSB * inputVector[cntRow]);
		}

		if (circuitNoise == true)
		{
			std::default_random_engine gen(seednum++);
			std::normal_distribution<double> dis(0, circuitNoiseSigma);
			for (int cnt = 0; cnt < 4; cnt++)
			{
				tempvec[cnt] *= 1 + dis(gen);
			}

		}
		outputVector.push_back(tempvec);
		tempvec.clear();

	}

	return outputVector;

}



/*
	Array Peripheral class
	currently support method: InputDecoder, ADC, AdderAndShifter
	quantization is not considered
*/
class ArrayPeripheral
{
public:
	std::vector<std::vector<double>> InputDecoder(std::vector<int> inputV);
	std::vector<std::vector<int>> ADC(std::vector<std::vector<double>> outputISingle);
	std::vector<int> Adder(std::vector<std::vector<int>> outputVSingle);
	std::vector<int> AdderAndShifter(std::vector<std::vector<int>> outputVSeries);

};


std::vector<std::vector<double>> ArrayPeripheral::InputDecoder(std::vector<int> inputV)    // (m)-> (m, 8bits)
{
	// check inputV range
	// int currentMax =  *max_element(inputV.begin(),inputV.end());
	// int currentMin =  *min_element(inputV.begin(),inputV.end());
	// try
	// {
	//     if((currentMax>maxInputV) || (currentMin<minInputV))
	//         throw "Illegal input!";
	// }
	// catch(const char* msg)
	// {
	//  cout << msg << endl;
	//  exit(-1);  // exit
	// }

	// convert inputV -> inputVSeries(from LSB to MSB, with read voltage=1V)
	std::vector<std::vector<double>> inputVSeries;
	std::vector<double> tempvec;   // temp vector for each inputV;
	for (int cnt = 0; cnt < inputV.size(); cnt++)
	{
		int tempnum = inputV[cnt];
		for (int cnt_bits = 0; cnt_bits < maxInputVBits; cnt_bits++)
		{
			if (tempnum > 0)
			{
				tempvec.push_back(tempnum % 2 * readVoltage);
				tempnum = tempnum / 2;
			}
			else
				tempvec.push_back(0);
		}

		inputVSeries.push_back(tempvec);
		tempvec.clear();
	}

	return inputVSeries;

}

std::vector<std::vector<int>> ArrayPeripheral::ADC(std::vector<std::vector<double>> outputISingle)
{
	std::vector<std::vector<int>> outputVSingle;
	for (int cntRow = 0; cntRow < outputISingle.size(); cntRow++)
	{
		std::vector<int> tempvec;
		for (int cntCol = 0; cntCol < outputISingle[0].size(); cntCol++)
		{
			int tempnum = int(outputISingle[cntRow][cntCol] + 0.5);
			if (ADCQuantization == true)
			{
				if (tempnum > maxADCOut)
					tempnum = maxADCOut;
			}
			tempvec.push_back(tempnum);
		}
		outputVSingle.push_back(tempvec);
		tempvec.clear();
	}

	return outputVSingle;
}



std::vector<int> ArrayPeripheral::Adder(std::vector<std::vector<int>> outputVSingle)
{
	// TODO: check size(n, 4)

	std::vector<int> outputVCell;
	for (int cntRow = 0; cntRow < outputVSingle.size(); cntRow++)
	{
		int tempnum = 0;
		tempnum += outputVSingle[cntRow][0] * conductanceFactor;
		tempnum -= outputVSingle[cntRow][1] * conductanceFactor;
		tempnum += outputVSingle[cntRow][2];
		tempnum -= outputVSingle[cntRow][3];

		outputVCell.push_back(tempnum);
	}

	return outputVCell;

}

std::vector<int> ArrayPeripheral::AdderAndShifter(std::vector<std::vector<int>> outputVSeries)   // (8bits, n)-> (n)
{
	std::vector<int> outputV;
	for (int cntCol = 0; cntCol < outputVSeries[0].size(); cntCol++)
	{
		int tempnum = 0;
		for (int cntRow = 0; cntRow < outputVSeries.size(); cntRow++)
		{
			tempnum += outputVSeries[cntRow][cntCol] * pow(2, cntRow);
		}

		// check output range
		if (tempnum > maxOutputV)
			tempnum = maxOutputV;
		if (tempnum < minOutputV)
			tempnum = minOutputV;

		outputV.push_back(tempnum);
	}

	return outputV;

}


class ArrayIO
{
public:
	Array array1;
	ArrayPeripheral ap1;
	double timeCost;    // us

	ArrayIO(int arrayRowSize, int arrayColSize);
	void Program(std::vector<std::vector<int>> targetArrayConductance);
	std::vector<int> Calculate(std::vector<int> inputV);
};


ArrayIO::ArrayIO(int arrayRowSize = defaultArraySize, int arrayColSize = defaultArraySize)
{
	Array array(arrayRowSize, arrayColSize);
	this->array1 = array;
	ArrayPeripheral ap;
	this->ap1 = ap;
}

void ArrayIO::Program(std::vector<std::vector<int>> targetArrayConductance)
{
	array1.ArrayProgram(targetArrayConductance);
}

std::vector<int> ArrayIO::Calculate(std::vector<int> inputV)
{
	std::vector<std::vector<double>> inputVSeries = ap1.InputDecoder(inputV);

	std::vector<std::vector<int>> outputVSeries;

	// here for multi-time steps
	for (int cntCol = 0; cntCol < inputVSeries[0].size(); cntCol++)   // cntCol counts times()
	{
		std::vector<double> tempvec;
		for (int cntRow = 0; cntRow < inputVSeries.size(); cntRow++)
		{
			tempvec.push_back(inputVSeries[cntRow][cntCol]);
		}

		std::vector<std::vector<double>> outputISingle = array1.ArrayVMM(tempvec);
		std::vector<std::vector<int>> outputVSingle = ap1.ADC(outputISingle);
		std::vector<int> outputVCell = ap1.Adder(outputVSingle);
		outputVSeries.push_back(outputVCell);
	}


	std::vector<int> outputV = ap1.AdderAndShifter(outputVSeries);

	this->timeCost = 0.1 * maxInputVBits * (array1.arrayRowSize / 8 + 1);   // us

	return outputV;

}



enum memristor_exec_status
{
	IDLE_mem = 0, EXEC_mem
};



template <class T>
class CMemristor : public sc_module{
public:
	SC_HAS_PROCESS(CMemristor);
	CMemristor(const sc_module_name &name):
		sc_module(name) {

		// Test example
		
		SC_THREAD(memristor_state_trans);
		sensitive << rst.pos() << prim_finish_reg << memristor_prim_start.pos();
		dont_initialize();

		SC_THREAD(memristor_prim_execution);
		sensitive << memristor_status << rst.pos();
		dont_initialize();


	}

	

	/*
	State machine
	IDLE_mem memristor not busy
	EXEC_mem memristor busy 
	
	*/
	void memristor_state_trans() {
		while (true) {
			if (this->rst.read()) {
				this->memristor_status.write(IDLE_mem);
				this->prim_reg.write(Prim_Memristor());
				this->memristor_busy.write(false);
			}
			else if (this->prim_finish_reg.read()) {
				this->memristor_status.write(IDLE_mem);
				this->memristor_busy.write(false);
			}
			else if (memristor_prim_start.read()) {
				Prim_Memristor Prim_temp = prim_in.read();
				prim_reg.write(Prim_temp);
				
				this->memristor_status.write(EXEC_mem);
				this->memristor_busy.write(true);
				
			}
			wait();
		}

	}


	void memristor_prim_execution() {


		while (true) {
			if ((this->memristor_status.read() == IDLE_mem) || this->rst.read()) {
				this->prim_finish_reg.write(false);
			}
			else if (this->memristor_status == EXEC_mem) {
				this->prim_finish_reg.write(false);
				Prim_Memristor prim_temp = prim_reg.read();
				std::vector<int> data_in;
				this->load(prim_temp.Scrachpad_Addr, prim_temp.Data_Length, data_in);
				m = 8;
				n = 4;
				std::vector<std::vector<int>> conductance(m, std::vector<int>(n, 30));
				
				ArrayIO a(m, n);
				a.Program(conductance);
				std::vector<int> out = a.Calculate(data_in);
				
				for (int cntRow = 0; cntRow < m; cntRow++)
				{
					for (int cntCol = 0; cntCol < n; cntCol++)
						std::cout << conductance[cntRow][cntCol] << " ";
					std::cout << std::endl;
				}
				std::cout << std::endl << "input:" << std::endl;
				std::cout << data_in.size() << std::endl;
				std::cout << out.size() << std::endl;
				for (size_t cnt = 0; cnt < data_in.size(); cnt++)
					std::cout << data_in[cnt] << " ";
				std::cout << std::endl;
				std::cout << std::endl << "output:" << std::endl;
				for (size_t cnt = 0; cnt < out.size(); cnt++)
					std::cout << out[cnt] << " ";
				std::cout << std::endl;
				std::cout << "Time Cost: " << a.timeCost << " us" << std::endl;
				
				wait(6 + prim_temp.Data_Length, SC_NS);
				this->prim_finish_reg.write(true);
			}
			wait();
		}

	}

public:
	
	sc_in<Prim_Memristor> prim_in;
	sc_in<bool> rst;
	sc_in<bool> memristor_prim_start;
	sc_out<bool> memristor_busy;
	sc_signal<Prim_Memristor> prim_reg;
	sc_signal<bool> prim_finish_reg;

	sc_signal<memristor_exec_status> memristor_status;
	sc_port<ram_if<T> > scratchpad_port;


private:
	int m;
	int n;

	void load(unsigned scratchpad_addr, unsigned data_length, std::vector<int> &data_tmp) {
		
		T* load_data = new T[data_length];
		for (unsigned i = 0; i < data_length; i++) {
			scratchpad_port->read(scratchpad_addr + i, load_data[i]);
			//std::cout << load_data[i] << std::endl;
		}
		
		for (unsigned i = 0; i < data_length; i++) {
			for (unsigned j = 0; j < PORT_WIDTH / 32; j++) {
				data_tmp.push_back(load_data[i].range(31 + j * 32, 0 + j * 32).to_int());
				//std::cout << load_data[i].range(31 + j * 32, 0 + j * 32).to_int() << std::endl;
			}
		}
		//return data_tmp;



	}


};