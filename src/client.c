#include "serv_cli_fifo.h"
#include "Handlers_Cli.h"
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#define SEM_PRINT "/print_lock"



int main(){
    /* --- Déclarations --- */
    int fd_fifo1, fd_fifo2;
    struct question quest;
    struct reponse resp;
    int i;

    /* --- Ouverture du sémaphore d'impression --- */
    sem_t *print_mutex = sem_open(SEM_PRINT, 0);
    if (print_mutex == SEM_FAILED) {
        perror("sem_open print_mutex");
        exit(1);
    }

    srand(getpid());

    /* --- Ouverture des tubes nommés --- */
    fd_fifo1 = open(FIFO1, O_WRONLY);
    if (fd_fifo1 == -1) {
        perror("open FIFO1");
        exit(1);
    }

    fd_fifo2 = open(FIFO2, O_RDONLY);
    if (fd_fifo2 == -1) {
        perror("open FIFO2");
        close(fd_fifo1);
        exit(1);
    }

    /* --- Installation du Handler --- */
    signal(SIGUSR1, hand_reveil);

    /* --- Construction et envoi d'une question --- */
    quest.client_id = getpid();
    quest.n = rand() % NMAX + 1;

 

    write(fd_fifo1, &quest, sizeof(struct question));

    /* --- Attente de la réponse du serveur --- */
    while (signal_recu == 0) {
        pause();
    }

    /* --- Lecture de la réponse --- */
    read(fd_fifo2, &resp, sizeof(struct reponse));

    /* --- Envoi du signal SIGUSR1 au serveur --- */
    kill(resp.server_id, SIGUSR1);

    /* --- Affichage synchronisé de la réponse --- */
    sem_wait(print_mutex);
    printf("Client %d : Demande de %d nombres aléatoires\n", getpid(), quest.n);
    printf("Client %d : Réponse reçue du serveur %d\n", getpid(), resp.server_id);
    printf("→ Nombres générés : ");
    for (i = 0; i < quest.n; i++) {
        printf("%d ", resp.numbers[i]);
    }
    printf("\n");
    printf("Client %d : Envoi de l'accusé de réception au serveur %d\n\n", getpid(), resp.server_id);
    sem_post(print_mutex);

    /* --- Fermeture des descripteurs --- */
    close(fd_fifo1);
    close(fd_fifo2);

    /* --- Fermeture du sémaphore --- */
    sem_close(print_mutex);

    return 0;
}
