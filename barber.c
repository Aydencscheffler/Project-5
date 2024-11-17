// barber.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "common.h"
#include "common_threads.h"
#include "zemaphore.h"

#define NUM_CUSTOMERS 20

int n; // number of chairs
int waiting = 0;
pthread_mutex_t mutex;
Zem_t customers;
Zem_t barber;

void *barber_thread(void *arg) {
    while (1) {
        Zem_wait(&customers); // Sleep if no customers
        Mutex_lock(&mutex);
        waiting--;
        Zem_post(&barber); // Ready to cut hair
        Mutex_unlock(&mutex);

        // Cutting hair
        printf("Barber is cutting hair\n");
        sleep(rand() % 3);
    }
    return NULL;
}

void *customer_thread(void *arg) {
    int id = *(int *)arg;
    free(arg);

    Mutex_lock(&mutex);
    if (waiting < n) {
        waiting++;
        Zem_post(&customers); // Wake up barber if necessary
        Mutex_unlock(&mutex);

        Zem_wait(&barber); // Wait for barber
        printf("Customer %d is getting a haircut\n", id);
    } else {
        Mutex_unlock(&mutex);
        printf("Customer %d is leaving (no chairs available)\n", id);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: barber n_chairs\n");
        exit(1);
    }

    n = atoi(argv[1]);

    pthread_t barber_t;
    pthread_t customers_t[NUM_CUSTOMERS];

    Mutex_init(&mutex);
    Zem_init(&customers, 0);
    Zem_init(&barber, 0);

    Pthread_create(&barber_t, NULL, barber_thread, NULL);

    // Create customer threads
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        sleep(rand() % 2);
        int *id = malloc(sizeof(int));
        *id = i;
        Pthread_create(&customers_t[i], NULL, customer_thread, id);
    }

    // Join customer threads
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        Pthread_join(customers_t[i], NULL);
    }

    
    return 0;
}
