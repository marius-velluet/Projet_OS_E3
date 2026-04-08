#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <sys/wait.h>
#include "ID.h"
#include "routage.h"
#include "serveur.h"

// Noms des tubes (FIFOs)
#define C_TO_R "fifo_client_to_router"
#define R_TO_C "fifo_router_to_client"
#define R_TO_S "fifo_router_to_server"
#define S_TO_R "fifo_server_to_router"

#define BUFFER_SIZE 256



// --- MAIN : GESTION DES PROCESSUS ---
int main() {
    // Création des FIFOs
    mkfifo(C_TO_R, 0666);
    mkfifo(R_TO_C, 0666);
    mkfifo(R_TO_S, 0666);
    mkfifo(S_TO_R, 0666);

    pid_t p1, p2;

    p1 = fork();
    if (p1 == 0) {
        run_router();
        exit(0);
    } else {
        p2 = fork();
        if (p2 == 0) {
            run_server();
            exit(0);
        } else {
            // Le parent devient le client pour faciliter l'interaction clavier
            run_client();
            wait(NULL);
            wait(NULL);
        }
    }

    // Nettoyage après fermeture (optionnel)
    unlink(C_TO_R);
    unlink(R_TO_C);
    unlink(R_TO_S);
    unlink(S_TO_R);

    return 0;
}
