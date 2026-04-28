#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "ID.h"
#include "routage.h"
#include "serveur.h"

int main() {
    pid_t p_router, p_server;

    /* 1. Lancer le routeur en premier */
    p_router = fork();
    if (p_router == 0) {
        run_router();
        exit(0);
    }

    /* Laisser le temps au routeur d'ouvrir ses sockets avant
       que le serveur tente de se connecter */
    sleep(1);

    /* 2. Lancer le serveur de données */
    p_server = fork();
    if (p_server == 0) {
        run_server();
        exit(0);
    }

    /* Laisser le temps au serveur de se connecter au routeur avant
       que le client envoie sa requête */
    sleep(1);

    /* 3. Le parent devient le client (interaction clavier dans le terminal) */
    run_client();

    /* Attendre la fin des deux fils */
    waitpid(p_router, NULL, 0);
    waitpid(p_server, NULL, 0);

    return 0;
}
