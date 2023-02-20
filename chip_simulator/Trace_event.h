#pragma once
#include "systemc.h"
#include<iostream>



/*


Event Type

	Sram_rdwt_type

	Dram_rdwt_type

	core_busy_type

	Dram_burst_rdwt_type

	Jump_prim_type
	


 




 */

using  namespace std;
class Trace_event
{
public:
	Trace_event(string _event_name, string _info, string _type, double _time);
	~Trace_event();



	string event_name;
	string info;
	string type;
	//sc_event* clock_event;
	double time;

};

