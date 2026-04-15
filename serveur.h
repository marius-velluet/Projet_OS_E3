#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "threading.h"

#define BUFFER_SIZE  256
#define ROUTER_IP    "127.0.0.1"
#define ROUTER_PORT_S  8081        // Correspond au PORT_SERVERS du routeur
#define BACKLOG      5

// fd de la socket connectée au routeur, partagé entre les deux threads
static int fd_router = -1;

// -------------------------------------------------------
// Thread lecteur : reçoit les requêtes du routeur
// -------------------------------------------------------
void *reader_thread(void *arg) {
    (void)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        int n = read(fd_router, buffer, BUFFER_SIZE);
        if (n <= 0) {
            fprintf(stderr, "[Reader] Connexion perdue avec le routeur.\n");
            break;
        }
        buffer[n] = '\0';
        fprintf(stderr, "[Reader] Reçu : %s\n", buffer);
        enqueue(buffer);
    }
    return NULL;
}

// -------------------------------------------------------
// Thread worker : traite les requêtes et répond au routeur
// -------------------------------------------------------
void *worker_thread(void *arg) {
    (void)arg;
    char buffer[BUFFER_SIZE];
    char result[BUFFER_SIZE];

    while (1) {
        dequeue(buffer);
        fprintf(stderr, "[Worker] Traitement : %s\n", buffer);

        // Parsing des identifiants 
        char id_str1[5];
        strncpy(id_str1, buffer + 4, 4);
        id_str1[4] = '\0';
        int id_ville = atoi(id_str1);

        char id_str2[5];
        strncpy(id_str2, buffer + 8, 4);
        id_str2[4] = '\0';
        int id_menu = atoi(id_str2);

        snprintf(result, BUFFER_SIZE, "Menu introuvable");

        void bienvenue(char const *restaurant, char const *ville) {
            printf("Bienvenue au restaurant %s à %s\n", restaurant, ville);
        }

        if (id_ville == 1) {
            if (id_menu == 1234) {
                system("feh images/menu1.jpeg &");
                bienvenue("Burger Roi", "Noisy");
                snprintf(result, BUFFER_SIZE, "OK");
            }
            if (id_menu == 5678) {
                system("feh images/menu4.jpeg &");
                bienvenue("Bahn mi", "Noisy");
                snprintf(result, BUFFER_SIZE, "OK");
            }
        } else if (id_ville == 2) {
            if (id_menu == 1234) {
                system("feh images/menu2.jpeg &");
                bienvenue("Tasty Crousty", "Bussy");
                snprintf(result, BUFFER_SIZE, "OK");
            }
            if (id_menu == 5678) {
                system("feh images/menu5.jpeg &");
                bienvenue("Restaurant volonté Thaï", "Bussy");
                snprintf(result, BUFFER_SIZE, "OK");
            }
        } else if (id_ville == 3) {
            if (id_menu == 5678) {
                system("feh images/menu3.jpeg &");
                bienvenue("Shushii", "Créteil");
                snprintf(result, BUFFER_SIZE, "OK");
            }
            if (id_menu == 9123) {
                system("feh images/menu6.jpeg &");
                bienvenue("Coré1", "Créteil");
                snprintf(result, BUFFER_SIZE, "OK");
            }
        } else {
            snprintf(result, BUFFER_SIZE, "Menu introuvable pour le lieu %.*s", 4, buffer + 4);
        }

        // Réponse au routeur — même fd, plus de boucle d'open()
        write(fd_router, result, strlen(result) + 1);
    }
    return NULL;
}

// -------------------------------------------------------
// Point d'entrée du serveur de données
// -------------------------------------------------------
void run_server() {

    // SETUP : connexion au routeur (le serveur est client du routeur ici)
    fd_router = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd_router == -1) {
        fprintf(stderr, "[Serveur] Erreur création socket.\n");
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ROUTER_IP);
    addr.sin_port        = htons(ROUTER_PORT_S);

    if (connect(fd_router, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        fprintf(stderr, "[Serveur] Erreur connexion au routeur.\n");
        close(fd_router);
        return;
    }
    fprintf(stderr, "[Serveur] Connecté au routeur.\n");

    // Lancement des threads
    pthread_t t1, t2;
    pthread_create(&t1, NULL, reader_thread, NULL);
    pthread_create(&t2, NULL, worker_thread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    close(fd_router);
}
