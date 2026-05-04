#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    int priority;
    int burst_time; // Treatment time
    int wait_time;
    int turnaround_time;
} Process;

// Helper to calculate and log metrics
void simulate(FILE* log, Process procs[], int n, const char* algo_name) {
    int total_wt = 0, total_tat = 0;
    int current_time = 0;

    fprintf(log, "===== %s SIMULATION =====\n", algo_name);
    fprintf(log, "Gantt Chart:\n[0] ");

    for (int i = 0; i < n; i++) {
        procs[i].wait_time = current_time;
        procs[i].turnaround_time = procs[i].wait_time + procs[i].burst_time;
        
        current_time += procs[i].burst_time;
        
        total_wt += procs[i].wait_time;
        total_tat += procs[i].turnaround_time;

        fprintf(log, "--- (P%d) --- [%d] ", procs[i].id, current_time);
    }

    fprintf(log, "\n\nMetrics:\n");
    for (int i = 0; i < n; i++) {
        fprintf(log, "Patient %d | Priority: %d | Burst: %d | Wait: %d | Turnaround: %d\n", 
                procs[i].id, procs[i].priority, procs[i].burst_time, procs[i].wait_time, procs[i].turnaround_time);
    }
    
    fprintf(log, "\nAverage Waiting Time: %.2f\n", (float)total_wt / n);
    fprintf(log, "Average Turnaround Time: %.2f\n\n", (float)total_tat / n);
}

int main() {
    FILE *log = fopen("logs/schedule_log.txt", "w");
    if (!log) return 1;

    // 5 Dummy Patients: ID, Priority, Burst Time
    Process fcfs_queue[] = {
        {1, 3, 5, 0, 0}, {2, 1, 8, 0, 0}, {3, 4, 2, 0, 0}, {4, 2, 6, 0, 0}, {5, 1, 4, 0, 0}
    };
    int n = sizeof(fcfs_queue) / sizeof(fcfs_queue[0]);

    // 1. Simulate FCFS (Executes in order of arrival/array index)
    simulate(log, fcfs_queue, n, "First-Come First-Served (FCFS)");

    // 2. Simulate Priority Scheduling (Sort array by Priority: 1 is highest)
    Process priority_queue[] = {
        {1, 3, 5, 0, 0}, {2, 1, 8, 0, 0}, {3, 4, 2, 0, 0}, {4, 2, 6, 0, 0}, {5, 1, 4, 0, 0}
    };

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (priority_queue[j].priority > priority_queue[j+1].priority) {
                Process temp = priority_queue[j];
                priority_queue[j] = priority_queue[j+1];
                priority_queue[j+1] = temp;
            }
        }
    }
    
    simulate(log, priority_queue, n, "Priority Scheduling");

    printf("Simulations completed! Result Available at schedule_log.txt for Gantt chart and metrics.\n");
    fclose(log);
    return 0;
}
