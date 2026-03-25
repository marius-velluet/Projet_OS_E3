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
#define R_TO_S "fifo_router_to_server"
#define S_TO_R "fifo_server_to_router"

// --- LOGIQUE DU SERVEUR DE DONNÉES ---
void run_server() {
    char buffer[BUFFER_SIZE];
    char result[BUFFER_SIZE];

    int fd_in = open(R_TO_S, O_RDONLY);
    read(fd_in, buffer, BUFFER_SIZE);
    close(fd_in);

    fprintf(stderr, "[DataServer] Traitement du code : %s\n", buffer);
    
    // Simulation de recherche de menu
    char id_str[5]; // 4 chiffres + '\0'
    
    // Copier les 4 caractères à partir de buffer+4
    strncpy(id_str, buffer + 4, 4);
    id_str[4] = '\0';

    // Conversion en entier
    int id_menu = atoi(id_str);
    
    if(id_menu == 1234){
    	system("feh images/menu1.jpeg &");
    	//snprintf(result, BUFFER_SIZE, "Menu pour le lieu %.*s : Tofu Royal, Nouilles aux poulets, Nems, Riz cantonais", 4, buffer + 4);
    }else if(id_menu == 5678){
    	snprintf(result, BUFFER_SIZE, "Menu pour le lieu %.*s : Tasty Crousty", 4, buffer + 4);
    }else if(id_menu == 9123){ 
    	snprintf(result, BUFFER_SIZE, "Menu pour le lieu %.*s : Sandwich au poulet, Sandwich au thon, Sandwich au jambon, Sandwich végétarien", 4, buffer + 4);
    }else{ 
    	snprintf(result, BUFFER_SIZE, "Menu introuvable pour le lieu %.*s", 4, buffer + 4);
   }
    	
    int fd_out = open(S_TO_R, O_WRONLY);
    write(fd_out, result, strlen(result) + 1);
    close(fd_out);
}

