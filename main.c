#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "ID.h"

// Noms des tubes (FIFOs)
#define C_TO_R "fifo_client_to_router"
#define R_TO_C "fifo_router_to_client"
#define R_TO_S "fifo_router_to_server"
#define S_TO_R "fifo_server_to_router"

#define BUFFER_SIZE 256

// --- LOGIQUE DU CLIENT ---
void run_client() {
    char input[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    fprintf(stderr, "[Client] Prêt. Entrez le code (12 chiffres) : ");
    if (fgets(input, sizeof(input), stdin) != NULL) {
        input[strcspn(input, "\n")] = 0; // Nettoyage \n

        // 1. Envoyer au routeur
        int fd_out = open(C_TO_R, O_WRONLY);
        write(fd_out, input, strlen(input) + 1);
        close(fd_out);
        fprintf(stderr, "[Client] Message envoyé au routeur.\n");

        // 2. Attendre réponse
        int fd_in = open(R_TO_C, O_RDONLY);
        read(fd_in, response, BUFFER_SIZE);
        close(fd_in);

        printf("[Client] Résultat reçu : %s\n", response);
    }
}

// --- LOGIQUE DU SERVEUR DE ROUTAGE ---
void run_router() {
    char buffer[BUFFER_SIZE];
    
    fprintf(stderr, "[Routeur] Démarré.\n");

    // Étape A : Recevoir du client
    int fd_c_in = open(C_TO_R, O_RDONLY);
    read(fd_c_in, buffer, BUFFER_SIZE);
    close(fd_c_in);
    fprintf(stderr, "[Routeur] Routage du message : %s\n", buffer);

    // Étape B : Envoyer au serveur de données
    int fd_s_out = open(R_TO_S, O_WRONLY);
    write(fd_s_out, buffer, strlen(buffer) + 1);
    close(fd_s_out);

    // Étape C : Recevoir du serveur de données
    int fd_s_in = open(S_TO_R, O_RDONLY);
    read(fd_s_in, buffer, BUFFER_SIZE);
    close(fd_s_in);

    // Étape D : Renvoyer au client
    int fd_c_out = open(R_TO_C, O_WRONLY);
    write(fd_c_out, buffer, strlen(buffer) + 1);
    close(fd_c_out);
}

// --- LOGIQUE DU SERVEUR DE DONNÉES ---
void run_server() {
    char buffer[BUFFER_SIZE];
    char result[BUFFER_SIZE];

    int fd_in = open(R_TO_S, O_RDONLY);
    read(fd_in, buffer, BUFFER_SIZE);
    close(fd_in);

    fprintf(stderr, "[DataServer] Traitement du code : %s\n", buffer);
    
    // Simulation de recherche de menu
    if(buffer[ == 1234){
    	snprintf(result, BUFFER_SIZE, "Menu pour le lieu %.*s : Tofu Royal, Nouilles aux poulets, Nems, Riz cantonais", 4, buffer + 4);
    }else if(*(buffer + 4) == 5678){
    	snprintf(result, BUFFER_SIZE, "Menu pour le lieu %.*s : Tasty Crousty", 4, buffer + 4);
    }else if(*(buffer + 4) == 9123){ 
    	snprintf(result, BUFFER_SIZE, "Menu pour le lieu %.*s : Sandwich au poulet, Sandwich au thon, Sandwich au jambon, Sandwich végétarien", 4, buffer + 4);
    }else{ 
    	snprintf(result, BUFFER_SIZE, "Menu introuvable pour le lieu %.*s", 4, buffer + 4);
   }
    	
    int fd_out = open(S_TO_R, O_WRONLY);
    write(fd_out, result, strlen(result) + 1);
    close(fd_out);
}

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
        }
    }

    // Nettoyage après fermeture (optionnel)
    unlink(C_TO_R);
    unlink(R_TO_C);
    unlink(R_TO_S);
    unlink(S_TO_R);

    return 0;
}
