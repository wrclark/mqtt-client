#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

#define QUEUE_SIZE 128

typedef struct {
    void *items[QUEUE_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} queue_t;


void queue_init(queue_t *q);
int queue_empty(queue_t *q);
int queue_full(queue_t *q);
void queue_push(queue_t *q, void *item);
void *queue_pop(queue_t *q);

#endif
