#include "Ccontroller.h"


Ccontroller::~Ccontroller() {
	delete decoder;
	delete prim_ctrl;
	delete fetcher;
	delete start_prim_rom;


}

