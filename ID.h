#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFER_SIZE 256
#define ROUTER_IP   "127.0.0.1"
#define ROUTER_PORT 8080        // Correspond au PORT_CLIENTS du routeur

void run_client() {
    char input[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    fprintf(stderr, "[Client] Prêt. Entrez le code (12 chiffres) : ");
    if (fgets(input, sizeof(input), stdin) == NULL){
    	return;
    }
    input[strcspn(input, "\n")] = 0; // Nettoyage \n

    // -------------------------------------------------------
    // SETUP : création et connexion de la socket
    // -------------------------------------------------------
    int fd_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd_socket == -1) {
        fprintf(stderr, "[Client] Erreur création socket.\n");
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ROUTER_IP);
    addr.sin_port        = htons(ROUTER_PORT);

    if (connect(fd_socket, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        fprintf(stderr, "[Client] Erreur connexion au routeur.\n");
        close(fd_socket);
        return;
    }
    fprintf(stderr, "[Client] Connecté au routeur.\n");

    // -------------------------------------------------------
    // 1. Envoyer au routeur
    // -------------------------------------------------------
    write(fd_socket, input, strlen(input) + 1);
    fprintf(stderr, "[Client] Message envoyé au routeur.\n");

    // -------------------------------------------------------
    // 2. Attendre la réponse sur la même socket
    // -------------------------------------------------------
    int n = read(fd_socket, response, BUFFER_SIZE);
    if (n <= 0) {
        fprintf(stderr, "[Client] Erreur lecture réponse.\n");
        close(fd_socket);
        return;
    }
    printf("[Client] Résultat reçu : %s\n", response);

    // -------------------------------------------------------
    // Nettoyage
    // -------------------------------------------------------
    
    close(fd_socket);
}
