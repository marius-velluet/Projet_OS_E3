#ifndef THREADING_H
#define THREADING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_QUEUE   10
#define BUFFER_SIZE 256

static char queue[MAX_QUEUE][BUFFER_SIZE];
static int q_head = 0, q_tail = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;

static inline void enqueue(char *msg) {
    pthread_mutex_lock(&mutex);
    strncpy(queue[q_tail], msg, BUFFER_SIZE - 1);
    queue[q_tail][BUFFER_SIZE - 1] = '\0';
    q_tail = (q_tail + 1) % MAX_QUEUE;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

static inline void dequeue(char *msg) {
    pthread_mutex_lock(&mutex);
    while (q_head == q_tail) {
        pthread_cond_wait(&cond, &mutex);
    }
    strncpy(msg, queue[q_head], BUFFER_SIZE - 1);
    msg[BUFFER_SIZE - 1] = '\0';
    q_head = (q_head + 1) % MAX_QUEUE;
    pthread_mutex_unlock(&mutex);
}

#endif /* THREADING_H */
