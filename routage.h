#include <fcntl.h> 
#include <stdio.h> 
#include <sys/stat.h> 
#include <unistd.h>
// for open, O_RDONLY, OWRONLY
// for printf
// for mkfifo
#define BUFFER_SIZE 256
#define FIFO_REQ "req.fifo"
#define FIFO_RESP "resp.fifo"
// Noms des tubes (FIFOs)
#define C_TO_R "fifo_client_to_router"
#define R_TO_C "fifo_router_to_client"
#define R_TO_S "fifo_router_to_server"
#define S_TO_R "fifo_server_to_router"

// --- LOGIQUE DU SERVEUR DE ROUTAGE ---
void run_router() {
    char buffer[BUFFER_SIZE];
    
    fprintf(stderr, "[Routeur] Démarré.\n");

    // Étape A : Recevoir du client
    int fd_c_in = open(C_TO_R, O_RDONLY);
    read(fd_c_in, buffer, BUFFER_SIZE);
    close(fd_c_in);
    fprintf(stderr, "[Routeur] Routage du message : %s\n", buffer);
    printf(".");
    usleep(2000000);
    printf(".");
    usleep(2000000);
    printf(".\n");
    usleep(2000000);

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

