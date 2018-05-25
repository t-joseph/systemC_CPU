
//-----------------------------------------------------
// Controller test bench module
// Design Name : tb_controller
// File Name : tb_controller.h
// Function : This is the header file of test bench for controller mod
//-----------------------------------------------------

#ifndef __tb_controller_h
#define __tb_controller_h

#include "systemc.h"
#include <string>
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

#define SC_INCLUDE_DYNAMIC_PROCESSES

using namespace sc_core;
using namespace sc_dt;
using namespace std;

SC_MODULE (tb_controller) {

  public:
  //output ports
  sc_port<sc_fifo_out_if<char> >     tb_gps_o ;      //
  sc_port<sc_fifo_out_if<char> >     tb_gsm_o ;      //

  //input ports
  sc_port<sc_fifo_in_if<char> >    tb_gsm_i ;     //

  tlm_utils::simple_target_socket<tb_controller> socket;

  // Local Variables
  string dataFrame;
  string gsmReq;

  enum { SIZE = 256};

  char mem[SIZE][SIZE];

  SC_HAS_PROCESS(tb_controller);
  tb_controller (sc_module_name nm);

  virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );

  private:
  //signals
  sc_fifo<char> tb_gsmRd_f;

  void test();
};

#endif
