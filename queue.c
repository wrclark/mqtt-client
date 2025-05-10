
#include "queue.h"

void queue_init(queue_t *q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;

    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

void queue_push(queue_t *q, void *item) {
    pthread_mutex_lock(&q->lock);

    /* wait for a pop before adding more */
    while (q->count == QUEUE_SIZE) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }

    q->items[q->tail] = item;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

void *queue_pop(queue_t *q) {
    void *item = NULL;
    pthread_mutex_lock(&q->lock);

    /* wait for a push before you can pop */
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }

    item = q->items[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return item;
}