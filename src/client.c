#include "serv_cli_fifo.h"
#include "Handlers_Cli.h"

int main() {
    /* Déclarations */
    int fd_fifo1, fd_fifo2;
    struct question quest;
    struct reponse resp;
    int i;
    
    srand(getpid());
    
    /* Ouverture des tubes nommés */
    fd_fifo1 = open(FIFO1, O_WRONLY);
    fd_fifo2 = open(FIFO2, O_RDONLY);
  
    /* Installation des Handlers */
    signal(SIGUSR1, hand_reveil);
    
    /* Construction et envoi d'une question */
    quest.client_id = getpid();
    quest.n = rand() % NMAX + 1;
    printf("Client %d: Requesting %d random numbers\n", getpid(), quest.n);
    write(fd_fifo1, &quest, sizeof(struct question));
    
    /* Attente de la réponse */
    while(signal_recu == 0) {
        pause();
    }
    
    /* Lecture de la réponse */
    read(fd_fifo2, &resp, sizeof(struct reponse));
    
    /* envoi du signal SIGUSR au serveur */
    printf("Client %d: Response read, Sending acknowledgment to server\n", getpid());
    kill(resp.server_id, SIGUSR1);
    
    /* Traitement local de la réponse */
    printf("Client %d received %d random numbers: ", getpid(), quest.n);
    for(i = 0; i < quest.n; i++) {
        printf("%d ", resp.numbers[i]);
    }
    printf("\n");
    
    close(fd_fifo1);
    close(fd_fifo2);
    
    return 0;
}
