#ifndef HANDLERS_SERV_H
#define HANDLERS_SERV_H

#include "serv_cli_fifo.h"

/* Variable gloable pour ACK  */
 volatile sig_atomic_t ack_received; 


void hand_reveil(int sig) {
 (void)sig;
 ack_received = 1;
  
}


void fin_serveur(int sig) {
printf("\nSignal %d ! Fermeture du serveur",sig);
   
    unlink(FIFO1);
    unlink(FIFO2);
    
    exit(0);
}

#endif 
