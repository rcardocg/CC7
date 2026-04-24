#include <stdio.h>
#include <time.h>
#include "scheduling.h"

#define QUANTUM 2

void print_metrics(const char *algo_name, ProcessMetrics metrics[], int count) {
    printf("════════ %s ════════\n", algo_name);
    
    // Prepare timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[30];
    strftime(timestamp, sizeof(timestamp), "%a %b %d %H:%M:%S %Y", t);

    // Print detailed logs
    printf("\n--- Execution Timeline ---\n");
    for(int i = 0; i < count; i++) {
        printf("[%s] Process %d (Burst %d): Arrived\n", timestamp,
               metrics[i].process_id, metrics[i].burst_time);
        printf("[%s] Process %d (Burst %d): Started\n", timestamp,
               metrics[i].process_id, metrics[i].burst_time);
        printf("[%s] Process %d (Burst %d): Completed\n", timestamp,
               metrics[i].process_id, metrics[i].burst_time);
    }
    
    // Calculate and print metrics
    double total_waiting = 0, total_turnaround = 0;
    printf("\n--- Waiting Times ---\n");
    printf("Individual: ");
    for(int i = 0; i < count; i++) {
        printf("%.0f", (double)metrics[i].waiting_time);
        if(i < count - 1) printf(", ");
        total_waiting += metrics[i].waiting_time;
    }
    printf("\nAverage Waiting Time: %.2f\n", total_waiting / count);
    
    printf("\n--- Turnaround Times ---\n");
    printf("Individual: ");
    for(int i = 0; i < count; i++) {
        printf("%.0f", (double)metrics[i].turnaround_time);
        if(i < count - 1) printf(", ");
        total_turnaround += metrics[i].turnaround_time;
    }
    printf("\nAverage Turnaround Time: %.2f\n", total_turnaround / count);
    printf("\n═════════════════════════════════════════════════════\n");
}

//------------------------------FIFO--------------------------------

void fifo(int randomT, int *burst, int *arrival){
    ProcessMetrics metrics[randomT];
    
    // Sort by arrival time
    int order[randomT], visited[randomT];
    for(int i = 0; i < randomT; i++) visited[i] = 0;

    for(int i = 0; i < randomT; i++){
        int minArr = -1;
        for(int j = 0; j < randomT; j++){
            if(!visited[j] && (minArr == -1 || arrival[j] < arrival[minArr]))
                minArr = j;
        }
        order[i] = minArr;
        visited[minArr] = 1;
    }

    int clock = 0;
    for(int i = 0; i < randomT; i++){
        int idx = order[i];
        if(clock < arrival[idx]) clock = arrival[idx];
        
        metrics[idx].process_id = idx;
        metrics[idx].arrival_time = arrival[idx];
        metrics[idx].burst_time = burst[idx];
        metrics[idx].start_time = clock;
        metrics[idx].end_time = clock + burst[idx];
        metrics[idx].waiting_time = clock - arrival[idx];
        metrics[idx].turnaround_time = metrics[idx].end_time - arrival[idx];
        
        clock += burst[idx];
    }

    print_metrics("FIFO SCHEDULING", metrics, randomT);
}

//------------------------------RR--------------------------------

void roundRobin(int randomT, int *burst, int *arrival){
    ProcessMetrics metrics[randomT];
    for(int i = 0; i < randomT; i++) {
        metrics[i].process_id = i;
        metrics[i].arrival_time = arrival[i];
        metrics[i].burst_time = burst[i];
        metrics[i].start_time = -1;
    }
    
    int remaining[randomT];
    for(int i = 0; i < randomT; i++) remaining[i] = burst[i];

    int clock = 0, done = 0;
    
    while(done < randomT){
        for(int i = 0; i < randomT; i++){
            if(remaining[i] > 0 && arrival[i] <= clock){
                if(metrics[i].start_time == -1) {
                    metrics[i].start_time = clock;
                }
                
                int slice = remaining[i] > QUANTUM ? QUANTUM : remaining[i];
                remaining[i] -= slice;
                clock += slice;
                
                if(remaining[i] == 0){
                    metrics[i].end_time = clock;
                    metrics[i].waiting_time = metrics[i].start_time - arrival[i];
                    metrics[i].turnaround_time = metrics[i].end_time - arrival[i];
                    done++;
                }
            }
        }
        if(done == randomT) break;
        
        // Idle time if no process ready
        int found = 0;
        for(int i = 0; i < randomT; i++) {
            if(remaining[i] > 0 && arrival[i] <= clock) {
                found = 1;
                break;
            }
        }
        if(!found) {
            for(int i = 0; i < randomT; i++) {
                if(remaining[i] > 0) {
                    clock = arrival[i];
                    break;
                }
            }
        }
    }

    print_metrics("ROUND ROBIN SCHEDULING (Quantum=2)", metrics, randomT);
}

//------------------------------SJF--------------------------------

void sjf(int randomT, int *burst, int *arrival){
    ProcessMetrics metrics[randomT];
    for(int i = 0; i < randomT; i++) {
        metrics[i].process_id = i;
        metrics[i].arrival_time = arrival[i];
        metrics[i].burst_time = burst[i];
    }
    
    int visited[randomT];
    for(int i = 0; i < randomT; i++) visited[i] = 0;

    int clock = 0, done = 0;

    while(done < randomT){
        int sel = -1;
        for(int i = 0; i < randomT; i++){
            if(!visited[i] && arrival[i] <= clock){
                if(sel == -1 || burst[i] < burst[sel])
                    sel = i;
            }
        }
        if(sel == -1){ 
            // Find next arrival
            for(int i = 0; i < randomT; i++) {
                if(!visited[i]) {
                    clock = arrival[i];
                    break;
                }
            }
            continue; 
        }

        metrics[sel].start_time = clock;
        metrics[sel].end_time = clock + burst[sel];
        metrics[sel].waiting_time = clock - arrival[sel];
        metrics[sel].turnaround_time = metrics[sel].end_time - arrival[sel];
        
        clock += burst[sel];
        visited[sel] = 1;
        done++;
    }

    print_metrics("SJF SCHEDULING", metrics, randomT);
}

//------------------------------SRTF--------------------------------
void srtf(int randomT, int *burst, int *arrival){
    ProcessMetrics metrics[randomT];
    for(int i = 0; i < randomT; i++) {
        metrics[i].process_id = i;
        metrics[i].arrival_time = arrival[i];
        metrics[i].burst_time = burst[i];
        metrics[i].start_time = -1;
    }
    
    int remaining[randomT], done_flags[randomT];
    for(int i = 0; i < randomT; i++){
        remaining[i]  = burst[i];
        done_flags[i] = 0;
    }

    int clock = 0, done = 0;

    while(done < randomT){
        int sel = -1;
        for(int i = 0; i < randomT; i++){
            if(!done_flags[i] && arrival[i] <= clock){
                if(sel == -1 || remaining[i] < remaining[sel])
                    sel = i;
            }
        }
        if(sel == -1){ 
            // Find next arrival
            for(int i = 0; i < randomT; i++) {
                if(!done_flags[i]) {
                    clock = arrival[i];
                    break;
                }
            }
            continue; 
        }

        if(metrics[sel].start_time == -1) {
            metrics[sel].start_time = clock;
        }

        remaining[sel]--;
        clock++;

        if(remaining[sel] == 0){
            done_flags[sel] = 1;
            metrics[sel].end_time = clock;
            metrics[sel].waiting_time = metrics[sel].start_time - arrival[sel];
            metrics[sel].turnaround_time = metrics[sel].end_time - arrival[sel];
            done++;
        }
    }

    print_metrics("SRTF SCHEDULING", metrics, randomT);
}