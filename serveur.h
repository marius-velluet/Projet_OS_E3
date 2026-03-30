#include <fcntl.h> 
#include <stdio.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "threading.h"
// for open, O_RDONLY, OWRONLY
// for printf
// for mkfifo
#define BUFFER_SIZE 256
#define FIFO_REQ "req.fifo"
#define FIFO_RESP "resp.fifo"
// Noms des tubes (FIFOs)
#define R_TO_S "fifo_router_to_server"
#define S_TO_R "fifo_server_to_router"

#define MAX_QUEUE 10

void *reader_thread(void *arg) {
    char buffer[BUFFER_SIZE];

    int fd_in = open(R_TO_S, O_RDWR);
    if (fd_in < 0) {
        perror("open R_TO_S");
        exit(1);
    }

    while (1) {
        int n = read(fd_in, buffer, BUFFER_SIZE);

        if (n > 0) {
            buffer[n] = '\0';
            fprintf(stderr, "[Reader] Reçu : %s\n", buffer);
            enqueue(buffer);
        }
    }
}
// --- LOGIQUE DU SERVEUR DE DONNÉES ---  
void *worker_thread(void *arg) {

    char buffer[BUFFER_SIZE];
    char result[BUFFER_SIZE];
    
    while (1) {
        dequeue(buffer);

        fprintf(stderr, "[Worker] Traitement : %s\n", buffer);
    	// Simulation de recherche de ville
    
     	char id_str1[5]; // 4 chiffres + '\0'
    
    	// Copier les 4 caractères à partir de buffer+4
    	strncpy(id_str1, buffer + 4, 4);
    	id_str1[4] = '\0';

    	// Conversion en entier
    	int id_ville = atoi(id_str1);
    	//===================================
    	char id_str2[5]; // 4 chiffres + '\0'
    
    	// Copier les 4 caractères à partir de buffer+4
    	strncpy(id_str2, buffer + 8, 4);
    	id_str2[4] = '\0';

    	// Conversion en entier
    	int id_menu = atoi(id_str2);
    
    	//Fonction pour éviter la duplication de code
    	void bienvenue(char const* restaurant, char const * ville){
    	printf("Bienvenue au restaurant %s à %s\n", restaurant, ville);
    
    	}
    	//=========================================
    	if(id_ville == 0001){
    		if(id_menu == 1234){
    			system("feh images/menu1.jpeg &");
    			bienvenue("Burger Roi", "Noisy");
    		}
   	 	if(id_menu == 5678){
    			system("feh images/menu4.jpeg &");
    			bienvenue("Bahn mi", "Noisy");
    			}
    	}
    	else if(id_ville == 0002){
    	if(id_menu == 1234){
    		//snprintf(result, BUFFER_SIZE, "Menu pour le lieu %.*s : Tasty Crousty", 4, buffer + 4);
    		system("feh images/menu2.jpeg &");
    		bienvenue("Tasty Crousty", "Bussy");
    		}
    	if(id_menu == 5678){
    		system("feh images/menu5.jpeg &");
    		bienvenue("Restaurant volonté Thaï", "Bussy");
    		}
    
    	}
    
    	else if(id_ville == 0003){
    	if(id_menu == 5678){ 
    		//snprintf(result, BUFFER_SIZE, "Menu pour le lieu %.*s : Sandwich au poulet, Sandwich au thon, Sandwich au jambon, Sandwich végétarien", 4, buffer + 4);
    		system("feh images/menu3.jpeg &");
    		bienvenue("Shushii", "Créteil");
    		}
    		if(id_menu == 9123){
    		system("feh images/menu6.jpeg &");
    		bienvenue("Coré1", "Créteil");
    		}
    	}
    	else{ 
    		snprintf(result, BUFFER_SIZE, "Menu introuvable pour le lieu %.*s", 4, buffer + 4);
   	}
    	
    	int fd_out;
	while ((fd_out = open(S_TO_R, O_WRONLY)) < 0) {
    	usleep(100000);
	}
    	write(fd_out, result, strlen(result) + 1);
    	close(fd_out);
	}
}

void run_server() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, reader_thread, NULL);
    pthread_create(&t2, NULL, worker_thread, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}
