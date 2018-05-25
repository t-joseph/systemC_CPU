//------------------------------------------------------------------------
// Controller bus peripheral interface (TLM) logic for the tracker module
// Design Name : bpi
// File Name : bpi.cpp
// Function : This is a bus peripheral interface block, which interacts
// BUS module and internal CPU functional block(systemC)
//-----------------------------------------------------------------------


#include "bpi.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

bpi::bpi(sc_module_name nm)
           : socket("socket")  // Construct and name socket
{
  addrGpsFrame = 0;
  addrGsmFrame = 128;
  SC_THREAD(bpiCpuRdGpsFrame);
  SC_THREAD(bpiBusWrGpsFrame);
  SC_THREAD(bpiCpuRdGsmFrame);
  SC_THREAD(bpiBusWrGsmFrame);
  SC_THREAD(bpiCpuWr);
}

void bpi::bpiCpuRdGpsFrame()
{

  int index, availChar;
  while(1)
  {
    wait(10500, SC_MS);
    availChar = bpi_i->num_available();
    memset(data, 0, frameSize);
    index = 0;
    while(bpi_i->num_available() != 0)
    {
      bpiRd_f = bpi_i->read();
      data[index] = bpiRd_f;
      index++;
    }
    data[index] = '\0';

    if(availChar == 64)
    {
      cout<<"@" << sc_time_stamp() <<" :: <bpi> Reading GPS frame from FuncBlock"
      <<", num_available"<< availChar
      <<endl;
      bpiBusWrGpsFrame_event.notify();
    }
    wait(bpiCpuRdGpsFrame_event);
  }
}


void bpi::bpiBusWrGpsFrame()
{
  while(1)
  {
    wait(bpiBusWrGpsFrame_event);
    // TLM-2 generic payload transaction, reused across calls to b_transport
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(100, SC_NS);


    tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;

    // initialize 8 out of the 10 attributes, byte_enable_length and extensions being unused
    trans->set_command( cmd );
    trans->set_address( addrGpsFrame );
    trans->set_data_ptr( reinterpret_cast<unsigned char*>(data) );
    trans->set_data_length( frameSize );
    trans->set_streaming_width( frameSize ); // = data_length to indicate no streaming
    trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
    trans->set_dmi_allowed( false ); // Mandatory initial value
    trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value


    socket->b_transport( *trans, delay );  // Blocking transport call

    // initiator obliged to check response status and delay
    if ( trans->is_response_error() )
      SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

    cout << "<bpi> trans = { " << 'W' << ", " << addrGpsFrame
         << " } , data = " << data << " at time " << sc_time_stamp()
         << " delay = " << delay << endl;
    // Realize the delay annotated onto the transport call
    wait(delay);
    bpiCpuRdGsmFrame_event.notify();
  }
}

void bpi::bpiCpuRdGsmFrame()
{
  int index, availChar;
  while(1)
  {
    wait(bpiCpuRdGsmFrame_event);
    wait(500, SC_NS);
    availChar = bpi_i->num_available();
    memset(data, 0, frameSize);
    index = 0;
    while(bpi_i->num_available() != 0)
    {
      bpiRd_f = bpi_i->read();
      data[index] = bpiRd_f;
      index++;
    }
    data[index] = '\0';

    if(availChar == 46)
    {
      cout<<"@" << sc_time_stamp() <<" :: <bpi> Reading GSM frame from FuncBlock"
      <<", num_available"<< availChar
      <<endl;
      bpiBusWrGsmFrame_event.notify();
    }
  }
}





void bpi::bpiBusWrGsmFrame()
{
  while(1)
  {
    wait(bpiBusWrGsmFrame_event);
    // TLM-2 generic payload transaction, reused across calls to b_transport
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(100, SC_NS);

    tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;

    // initialize 8 out of the 10 attributes, byte_enable_length and extensions being unused
    trans->set_command( cmd );
    trans->set_address( addrGsmFrame );
    trans->set_data_ptr( reinterpret_cast<unsigned char*>(data) );
    trans->set_data_length( frameSize );
    trans->set_streaming_width( frameSize ); // = data_length to indicate no streaming
    trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
    trans->set_dmi_allowed( false ); // Mandatory initial value
    trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

    socket->b_transport( *trans, delay );  // Blocking transport call

    // initiator obliged to check response status and delay
    if ( trans->is_response_error() )
      SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

    cout << "<bpi> trans = { " << 'W' << ", " << addrGsmFrame
         << " } , data = " << data << " at time " << sc_time_stamp()
         << " delay = " << delay << endl;
    // Realize the delay annotated onto the transport call
    wait(delay);

  bpiCpuWr_event.notify();
  }
}


void bpi::bpiCpuWr()
{
  int index;
  while(1)
  {
    wait(bpiCpuWr_event);
    // TLM-2 generic payload transaction, reused across calls to b_transport
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(100, SC_NS);


    tlm::tlm_command cmd = tlm::TLM_READ_COMMAND;

    // initialize 8 out of the 10 attributes, byte_enable_length and extensions being unused
    trans->set_command( cmd );
    trans->set_address( addrGpsFrame );
    trans->set_data_ptr( reinterpret_cast<unsigned char*>(data) );
    trans->set_data_length( frameSize );
    trans->set_streaming_width( frameSize ); // = data_length to indicate no streaming
    trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
    trans->set_dmi_allowed( false ); // Mandatory initial value
    trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

    socket->b_transport( *trans, delay );  // Blocking transport call

    // initiator obliged to check response status and delay
    if ( trans->is_response_error() )
      SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

    cout << "<bpi> trans = { " << 'R' << ", " << addrGpsFrame
         << " } , data = " << data << " at time " << sc_time_stamp()
         << " delay = " << delay << endl;
    // Realize the delay annotated onto the transport call
    wait(delay);


    cout<<"@" << sc_time_stamp() <<" ::<bpi> Writing GPS frame to FuncBlock"
    <<endl;
	  for(index = 0; data[index]; index++)
    {
      bpi_o->write(data[index]);
    }
    memset(data, 0, frameSize);

    // initialize 8 out of the 10 attributes, byte_enable_length and extensions being unused
    trans->set_command( cmd );
    trans->set_address( addrGsmFrame );
    trans->set_data_ptr( reinterpret_cast<unsigned char*>(data) );
    trans->set_data_length( frameSize );
    trans->set_streaming_width( frameSize ); // = data_length to indicate no streaming
    trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
    trans->set_dmi_allowed( false ); // Mandatory initial value
    trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

    socket->b_transport( *trans, delay );  // Blocking transport call

    // initiator obliged to check response status and delay
    if ( trans->is_response_error() )
      SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

      cout << "<bpi> trans = { " << 'R' << ", " << addrGsmFrame
      << " } , data = " << data << " at time " << sc_time_stamp()
      << " delay = " << delay << endl;

      // Realize the delay annotated onto the transport call
      wait(delay);


    cout<<"@" << sc_time_stamp() <<" ::<bpi> Writing GSM frame to FuncBlock"
    <<endl;
    for(index = 0; data[index]; index++)
    {
      bpi_o->write(data[index]);
    }
    memset(data, 0, frameSize);

    addrGpsFrame++;
    addrGsmFrame++;

    //To reset the memory index after wraparound
    if(addrGpsFrame == 127)
      addrGpsFrame = 0;
    if(addrGsmFrame == 255)
      addrGsmFrame = 128;

    bpiCpuRdGpsFrame_event.notify();
  }
}
