#include "controller.h"


using namespace sc_core;
using namespace sc_dt;
using namespace std;

	//---------------- internal fifo-----------------------------------------------------------------------------------------
	sc_fifo<char>	buff_register(FIFO_LEN);

	//---------------- debug data regiester----------------------------------------------------------------------------------
	sc_fifo<char>	data_register(FIFO_LEN);


//---------------- thread : exchange the TDI data with module data then send it to TDO ------------------------------------------

void MOD_NAME::THD_Debugg(void)
{
	int w= 0;

//---------------- data to load in module regiester when only debug element needs to be tested
	char ch;
	cout << "Debug Module Initialized here "<< endl;

//---------------- while loop for keep the logic runing -------------------------------------------------------------------------
	while(1)
	{
		wait(debug_element_event);
		if(dbgEn_i == true)
		{
			//---------------- Debug Capture ----------------------------------------------------------------------
			if((w != 1) && (dbgCapture_i == 1)  )	//debug Capture condition
			{

				w = 1;	// seting varible to 1 : to prevent multiple exhicution
				cout <<  endl << "\t<------ Debug CAPTURE in progress ------>" << endl ;

				//wait(1000,SC_MS);

				//---------------- storing data from module register to debug register---------------------------
				while(1)
				{
					ch = dbg_write_buff.read();
					data_register.write(ch);
					buff_register.write(ch);
					if (dbg_write_buff.num_available() == 0)
					  break;
				}
				wait(100,SC_MS);

				while(1)
				{
					ch = buff_register.read();
					dbg_write_buff.write(ch);
					if (buff_register.num_available() == 0)
					  break;
				}

				cout << endl <<"\t<------ End of CAPTURE ------>" <<endl;

				wait(500,SC_MS);
			}
			//----------------end of capture logic ------------------------------------------------------------------

			//---------------- Debug Scan ------------------------------------------------------------------------

			if ((w != 2) && (dbgScan_i == 1)  )	// checking for debug scan condition
			{
				w =2;					// set w to 2 when debug scan is executed
				cout << "\t<------ Debug SCAN is in progress ------>" << endl ;

				//---------------- shifting the data from debug register to TDO----------------------------------
				while(1)
				{
					ch = data_register.read();
					decipher_tdo.write(ch);
					if(data_register.num_available() == 0)
					{
						cipher_security_event.notify();
						wait(cipher_security_return_event);
						break;
					}
				}

				//---------------- shifing the data from TDI to debug register-----------------------------------
				while(1)
				{
					ch = decipher_tdi.read();
					data_register.write(ch);
					if(decipher_tdi.num_available() == 0)
					  break ;
				}
				cout << endl << "\t<------ End of SCAN ------>" << endl;
			}
			//---------------- End of Debug scan logic--------------------------------------------------------------

			//---------------- Debug Update--------------------------------------------------------------------------
			if ( (w != 3) && (dbgUpdate_i == 1) )		// condition for debug scan
			{
				w = 3;						// update w to 3 when update is exhicuted
				cout << endl << "\t<------ Debug UPDATE is in progress ------>" << endl;

				while(1)
				{
					ch = dbg_write_buff.read();
					if(dbg_write_buff.num_available() == 0)
					  break ;
				}
				//-----------------------------------------------------------------------------------------------

				//---------------- store data to module register from debug regiester----------------------------
				while(1)
				{
					ch = data_register.read();
					dbg_write_buff.write(ch);
					if(data_register.num_available() == 0)
					  break ;
				}
				//-----------------------------------------------------------------------------------------------

				cout << endl <<"\t<------ End of UPDATE ------>" << endl;

				wait(1500,SC_MS);
			}
		}
		debug_element_return_event.notify();
	}
	//---------------- End of while loop-------------------------------------------------------------------------------------

}
//---------------- End of thread-------------------------------------------------------------------------------------------------
