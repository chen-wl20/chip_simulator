#pragma once
#include "systemc.h"
#include "config.h"

class PrimPacket
{
public:
	PrimPacket();
	~PrimPacket();
	PrimPacket& operator=(const PrimPacket& rhs);
	bool operator==(const PrimPacket& rhs) const;
	friend ostream& operator<<(ostream& file, const PrimPacket& prim);
	friend void sc_trace(sc_trace_file*& tf, const PrimPacket& prim, std::string nm);
	friend sc_bv<PORT_WIDTH> convertPrim2Code(const PrimPacket& p);

	

public:
	std::string type_name;

protected:
	sc_bv<PORT_WIDTH> prim_id_code;
};



class Prim_Mem : public PrimPacket
{
public:
	Prim_Mem(unsigned scratchpad_addr = 0, unsigned length = 0, unsigned dram_addr = 0)
		: Scrachpad_Addr(scratchpad_addr), Data_Length(length), Dram_Addr(dram_addr)
	{
		this->prim_id_code.range(19, 4) = sc_bv<16>(scratchpad_addr);
		this->prim_id_code.range(35, 20) = sc_bv<16>(length);
		this->prim_id_code.range(115, 84) = sc_bv<32>(dram_addr);
	}

public:
	unsigned Scrachpad_Addr;
	unsigned Data_Length;
	unsigned Dram_Addr;
	std::string type_name;
};



class Prim_Load : public Prim_Mem {
public:
	Prim_Load(unsigned scratchpad_addr = 0, unsigned length = 0, unsigned dram_addr = 0)
		: Prim_Mem(scratchpad_addr, length, dram_addr)
	{
		type_name = "Load";
		this->prim_id_code.range(3, 0) = "0010";
	}	
};

class Prim_Store : public Prim_Mem {
public:
	Prim_Store(unsigned scratchpad_addr = 0, unsigned length = 0, unsigned dram_addr = 0)
		: Prim_Mem(scratchpad_addr, length, dram_addr)
	{
		type_name = "Store";
		this->prim_id_code.range(3, 0) = "0011";
	}
};


class Prim_Jump : public PrimPacket
{
public:
	Prim_Jump(int relative_addr_in_buffer = 0)
		: jump_addr(relative_addr_in_buffer)
	{
		this->prim_id_code.range(67, 52) = sc_bv<16>(jump_addr);
		type_name = "Jump";
		this->prim_id_code.range(3, 0) = "0001";
	}

public:
	sc_int<16> jump_addr;
	std::string type_name;
};

class Prim_Memristor : public PrimPacket {
public:
	Prim_Memristor(unsigned scratchpad_addr = 0, unsigned length = 0)
		: Scrachpad_Addr(scratchpad_addr), Data_Length(length) {

		type_name = "Memristor";
		this->prim_id_code.range(19, 4) = sc_bv<16>(scratchpad_addr);
		this->prim_id_code.range(35, 20) = sc_bv<16>(length);
		this->prim_id_code.range(3, 0) = "1000";



	}

public:
	unsigned Scrachpad_Addr;
	unsigned Data_Length;
	std::string type_name;

};
class Prim_Matrix_Mul : public PrimPacket {
public:
	Prim_Matrix_Mul(unsigned left_addr = 0, unsigned left_height = 0, unsigned left_width = 0,
		unsigned right_addr = 0, unsigned right_width = 0, unsigned bias_addr = 0, 
		unsigned output_addr = 0)
		:Left_Addr(left_addr), Left_Height(left_height), Left_Width(left_width),
		Right_Addr(right_addr), Right_Width(right_width), Bias_Addr(bias_addr),
		Output_Addr(output_addr){
			type_name = "Matrix_Mul";
			this->prim_id_code.range(19, 4) = sc_bv<16>(left_addr);
			this->prim_id_code.range(35, 20) = sc_bv<16>(left_height);
			this->prim_id_code.range(51, 36) = sc_bv<16>(left_width);
			this->prim_id_code.range(67, 52) = sc_bv<16>(right_addr);
			this->prim_id_code.range(83, 68) = sc_bv<16>(right_width);
			this->prim_id_code.range(99, 84) = sc_bv<16>(bias_addr);
			this->prim_id_code.range(115, 100) = sc_bv<16>(output_addr);
			this->prim_id_code.range(3, 0) = "0101";
		}

public:
	unsigned Left_Addr;
	unsigned Left_Height;
	unsigned Left_Width;
	unsigned Right_Addr;
	unsigned Right_Width;
	unsigned Bias_Addr;
	unsigned Output_Addr;
	std::string type_name;

};
class Prim_ReLU : public PrimPacket {
public:
	Prim_ReLU(unsigned input_addr = 0, unsigned data_length = 0, unsigned threshold = 0, unsigned output_addr = 0)
		:Input_Addr(input_addr), Data_Length(data_length), Threshold(threshold), Output_Addr(output_addr)
	{
		type_name = "ReLU";
		this->prim_id_code.range(19, 4) = sc_bv<16>(input_addr);
		this->prim_id_code.range(35, 20) = sc_bv<16>(data_length);
		this->prim_id_code.range(67, 52) = sc_bv<16>(threshold);
		this->prim_id_code.range(115, 100) = sc_bv<16>(output_addr);
		this->prim_id_code.range(3, 0) = "0100";
	}
public:
	unsigned Input_Addr;
	unsigned Data_Length;
	unsigned Threshold;
	unsigned Output_Addr;
	std::string type_name;
};
class Prim_Conv2d : public PrimPacket {
public:
	Prim_Conv2d(unsigned input_addr = 0, unsigned input_height = 0, unsigned input_width = 0,
		unsigned kernel_addr = 0, unsigned in_channels = 0, unsigned out_channels = 0,
		unsigned kernel_size = 0, unsigned padding = 0, unsigned stride = 0,
		unsigned is_bias = 0, unsigned bias_addr = 0,  unsigned output_addr = 0)
		:Input_Addr(input_addr),Input_Height(input_height),Input_Width(input_width),
	Kernel_Addr(kernel_addr),In_Channels(in_channels),Out_Channels(out_channels),
	Kernel_Size(kernel_size),Padding(padding),Stride(stride),
	Is_Bias(is_bias), Bias_Addr(bias_addr),Output_Addr(output_addr){
		type_name = "Conv2d";
		this->prim_id_code.range(19, 4) = sc_bv<16>(input_addr);
		this->prim_id_code.range(35, 20) = sc_bv<16>(input_height);
		this->prim_id_code.range(51, 36) = sc_bv<16>(input_width);
		this->prim_id_code.range(67, 52) = sc_bv<16>(kernel_addr);
		this->prim_id_code.range(72, 68) = sc_bv<5>(in_channels);
		this->prim_id_code.range(77, 73) = sc_bv<5>(out_channels);
		this->prim_id_code.range(80, 78) = sc_bv<3>(kernel_size);
		this->prim_id_code.range(81, 81) = sc_bv<1>(padding);
		this->prim_id_code.range(82, 82) = sc_bv<1>(stride);
		this->prim_id_code.range(83, 83) = sc_bv<1>(is_bias);
		this->prim_id_code.range(99, 84) = sc_bv<16>(bias_addr);
		this->prim_id_code.range(115, 100) = sc_bv<16>(output_addr);
		this->prim_id_code.range(3, 0) = "0110";
	}



public:
	unsigned Input_Addr;
	unsigned Input_Height;
	unsigned Input_Width;
	unsigned Kernel_Addr;
	unsigned In_Channels;
	unsigned Out_Channels;
	unsigned Kernel_Size;
	unsigned Padding;
	unsigned Stride;
	unsigned Is_Bias;
	unsigned Bias_Addr;
	unsigned Output_Addr;
	std::string type_name;
};
class Prim_Pooling : public PrimPacket {
public:
	Prim_Pooling(unsigned input_addr = 0, unsigned input_height = 0, unsigned input_width = 0,
		unsigned in_channels = 0, unsigned out_channels = 0,
		unsigned kernel_size = 0, unsigned padding = 0, unsigned stride = 0,
		unsigned is_bias = 0,  unsigned output_addr = 0)
		:Input_Addr(input_addr), Input_Height(input_height), Input_Width(input_width),
		In_Channels(in_channels), Out_Channels(out_channels),
		Kernel_Size(kernel_size), Padding(padding), Stride(stride),
		Is_Bias(is_bias),  Output_Addr(output_addr) {
		type_name = "Conv2d";
		this->prim_id_code.range(19, 4) = sc_bv<16>(input_addr);
		this->prim_id_code.range(35, 20) = sc_bv<16>(input_height);
		this->prim_id_code.range(51, 36) = sc_bv<16>(input_width);
		this->prim_id_code.range(72, 68) = sc_bv<5>(in_channels);
		this->prim_id_code.range(77, 73) = sc_bv<5>(out_channels);
		this->prim_id_code.range(80, 78) = sc_bv<3>(kernel_size);
		this->prim_id_code.range(81, 81) = sc_bv<1>(padding);
		this->prim_id_code.range(82, 82) = sc_bv<1>(stride);
		this->prim_id_code.range(83, 83) = sc_bv<1>(is_bias);
		this->prim_id_code.range(115, 100) = sc_bv<16>(output_addr);
		this->prim_id_code.range(3, 0) = "0111";
	}

public:
	unsigned Input_Addr;
	unsigned Input_Height;
	unsigned Input_Width;
	unsigned In_Channels;
	unsigned Out_Channels;
	unsigned Kernel_Size;
	unsigned Padding;
	unsigned Stride;
	unsigned Is_Bias;
	unsigned Output_Addr;
	std::string type_name;
};