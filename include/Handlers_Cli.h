#ifndef HANDLERS_CLI_H
#define HANDLERS_CLI_H

#include "serv_cli_fifo.h"

/* Global variable to indicate signal reception */
int signal_recu = 0;

/* Handler for SIGUSR1 - wake up client when response is ready */
void hand_reveil() {
    signal_recu = 1;
}

#endif /* HANDLERS_CLI_H */
