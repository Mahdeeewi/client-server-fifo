#ifndef HANDLERS_CLI_H
#define HANDLERS_CLI_H

#include "serv_cli_fifo.h"

/* Variable globale pour indiquer la reception du signal */
int signal_recu = 0;


void hand_reveil() {
    signal_recu = 1;
}

#endif 
