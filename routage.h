#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFER_SIZE 256
#define PORT_CLIENTS  8080   // port où les clients se connectent
#define PORT_SERVERS  8081   // port où les serveurs de données se connectent
#define BACKLOG       5

// --- LOGIQUE DU SERVEUR DE ROUTAGE ---
void run_router() {
    char buffer[BUFFER_SIZE];
    fprintf(stderr, "[Routeur] Démarré.\n");

    // -------------------------------------------------------
    // SETUP : socket d'écoute côté clients
    // -------------------------------------------------------
    int fd_listen_clients = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd_listen_clients == -1) {
        fprintf(stderr, "[Routeur] Erreur création socket clients.\n");
        return;
    }

    int opt = 1;
    setsockopt(fd_listen_clients, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr_clients;
    addr_clients.sin_family      = AF_INET;
    addr_clients.sin_addr.s_addr = INADDR_ANY;
    addr_clients.sin_port        = htons(PORT_CLIENTS);

    if (bind(fd_listen_clients, (struct sockaddr *)&addr_clients, sizeof(addr_clients)) != 0) {
        fprintf(stderr, "[Routeur] Erreur bind clients.\n");
        return;
    }
    listen(fd_listen_clients, BACKLOG);

    // -------------------------------------------------------
    // SETUP : socket d'écoute côté serveurs de données
    // -------------------------------------------------------
    int fd_listen_servers = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd_listen_servers == -1) {
        fprintf(stderr, "[Routeur] Erreur création socket serveurs.\n");
        return;
    }

    setsockopt(fd_listen_servers, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr_servers;
    addr_servers.sin_family      = AF_INET;
    addr_servers.sin_addr.s_addr = INADDR_ANY;
    addr_servers.sin_port        = htons(PORT_SERVERS);

    if (bind(fd_listen_servers, (struct sockaddr *)&addr_servers, sizeof(addr_servers)) != 0) {
        fprintf(stderr, "[Routeur] Erreur bind serveurs.\n");
        return;
    }
    listen(fd_listen_servers, BACKLOG);

    fprintf(stderr, "[Routeur] En écoute (clients:%d / serveurs:%d).\n",
            PORT_CLIENTS, PORT_SERVERS);

    // -------------------------------------------------------
    // Étape A : Accepter la connexion du client et lire
    // -------------------------------------------------------
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int fd_client = accept(fd_listen_clients,
                           (struct sockaddr *)&client_addr, &client_len);
    if (fd_client == -1) {
        fprintf(stderr, "[Routeur] Erreur accept client.\n");
        return;
    }

    int n = read(fd_client, buffer, BUFFER_SIZE);
    if (n <= 0) {
        fprintf(stderr, "[Routeur] Erreur lecture client.\n");
        return;
    }
    fprintf(stderr, "[Routeur] Reçu du client : %s\n", buffer);

    // -------------------------------------------------------
    // Étape B : Accepter la connexion du serveur de données et transmettre
    // -------------------------------------------------------
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);
    int fd_data_server = accept(fd_listen_servers,
                                (struct sockaddr *)&server_addr, &server_len);
    if (fd_data_server == -1) {
        fprintf(stderr, "[Routeur] Erreur accept serveur de données.\n");
        return;
    }

    write(fd_data_server, buffer, n);

    // -------------------------------------------------------
    // Étape C : Recevoir la réponse du serveur de données
    // -------------------------------------------------------
    n = read(fd_data_server, buffer, BUFFER_SIZE);
    if (n <= 0) {
        fprintf(stderr, "[Routeur] Erreur lecture serveur de données.\n");
        return;
    }
    fprintf(stderr, "[Routeur] Reçu du serveur de données : %s\n", buffer);

    // -------------------------------------------------------
    // Étape D : Renvoyer la réponse au client
    // -------------------------------------------------------
    write(fd_client, buffer, n);

    // -------------------------------------------------------
    // Nettoyage
    // -------------------------------------------------------
    close(fd_client);
    close(fd_data_server);
    close(fd_listen_clients);
    close(fd_listen_servers);
}

