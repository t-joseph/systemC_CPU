#ifndef __top_controller_h
#define __top_controller_h

#include "tb_controller.h"
#include "controller.h"
#include "bpi.h"
#include "interruptGen.h"


using namespace sc_core;
using namespace sc_dt;
using namespace std;


SC_MODULE(top_controller)
{
	public:

	//FIFOs/Signals
	sc_fifo<char> gps_f;
	sc_fifo<char> gsm1_f;
	sc_fifo<char> gsm2_f;
	sc_fifo<char> gsm3_f;
	sc_fifo<char> cpuBpi_f;
	sc_fifo<char> bpiCpu_f;
	sc_signal<bool> intr_ch1;
	sc_signal<bool> intr_ch2;


	SC_HAS_PROCESS(top_controller);
	top_controller (sc_module_name nm);


	private:
	controller* cont;
	tb_controller* tb_cont;
	bpi* bpi_cont;
	interruptGen* interruptGen_cont;
};


#endif
