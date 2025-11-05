#ifndef HANDLERS_SERV_H
#define HANDLERS_SERV_H

#include "serv_cli_fifo.h"

extern int ack_received; 

/* Handler for SIGUSR1 - wake up server when client has read the response */
void hand_reveil() {
 ack_received = 1;
  
}

/* Handler for termination - clean up before server exit */
void fin_serveur() {
printf("\nInterruption clavier! Fermeture du serveur");
    /* Remove the FIFO files */
    unlink(FIFO1);
    unlink(FIFO2);
    /* Exit the program */
    exit(0);
}

#endif /* HANDLERS_SERV_H */
