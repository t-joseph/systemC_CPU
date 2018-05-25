
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

  int testCount = 9;
  int i = 0;

  ifstream myGpsFile ("tb_gps.txt");
  ifstream myGsmFile ("tb_gsm.txt");
  cout<<"************************************************************************"
  "******************************************************************************"
  "**********************************************" <<endl;
  cout<<"@" << sc_time_stamp()<<" :: <TB> Entering TestBench" <<endl;

  if (myGpsFile.is_open() && myGsmFile.is_open())
  {

    while(testCount)
    {
      i += 1;
      cout<<"@" << sc_time_stamp()<<" :: <TB> Test Run "
      <<i
      <<endl;

      getline (myGpsFile,testGpsFrame);
      for(int i = 0; testGpsFrame[i]; i++)
      {
        tb_gps_o->write(testGpsFrame[i]);
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
      for(int i = 0; testGsmFrame[i]; i++)
      {
        tb_gsm_o->write(testGsmFrame[i]);
      }

      wait(500, SC_MS);
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

    }
  }
  myGpsFile.close();
  myGsmFile.close();
  sc_stop();
 }
