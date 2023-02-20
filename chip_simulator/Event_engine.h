#pragma once
#include"systemc.h"
#include "Trace_event_queue.h"

class Event_engine : public sc_module
{
public:
	SC_HAS_PROCESS(Event_engine);
	Event_engine(sc_module_name name)
		:sc_module(name)
	{
		SC_THREAD(engine_run);
	}

	~Event_engine();
	void engine_run();
	double core_busy_time = 0;
	double core_idle_time = 0;
	double core_busy_last = 0;
	double core_idle_last = 0;
	Trace_event_queue *Trace_event_queue_clock_engine;
	sc_event* sync_event;
};

