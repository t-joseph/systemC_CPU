#include "top_controller.h"
#include "tb_controller.cpp"
#include "controller.cpp"
#include "bpi.cpp"
#include "interruptGen.cpp"

using namespace sc_core;
using namespace sc_dt;
using namespace std;


//Contructor

top_controller::top_controller(sc_module_name nm)
	: sc_module(nm)
	, gps_f(64), gsm1_f(46), gsm2_f(110), cpuBpi_f(64), bpiCpu_f(64), top_tdi_f(16),top_tdo_f(16)
{
	cout << "ConstructorCPP - Top:" << name() << endl;

	tb_cont = new tb_controller("TestBench");
	cont = new controller("Controller");
	bpi_cont = new bpi("BPI");
	interruptGen_cont = new interruptGen("InterruptGeneration");


	cont->gps_i(gps_f);
	tb_cont->tb_gps_o(gps_f);

	interruptGen_cont->gsm_i(gsm1_f);
	tb_cont->tb_gsm_o(gsm1_f);

	interruptGen_cont->gsmInt_o(gsm3_f);
	cont->gsmInt_i(gsm3_f);

	cont->gsm_o(gsm2_f);
	tb_cont->tb_gsm_i(gsm2_f);

	bpi_cont->bpi_i(bpiCpu_f);
	cont->cpu_o(bpiCpu_f);

	bpi_cont->bpi_o(cpuBpi_f);
	cont->cpu_i(cpuBpi_f);

	interruptGen_cont->intr2Cont_o(intr_ch1);
	cont->intr_i(intr_ch1);

	bpi_cont->intr_i(intr_ch2);
	interruptGen_cont->intr2Bpi_o(intr_ch2);

	cont->dbg_i(top_tdi_f);
	tb_cont->tb_tdo(top_tdi_f);
	cont->dbg_o(top_tdo_f);
	tb_cont->tb_tdi(top_tdo_f);
	cont->dbgEn_i(top_dbgEn_i);
	tb_cont->tb_dbgEn_o(top_dbgEn_i);
	cont->dbgCapture_i(top_dbgCapture_i);
	tb_cont->tb_dbgCapture_o(top_dbgCapture_i);
	cont->dbgUpdate_i(top_dbgUpdate_i);
	tb_cont->tb_dbgUpdate_o(top_dbgUpdate_i);
	cont->dbgScan_i(top_dbgScan_i);
	tb_cont->tb_dbgScan_o(top_dbgScan_i);

	// Bind initiator socket to target socket
	bpi_cont->socket.bind( tb_cont->socket );


}
