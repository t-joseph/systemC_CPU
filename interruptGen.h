

#ifndef __interruptGen_h
#define __interruptGen_h

#include "systemc.h"
#include <string>

using namespace std;

SC_MODULE (interruptGen) {

  public:

  //input ports
  sc_port<sc_fifo_in_if<char> >     gsm_i;      //
  sc_port<sc_fifo_out_if<char> >    gsmInt_o;

  sc_out<bool>                      intr2Cont_o;
  sc_out<bool>                      intr2Bpi_o;

  // Local Variable
  string gsmData;

  // Constructor prototype
  SC_HAS_PROCESS(interruptGen);
  interruptGen (sc_module_name nm);

  private:

  //signals
  sc_fifo<char> gsmRd_f;

  //sleep thread
  void interruptGenProcess();

};
#endif
