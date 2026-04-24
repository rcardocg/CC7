#ifndef SCHEDULING_H
#define SCHEDULING_H

typedef struct {
    int process_id;
    int arrival_time;
    int burst_time;
    int start_time;
    int end_time;
    int waiting_time;
    int turnaround_time;
} ProcessMetrics;

void fifo(int randomT, int *burst, int *arrival);
void roundRobin(int randomT, int *burst, int *arrival);
void sjf(int randomT, int *burst, int *arrival);
void srtf(int randomT, int *burst, int *arrival);

#endif