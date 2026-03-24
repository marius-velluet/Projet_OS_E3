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
