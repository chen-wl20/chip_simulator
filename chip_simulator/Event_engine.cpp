#include "Event_engine.h"


void Event_engine::engine_run()
{
	while(1)
	{
		wait(*sync_event);
		cout << "sync event begin" << sc_time_stamp() <<endl;
		while (Trace_event_queue_clock_engine->trace_event_queue.size() != 0)
		{
			if (Trace_event_queue_clock_engine->trace_event_queue.front().type == "core_busy_type") {
				cout << "Event time " << Trace_event_queue_clock_engine->trace_event_queue.front().time << endl;
				cout << "Event info " << Trace_event_queue_clock_engine->trace_event_queue.front().info << endl;
				if (Trace_event_queue_clock_engine->trace_event_queue.front().event_name == "core_busy_event")
				{
					core_idle_time += Trace_event_queue_clock_engine->trace_event_queue.front().time - core_idle_last;
					core_busy_last = Trace_event_queue_clock_engine->trace_event_queue.front().time;
				}
				if (Trace_event_queue_clock_engine->trace_event_queue.front().event_name == "core_idle_event")
				{
					core_busy_time += Trace_event_queue_clock_engine->trace_event_queue.front().time - core_busy_last;
					core_idle_last = Trace_event_queue_clock_engine->trace_event_queue.front().time;
				}

				Trace_event_queue_clock_engine->trace_event_queue.pop();
			}
			else
			{
			
				cout << "Event time " << Trace_event_queue_clock_engine->trace_event_queue.front().time << endl;
				cout << "Event info " << Trace_event_queue_clock_engine->trace_event_queue.front().info << endl;
				Trace_event_queue_clock_engine->trace_event_queue.pop();
			}

		}
	}

}


Event_engine::~Event_engine()
{
}
