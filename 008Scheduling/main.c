#include "genThreads.h"
#include "scheduling.h"

int main(){
    int burst[15], arrival[15];
    int randomT = genThreads(burst, arrival);

    fifo(randomT, burst, arrival);
    roundRobin(randomT, burst, arrival);
    sjf(randomT, burst, arrival);
    srtf(randomT, burst, arrival);

    return 0;
}