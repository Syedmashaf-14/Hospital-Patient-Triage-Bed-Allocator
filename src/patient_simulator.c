#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "bed_allocator.h"

int main(int argc, char* argv[]) {
    // Basic argument check
    if (argc < 2) {
        exit(1);
    }

    int patient_id = atoi(argv[1]);
    
    // Simulate treatment duration (random for now)
    int sleep_time = rand() % 10 + 5; 

    printf("Patient %d: Entering Treatment...\n", patient_id);
    sleep(sleep_time);
    printf("Patient %d: Treatment Complete. Discharging...\n", patient_id);

    // Notify admissions via FIFO
    int fd = open(FIFO_NAME, O_WRONLY);
    if (fd != -1) {
        write(fd, &patient_id, sizeof(int));
        close(fd);
    }

    return 0;
}
