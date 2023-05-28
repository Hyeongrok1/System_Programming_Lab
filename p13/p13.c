#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int count;
int points;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *montecarlo() {
    double x, y;
    unsigned int seed;
    /* your code here */
    for (int i = 0; i < points; i++) {
        x = (double)rand_r(&seed) / RAND_MAX;
        y = (double)rand_r(&seed) / RAND_MAX;

        if ((pow(x, 2.0) + pow(y, 2.0)) < 1) {
            pthread_mutex_lock(&m);
            count += 1;
            pthread_mutex_unlock(&m);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: ./p13 [num_threads] [points per thread]\n");
        exit(1);
    }
    int n = atoi(argv[1]);
    points = atoi(argv[2]);
    /* your code here */
    pthread_t thread[n];
    for (int i = 0; i < n; i++) {
        if (pthread_create(&thread[i], NULL, montecarlo, NULL)) {
            printf("pthread_create err: please start again\n");
            exit(1);
        }
    }
    
    for (int i = 0; i < n; i++) {
        pthread_join(thread[i], NULL);
    }
		
	double pi = (double)(4 * count) / (double)(n * points);
    printf("pi: %lf\n", pi);
    return 0;
}
