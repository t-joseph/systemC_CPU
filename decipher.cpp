#include "key.h"
#include "controller.h"

#ifndef FIFO_LEN
#define FIFO_LEN 64
#endif
/*
#ifndef key
#define key 3
#endif
*/


void MOD_NAME::Decipher_main(void)
{
  char var;
  char result;

  while(1)
  {
    wait(decipher_security_event);
    wait(500, SC_MS);

    for(int i=0; i<=FIFO_LEN; i++)
    {
      dbg_i->read(var);
      if(dbg_i->num_available() == 0)
        break;
    	result = char(((int(var)+key1+64)%96) +32);
      decipher_tdi.write(result);
    }
    debug_element_event.notify();
  }


}
