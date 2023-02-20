#include "Trace_event.h"



Trace_event::Trace_event(string _event_name, string _info , string _type, double _time)
{
	event_name = _event_name;
	info = _info;
	type = _type;
	//clock_event = _clock_event;
	time = _time;
}


Trace_event::~Trace_event()
{

	/*if (clock_event != NULL)
	{

		delete clock_event;

	}*/
}
