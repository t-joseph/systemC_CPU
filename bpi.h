#ifndef __bpi_h
#define __bpi_h

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"


// Bus Peripheral Interface module generating generic payload transactions
SC_MODULE (bpi)
{

  public:
  //input port
  sc_port<sc_fifo_in_if<char> >     bpi_i ;      //
  //output port
  sc_port<sc_fifo_out_if<char> >    bpi_o ;      //

  sc_in<bool>                      intr_i;

  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_initiator_socket<bpi> socket;


  SC_HAS_PROCESS(bpi);
  bpi (sc_module_name nm);

  // Enum datatype defining constants
  enum {frameSize = 128};

  //local variable
  char data[frameSize];
  int addrGpsFrame;
  int addrGsmFrame;


  private:

  //signals
  sc_fifo<char> bpiRd_f;

  //events
  sc_event interrupt_event;
  sc_event bpiCpuRdGpsFrame_event;
  sc_event bpiBusWrGpsFrame_event;
  sc_event bpiCpuRdGsmFrame_event;
  sc_event bpiBusWrGsmFrame_event;
  sc_event bpiCpuWr_event;

  //thread functions
  void interProcess();
  void bpiCpuRdGpsFrame();
  void bpiBusWrGpsFrame();
  void bpiCpuRdGsmFrame();
  void bpiBusWrGsmFrame();
  void bpiCpuWr();
};

#endif
