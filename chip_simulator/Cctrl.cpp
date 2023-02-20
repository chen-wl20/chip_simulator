
#include "Cctrl.h"
#include "config.h"


void Cctrl::dispatch_prim()
{
	while (true) {
		if (this->reset.read()) {
			mem_prim_start.write(false);

			this->pc_jump.write(false);
			this->from_start.write(false);
		}
		else if (this->ctrl_status.read() == EXECUTING) {
			unsigned type_code = prim_type_code.read().to_uint();
			//std::cout << prim_type_code.read() << std::endl;
			switch (type_code)
			{
			case 0b0010:
			case 0b0011:
				this->pc_jump.write(false);
				mem_prim_start.write(true);
				wait(mem_busy_i.negedge_event());
				mem_prim_start.write(false);

				this->to_idle.write(false);
				break;

			case 0b0001:
				this->pc_jump.write(true);
				wait(jump_busy_i.negedge_event());
				this->pc_jump.write(false);

				this->to_idle.write(false);
				break;

			case 0b0000:
				this->from_start.write(true);
				wait(jump_busy_i.negedge_event());
				this->from_start.write(false);
				this->to_idle.write(true);
				break;


			case 0b1000:
				this->pc_jump.write(false);
				this->memristor_prim_start.write(true);
				wait(memristor_busy_i.negedge_event());
				this->memristor_prim_start.write(false);
				this->to_idle.write(false);
				break;
			case 0b0101:
				this->pc_jump.write(false);
				this->matrix_mul_prim_start.write(true);
				wait(matrix_mul_busy_i.negedge_event());
				this->matrix_mul_prim_start.write(false);
				this->to_idle.write(false);
				break;
			case 0b0100:
				this->pc_jump.write(false);
				this->relu_prim_start.write(true);
				wait(relu_busy_i.negedge_event());
				this->relu_prim_start.write(false);
				this->to_idle.write(false);
				break;
			case 0b0110:
				this->pc_jump.write(false);
				this->conv2d_prim_start.write(true);
				wait(conv2d_busy_i.negedge_event());
				this->conv2d_prim_start.write(false);
				this->to_idle.write(false);
				break;
			case 0b0111:
				this->pc_jump.write(false);
				this->pooling_prim_start.write(true);
				wait(pooling_busy_i.negedge_event());
				this->pooling_prim_start.write(false);
				this->to_idle.write(false);
				break;
			default:
				this->matrix_mul_prim_start.write(false);
				this->mem_prim_start.write(false);
				this->conv2d_prim_start.write(false);
				this->pooling_prim_start.write(false);
				this->from_start.write(true);
				wait(jump_busy_i.negedge_event());
				this->from_start.write(false); 
				this->to_idle.write(true);
				break;
			}
		}
		wait();
	}
}

// next_prim call fetch unit start
void Cctrl::fetch_and_decode()
{
	while (true) {
		if (this->reset.read()) {
			this->next_prim.write(false);
			
		}
		else if (this->ctrl_status.read() == PROCESSING) {
			this->next_prim.write(true);
			wait(this->fetch_finish.posedge_event());
			this->next_prim.write(false);
		}
		wait();
	}

}

void Cctrl::core_busy_combinational_logic()
{
	while (true) {
		this->core_busy.write(
			this->mem_busy_i.read() || this->jump_busy_i.read() || this->memristor_busy_i.read() || this->matrix_mul_busy_i.read()||this->relu_busy_i.read()||this->conv2d_busy_i.read()||this->pooling_busy_i.read()
		);
		wait();
	}
}

void Cctrl::core_busy_event()
{
	while(true)
	{
		if (!(this->reset.read()))
		{
			/*sc_event E_core_busy;
			E_core_busy->notify();*/
			sync_event->notify();
			string name = "core_busy_event";
			string info = "Event of core busy state";
			string type = "core_busy_type";
			double time = sc_time_stamp().to_double();
			Trace_event TE_core_busy(name, info, type, time);
			Trace_event_queue_ctrl->trace_event_queue.push(TE_core_busy);

		}
		wait();
	}
}


void Cctrl::core_idle_event()
{
	while (true)
	{
		if (!(this->reset.read()))
		{
			/*sc_event* E_core_busy;
			E_core_busy = new sc_event;
			E_core_busy->notify();*/
			sync_event->notify();
			string name = "core_idle_event";
			string info = "Event of core idle state";
			string type = "core_busy_type";
			double time = sc_time_stamp().to_double();
			Trace_event TE_core_idle(name, info, type, time);
			Trace_event_queue_ctrl->trace_event_queue.push(TE_core_idle);

		}
		wait();
	}
}



/*


State machine:


PROCESSING : start fetch prim;

EXECUTING  : start prim execute;


*/
void Cctrl::ctrl_status_trans()
{
	while (true) {
		if (this->reset.read()) {
			this->ctrl_status.write(CORE_IDLE);
		}
		else if (this->to_idle.read()) {
			this->ctrl_status.write(CORE_IDLE);
		}
		else if (!core_idle.read() && this->ctrl_status.read() == CORE_IDLE) {
			this->ctrl_status.write(PROCESSING);
		}
		else if (!this->core_busy.read() && this->ctrl_status.read() == EXECUTING) {
			this->ctrl_status.write(PROCESSING);
		}
		else if (this->fetch_finish.read() && this->ctrl_status.read() == PROCESSING) {
			this->ctrl_status.write(EXECUTING);
		}
		wait();
	}
}


Cctrl::~Cctrl()
{
}
