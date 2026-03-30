#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_QUEUE 10
#define BUFFER_SIZE 256
char queue[MAX_QUEUE][BUFFER_SIZE];
int q_head = 0, q_tail = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void enqueue(char *msg) {
    pthread_mutex_lock(&mutex);

    strcpy(queue[q_tail], msg);
    q_tail = (q_tail + 1) % MAX_QUEUE;

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void dequeue(char *msg) {
    pthread_mutex_lock(&mutex);

    while (q_head == q_tail) {
        pthread_cond_wait(&cond, &mutex);
    }

    strcpy(msg, queue[q_head]);
    q_head = (q_head + 1) % MAX_QUEUE;

    pthread_mutex_unlock(&mutex);
}





