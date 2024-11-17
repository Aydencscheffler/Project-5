#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>    // For usleep()
#include <time.h>
#include "common.h"
#include "common_threads.h"
#include "zemaphore.h"

int N;
Zem_t *forks;
int *eat_count;
double thinking_time = 0;
double eating_time = 0;
int *last_eat_time;
int deadlock_timeout = 3; // seconds
int running = 1;

pthread_mutex_t monitor_lock;
pthread_t *threads;          // Philosopher threads
pthread_t detector_thread;   // Deadlock detector thread

void *philosopher(void *arg) {
    int id = *(int *)arg;
    free(arg);

    // Enable thread cancellation
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    int left = id;
    int right = (id + 1) % N;

    while (running) {
        // Thinking
        printf("Philosopher %d is thinking\n", id);
        usleep(thinking_time * 1e6); // Convert seconds to microseconds

        // Check if running is set to 0 before attempting to pick up forks
        if (!running) break;

        // Pick up left fork
        Zem_wait(&forks[left]);
        printf("Philosopher %d picked up left fork %d\n", id, left);

        // Pick up right fork
        Zem_wait(&forks[right]);
        printf("Philosopher %d picked up right fork %d\n", id, right);

        // Eating
        printf("Philosopher %d is eating\n", id);
        eat_count[id]++;
        last_eat_time[id] = time(NULL);
        usleep(eating_time * 1e6); // Convert seconds to microseconds

        // Put down forks
        Zem_post(&forks[right]);
        printf("Philosopher %d put down right fork %d\n", id, right);

        Zem_post(&forks[left]);
        printf("Philosopher %d put down left fork %d\n", id, left);
    }
    printf("Philosopher %d is exiting\n", id);
    return NULL;
}

void *deadlock_detector(void *arg) {
    while (running) {
        sleep(deadlock_timeout);
        int all_waiting = 1;

        // Check if any philosopher has eaten recently
        time_t current_time = time(NULL);
        for (int i = 0; i < N; i++) {
            if (current_time - last_eat_time[i] <= deadlock_timeout) {
                all_waiting = 0;
                break;
            }
        }

        if (all_waiting) {
            printf("Deadlock detected! No philosopher has eaten in the last %d seconds.\n", deadlock_timeout);
            running = 0;

            // Cancel philosopher threads
            for (int i = 0; i < N; i++) {
                Pthread_cancel(threads[i]);
            }

            // No need to cancel self; will exit loop
            break;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: philosopher1 N [thinking_time] [eating_time]\n");
        exit(1);
    }

    N = atoi(argv[1]);
    if (N < 3 || N > 20) {
        printf("N must be between 3 and 20\n");
        exit(1);
    }

    // Parse optional thinking_time and eating_time from command line arguments
    if (argc >= 3)
        thinking_time = atof(argv[2]);
    if (argc >= 4)
        eating_time = atof(argv[3]);

    // Initialize data structures
    forks = malloc(N * sizeof(Zem_t));
    eat_count = malloc(N * sizeof(int));
    last_eat_time = malloc(N * sizeof(int));
    threads = malloc(N * sizeof(pthread_t)); // Allocate memory for threads

    // Initialize forks and counts
    for (int i = 0; i < N; i++) {
        Zem_init(&forks[i], 1);
        eat_count[i] = 0;
        last_eat_time[i] = time(NULL);
    }

    // Initialize mutex
    Mutex_init(&monitor_lock);

    // Seed the random number generator for testing
    srand(time(NULL));

    // Create philosopher threads
    for (int i = 0; i < N; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        Pthread_create(&threads[i], NULL, philosopher, id);
    }

    // Create deadlock detector thread
    Pthread_create(&detector_thread, NULL, deadlock_detector, NULL);

    // Wait for philosophers to finish
    for (int i = 0; i < N; i++) {
        Pthread_join(threads[i], NULL);
    }

    // Wait for deadlock detector to finish
    Pthread_join(detector_thread, NULL);

    // Print statistics
    printf("\nSimulation ended.\n");
    printf("Philosopher eating counts:\n");
    for (int i = 0; i < N; i++) {
        printf("Philosopher %d ate %d times\n", i, eat_count[i]);
    }

    // Clean up
    free(forks);
    free(eat_count);
    free(last_eat_time);
    free(threads);

    return 0;
}