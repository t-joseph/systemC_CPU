#include "key.h"
#include "controller.h"


#ifndef FIFO_LEN
#define FIFO_LEN 64
#endif

void MOD_NAME::Cipher_main(void) // Scoping operator(::) means that the thread Cipher_main belongs to Module Cipher
{
  char var;
  char result;
  while(1){
    wait(cipher_security_event);
    wait(500, SC_MS);
    for(int i=0; i<=FIFO_LEN; i++)
    {
      decipher_tdo.read(var);
      if(decipher_tdo.num_available() == 0)
        break;
	    result = char((int(var)+key2-32)%96+32);
      dbg_o->write(result);
    }
    cipher_security_return_event.notify();
  }
}
