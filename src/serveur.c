#include "serv_cli_fifo.h"
#include "Handlers_Serv.h"
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>

#define SEM_SERVER "/server_lock"
#define SEM_PRINT  "/print_lock"
void ignore_signal(){
    signal(SIGINT,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
}
void disable_terminal_stop_signals() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ISIG; // Désactive les signaux générés par Ctrl+C, Ctrl+Z, etc.
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
int main()
{
    /* Déclarations */
    int fd_fifo1, fd_fifo2;
    struct question quest;
    struct reponse resp;
    int i;
    int bytes_read;
    /* --- Ignore les signaux d’interruption utilisateur --- */
    ignore_signal();
    disable_terminal_stop_signals();

    /* Nettoyage et création des sémaphores */
    sem_unlink(SEM_SERVER);
    sem_unlink(SEM_PRINT);


    sem_t *sem_server = sem_open(SEM_SERVER, O_CREAT | O_EXCL, 0666, 1);
    if (sem_server == SEM_FAILED)
    {
        perror("Server: sem_open server_lock");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_print = sem_open(SEM_PRINT, O_CREAT | O_EXCL, 0666, 1);
    if (sem_print == SEM_FAILED)
    {
        perror("Server: sem_open print_lock");
        sem_close(sem_server);
        sem_unlink(SEM_SERVER);
        exit(EXIT_FAILURE);
    }

    /* Installation des gestionnaires */
    // for (int j = 1; j < 32; j++)
    // {
        signal(SIGTERM, fin_serveur);
 
    signal(SIGUSR1, hand_reveil);

    /* Création des tubes nommés si inexistants */
    mkfifo(FIFO1, 0666);
    mkfifo(FIFO2, 0666);

    srand(getpid());

    sem_wait(sem_print);
    printf("Server started with PID %d\n", getpid());
    printf("Server: Waiting for client connection...\n");
    sem_post(sem_print);

    /* Ouverture des tubes nommés (bloquant jusqu’à connexion client) */
    fd_fifo1 = open(FIFO1, O_RDONLY);
    fd_fifo2 = open(FIFO2, O_WRONLY);

    while (1)
    {
        /* Lecture d'une requête client */
        bytes_read = read(fd_fifo1, &quest, sizeof(struct question));

        if (bytes_read == sizeof(struct question))
        {
            sem_wait(sem_print);
            printf("Server: Received request from client %d for %d numbers\n",
                   quest.client_id, quest.n);
            sem_post(sem_print);

            /* Construction de la réponse */
            resp.client_id = quest.client_id;
            resp.server_id = getpid();
            for (i = 0; i < quest.n; i++)
                resp.numbers[i] = rand() % NMAX + 1;

            /* Envoi de la réponse */
            sem_wait(sem_server); // Protège l’accès aux FIFOs
            if (write(fd_fifo2, &resp, sizeof(struct reponse)) == sizeof(struct reponse))
            {
                sem_post(sem_server);
                if (kill(quest.client_id, SIGUSR1) == 0)
                {
                    sem_wait(sem_print);
                    printf("Server: Sent %d numbers to client %d\n",
                           quest.n, quest.client_id);
                    printf("Server: Waiting for client acknowledgment...\n");
                    sem_post(sem_print);

                    ack_received = 0;
                    while (ack_received == 0)
                        pause();

                    sem_wait(sem_print);
                    printf("Server: Client %d acknowledged receipt\n", quest.client_id);
                    sem_post(sem_print);
                }
                else
                {
                    sem_wait(sem_print);
                    printf("Server: Failed to signal client %d\n", quest.client_id);
                    sem_post(sem_print);
                }
            }
            else
            {
                sem_post(sem_server);
                sem_wait(sem_print);
                printf("Server: Failed to write response to client %d\n", quest.client_id);
                sem_post(sem_print);
            }
        }
        else if (bytes_read == 0)
        {
            /* Aucun client connecté — reviens en attente */
            sem_wait(sem_print);
            printf("Server: All clients disconnected. Waiting for new connections...\n");
            sem_post(sem_print);

            close(fd_fifo1);
            close(fd_fifo2);
            fd_fifo1 = open(FIFO1, O_RDONLY);
            fd_fifo2 = open(FIFO2, O_WRONLY);
        }
    }

    /* Nettoyage final (jamais atteint en boucle infinie) */
    sem_close(sem_server);
    sem_close(sem_print);
    sem_unlink(SEM_SERVER);
    sem_unlink(SEM_PRINT);

    return 0;
}
