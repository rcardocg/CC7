#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "genThreads.h"

int genThreads(int *bursts, int *arrival){
	int min = 5, max = 15;
	srand(time(NULL));
	int randomT = (rand() % (max - min + 1)) + min;
	printf("Threads generados: %d\n", randomT);
	printf("\n");

	for(int i = 0; i < randomT; i++){
		bursts[i] = (rand() % 10) + 1;  // burst entre 1 y 10
		arrival[i] = rand() % 101; //arrival entre 0 y 100 
		printf("Thread %d -> Burst: %d, Arrival: %d\n", i, bursts[i], arrival[i]);
    }
	printf("-----------------------------------------------------\n");
	return randomT;
}
