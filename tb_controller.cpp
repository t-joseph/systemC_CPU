
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
