
#include "tb_controller.h"
#include <fstream>

using namespace sc_core;
using namespace sc_dt;
using namespace std;


// Constructor for the tb_controller

tb_controller::tb_controller(sc_module_name nm)
           : socket("socket")
  {
    // Register callback for incoming b_transport interface method call
    socket.register_b_transport(this, &tb_controller::b_transport);
    SC_THREAD(test);
    SC_THREAD(tb_debug);

  } // End of Constructor


  void tb_controller:: b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
  {

    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address();
    unsigned char*   ptr = trans.get_data_ptr();
    unsigned int     len = trans.get_data_length();
    unsigned char*   byt = trans.get_byte_enable_ptr();
    unsigned int     wid = trans.get_streaming_width();


    if (adr >= sc_dt::uint64(SIZE) || byt != 0 || len > SIZE || wid < len)
     SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");

      // Obliged to implement read and write commands
    if ( cmd == tlm::TLM_READ_COMMAND )
    {
        memcpy(ptr, mem[adr], len);
    }
    else if ( cmd == tlm::TLM_WRITE_COMMAND )
    {
        memcpy(mem[adr], ptr, len);
    }

      // Obliged to set response status to indicate successful completion
    trans.set_response_status( tlm::TLM_OK_RESPONSE );
  }


void tb_controller:: test()
{

  string testGpsFrame;
  string testGsmFrame;
  char userRequest[] = "TrackerId8";
  int testCount = 9;
  int i = 0;
  int index;

  ifstream myGpsFile ("tb_gps.txt");
  ifstream myGsmFile ("tb_gsm.txt");
  cout<<"************************************************************************"
  "******************************************************************************"
  "**********************************" <<endl;
  cout<<"@" << sc_time_stamp()<<" :: <TB> Entering TestBench" <<endl;

  if (myGpsFile.is_open() && myGsmFile.is_open())
  {

    while(testCount)
    {
      i += 1;
      cout<<"@" << sc_time_stamp()<<" :: <TB> Test Run "
      <<i
      <<endl;

      if(i == 2 || i == 7) //In testcase 2 and 7, generate user request interrupts
      {
        for(index = 0; userRequest[index]; index++)
        {
          tb_gsm_o->write(userRequest[index]);
        }
      }

      getline (myGpsFile,testGpsFrame);
      for(index = 0; testGpsFrame[index]; index++)
      {
        tb_gps_o->write(testGpsFrame[index]);
      }

      while(1)
      {
    	  tb_gsmRd_f = tb_gsm_i->read();
        gsmReq.push_back(tb_gsmRd_f);
        if(gsmReq.compare("power?") == 0)
        {
          gsmReq.clear();
          break;
        }
      }

      getline (myGsmFile,testGsmFrame);
      for(index = 0; testGsmFrame[index]; index++)
      {
        tb_gsm_o->write(testGsmFrame[index]);
      }

      if(i == 9)
      {
        tb_debug_element_event.notify();
        wait(tb_debug_element_return_event);
        wait(300, SC_MS);
      }
      else
      {
        wait(2000, SC_MS);
      }
      while(tb_gsm_i->num_available() != 0)
      {
        tb_gsmRd_f = tb_gsm_i->read();
        dataFrame.push_back(tb_gsmRd_f);
      }
      cout<<"@" << sc_time_stamp()<<" :: <TB> Received Dataframe - "
      <<dataFrame<<endl;
      cout<<"******************************************************************"
      "************************************************************************"
      "**********************************************" <<endl;
      dataFrame.clear();
      testCount -= 1;
      wait(200, SC_MS);
    }
  }
  myGpsFile.close();
  myGsmFile.close();
  sc_stop();
 }


void tb_controller :: tb_debug()
{
  int index;
  char debugOutput[] = "THIS IS A DBTEST";
  while(1)
  {
    wait(tb_debug_element_event);
    tb_dbgEn_o.write(1);
    for(index = 0; debugOutput[index]; index++)
    {
      tb_tdo->write(debugOutput[index]);
    }

    tb_dbgCapture_o.write(1);
    wait(1500, SC_MS);
    tb_dbgScan_o.write(1);
    wait(1500, SC_MS);
    tb_dbgCapture_o.write(0);
    tb_dbgUpdate_o.write(1);
    wait(1500, SC_MS);
    while(1)
    {
      tb_debug_f = tb_tdi->read();
      debug.push_back(tb_debug_f);
      if(tb_tdi->num_available() == 0)
      {
        break;
      }
    }
    cout<<"@" << sc_time_stamp()<<" :: <TB - Debug> Debug Scan - "
    <<debug<<endl;
    tb_dbgScan_o.write(0);
    tb_dbgEn_o.write(0);
    tb_dbgUpdate_o.write(0);
    tb_debug_element_return_event.notify();
  }
}
