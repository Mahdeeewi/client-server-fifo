#include "serv_cli_fifo.h"
#include "Handlers_Serv.h"

/* Global variable for acknowledgment tracking */
int ack_received = 0;

int main() {
    /* Déclarations */
    int fd_fifo1, fd_fifo2;
    struct question quest;
    struct reponse resp;
    int i;
    int bytes_read;
    
    /* Ignorer le signal SIGPIPE pour éviter la terminaison */
    signal(SIGPIPE, SIG_IGN);
    
    /* Installation des Handlers */
    signal(SIGUSR1, hand_reveil);
    signal(SIGINT, fin_serveur);
    
    /* Création des tubes nommés */
    mkfifo(FIFO1, 0666);
    mkfifo(FIFO2, 0666);
    
    /* Initialisation du générateur de nombres aléatoires */
    srand(getpid());
    
    printf("Server started with PID %d\n", getpid());
    printf("Press Ctrl+C to stop the server and clean up FIFOs\n");
    
    /* Ouverture des tubes nommés */
    printf("Server: Waiting for client connection...\n");
    fd_fifo1 = open(FIFO1, O_RDONLY);
    fd_fifo2 = open(FIFO2, O_WRONLY);
    
    while(1) {
        /* lecture d'une question (BLOCKS until client writes) */
        bytes_read = read(fd_fifo1, &quest, sizeof(struct question));
        
        if(bytes_read == sizeof(struct question)) {
            printf("Server: Received request from client %d for %d numbers\n", 
                   quest.client_id, quest.n);
            
            /* construction de la réponse */
            resp.client_id = quest.client_id;
            resp.server_id = getpid();
            for(i = 0; i < quest.n; i++) {
                resp.numbers[i] = rand() % NMAX + 1;
            }
            
            /* envoi de la réponse */
            if(write(fd_fifo2, &resp, sizeof(struct reponse)) == sizeof(struct reponse)) {
                /* envoi du signal SIGUSR1 au client concerné */
                if(kill(quest.client_id, SIGUSR1) == 0) {
                    printf("Server: Sent %d numbers to client %d\n", 
                           quest.n, quest.client_id);
                    printf("Server: Waiting for client acknowledgment...\n");
                    
                    /* ATTENTE DE L'ACCUSÉ DE RÉCEPTION */
                    ack_received = 0;
                    while(ack_received == 0) {
                        pause();  // Block until client sends SIGUSR1 back
                    }
                    printf("Server: Client %d acknowledged receipt\n", quest.client_id);
                    
                } else {
                    printf("Server: Failed to signal client %d\n", quest.client_id);
                }
            } else {
                printf("Server: Failed to write response to client %d\n", quest.client_id);
            }
        } else if (bytes_read == 0) {
            /* Client closed the connection */
            printf("Server: All clients disconnected. Waiting for new connections...\n");
            
            close(fd_fifo1);
            close(fd_fifo2);
            
            fd_fifo1 = open(FIFO1, O_RDONLY);
            fd_fifo2 = open(FIFO2, O_WRONLY);
        }
    }
    
    return 0;
}
