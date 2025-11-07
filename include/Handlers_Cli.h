#ifndef HANDLERS_CLI_H
#define HANDLERS_CLI_H

#include "serv_cli_fifo.h"

/* Variable globale pour indiquer la reception du signal */
volatile sig_atomic_t signal_recu = 0;


void hand_reveil(int sig ) {
	(void)sig;
    signal_recu = 1;
}

#endif 
