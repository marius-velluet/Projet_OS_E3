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

int main() {
	char buffer[BUFFER_SIZE];
	//Création permier pipe nomme
	int result = mkfifo (FIFO_REQ, 0777);
	if (result != 0){
		printf("Ne peut cree '%s'\n", FIFO_REQ);
		return 0;
	}
	//Création deuxième pipe nomme 
	result = mkfifo(FIFO_RESP, 0777);
	if (result != 0){
		printf("Ne peut cree '%s'\n", FIFO_RESP);
		return 0;
	}
	//Ouverture en lecture du premier pipe
	int fd_in = open(FIFO_REQ, O_RDONLY);
	if (fd_in == -1){
		printf("Ne peut ouvrir '%s'\n", FIFO_REQ);
		return 0;
	}
	//Lecture bloquante sur le premier pipe 
	result = read(fd_in, buffer, BUFFER_SIZE);
	if (result < 0){
		printf("Ne peut lire '%s'\n", FIFO_REQ);
		return 0;
	}
	printf("Recu : '%s'\n", buffer);
	//Ouverture en ecriture du second pipe 
	int fd_out = open(FIFO_RESP, O_WRONLY);
	if (fd_out == -1) {
		printf("Ne peut ouvrir '%s'\n", FIFO_RESP);
		return 0;
	}
	//Ecriture sur le second pipe
	result = write(fd_out, buffer, result);
	if (result < 0){
		printf("Ne peut ecrire '%s'\n", FIFO_RESP);
		return 0;
	}
}
