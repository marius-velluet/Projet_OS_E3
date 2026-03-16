#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <Message.h>
#include <Client.h>
#include <Database.h>
#include "routage.h"

//Paramètres 
int server_count = 0;
int client_count = 0;
int routing_count = 0;

int cache_size;


//Fonction de récupération du fichier de configuration 
void load_config(char *path){
    FILE *f = fopen(path, "r");
    if (!f) {
        perror("config");
        exit(1);
    }
    while (!feof(f)) {
        DataServer s;
        if (fscanf(f,"%d \"%[^\"]\" %d",
                   &s.id,
                   s.name,
                   &s.port) == 3) {
            servers[server_count++] = s;
        }
    }

    fclose(f);
}

//Création des pipes pour les serveurs 
void init_servers()
{
    char fifo_in[64];
    char fifo_out[64];

    for (int i=0;i<server_count;i++) {

        sprintf(fifo_in,"server_%d_in",servers[i].id);
        sprintf(fifo_out,"server_%d_out",servers[i].id);

        mkfifo(fifo_in,0666);
        mkfifo(fifo_out,0666);

        servers[i].fd_in = open(fifo_in,O_RDONLY | O_NONBLOCK);
        servers[i].fd_out = open(fifo_out,O_WRONLY | O_NONBLOCK);
    }
}

//Création des pipes pour les clients 
void init_clients(int max_clients)
{
    char fifo_in[64];
    char fifo_out[64];

    for(int i=0;i<max_clients;i++) {

        clients[i].id = i;

        sprintf(fifo_in,"client_%d_in",i);
        sprintf(fifo_out,"client_%d_out",i);

        mkfifo(fifo_in,0666);
        mkfifo(fifo_out,0666);

        clients[i].fd_in = open(fifo_in,O_RDONLY | O_NONBLOCK);
        clients[i].fd_out = open(fifo_out,O_WRONLY | O_NONBLOCK);

        client_count++;
    }
}

//Validation du message 
int validate_message(char *msg)
{
    if(strlen(msg) == 0)
        return 0;

    return 1;
}




	 
