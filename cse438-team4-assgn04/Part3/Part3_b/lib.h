#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void sendToAll(int m, pthread_t t[], int sigMask[]);

