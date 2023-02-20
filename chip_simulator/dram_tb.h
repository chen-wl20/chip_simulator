#include "dram.h"
#include "systemc.h"
#include<iostream>

enum dram_mode
{
	RESET = 0,
	READ,
	WRITE,
	BURST_READ,
	BURST_WRITE
};

class dram_tb : public sc_module{
public:
	SC_HAS_PROCESS(dram_tb);
	dram_tb(sc_module_name nm) 
	{
		SC_THREAD(burst_read_data);
		sensitive << mode;
		dont_initialize();

		SC_THREAD(convert_mode);
	}
	
	void convert_mode() {
		wait(10, SC_NS);
		mode.write(RESET);
		wait(10, SC_NS);
		mode.write(BURST_READ);
		wait(10, SC_NS);
		mode.write(BURST_WRITE);
		wait(10, SC_NS);
		mode.write(BURST_READ);
		wait(10, SC_NS);
		sc_stop();
	}

	void burst_read_data() {
		while (true) {
			if (mode.read() == BURST_READ) {
				sc_bv<128>* out_data;
				dram_port->burst_read(address.read(), length.read(), out_data);
				std::cout << "reading:" << std::endl;
				for (unsigned int i = 0; i < length.read(); i++) {
					std::cout << out_data[i] << ", ";
				}
				std::cout << std::endl;
				//wait(5, SC_NS);
			}
			else if (mode.read() == BURST_WRITE) {
				sc_bv<128>* in_data = new sc_bv<128>[length.read()];
				for (unsigned int i = 0; i < length.read(); i++) {
					in_data[i] = test_data;
				}
				dram_port->burst_write(address.read(), length.read(), in_data);
				//wait(5, SC_NS);
			}
			else if (mode.read() == RESET) {
				dram_port->reset();
				//wait(5, SC_NS);
			}
			wait();
		}
	}


public:
	sc_port<burst_ram_if<sc_bv<128> > > dram_port;
	sc_in<unsigned>  address;
	sc_in<unsigned>  length;
	sc_signal<dram_mode> mode;
	sc_in<bool> clock;
	sc_bv<128> test_data = 0x123456789;
};