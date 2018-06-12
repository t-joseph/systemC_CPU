#include "interruptGen.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

// Constructor for the controller

interruptGen::interruptGen(sc_module_name nm)
           : sc_module(nm)
  {
	  SC_THREAD(interruptGenProcess);

  } // End of Constructor


//Interrupt generation
void interruptGen:: interruptGenProcess()
{
  int availChar, i;
  while(1)
  {
    wait(1, SC_MS);
    intr2Cont_o.write(0);
    intr2Bpi_o.write(0);
    availChar = gsm_i->num_available();
    
    while(gsm_i->num_available() != 0)
    {
      gsmRd_f = gsm_i->read();
      gsmData.push_back(gsmRd_f);
    }
    if(availChar != 46 && (gsmData.compare(0, 9, "TrackerId") == 0))
    {
      cout<<"@" << sc_time_stamp() <<" :: <Intr> Generating Interrupt"
      <<", num_available"<< availChar
      <<endl;
      intr2Cont_o.write(1);
      intr2Bpi_o.write(1);
      wait(1, SC_MS);
      intr2Cont_o.write(0);
      intr2Bpi_o.write(0);
      gsmData.clear();
    }
    else if(availChar == 46)
    {
      for(i = 0; gsmData[i]; i++)
      {
        gsmInt_o->write(gsmData[i]);
      }
      gsmData.clear();
    }
  }
}
