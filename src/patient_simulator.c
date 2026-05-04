#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Invalid args\n");
        return 1;
    }

    int patient_id = atoi(argv[1]);
    int triage = atoi(argv[2]);
    int bed_id = atoi(argv[3]);

    printf("Patient %d arrived (Triage %d, Bed %d)\n",
           patient_id, triage, bed_id);

    srand(getpid());

    int sleep_time;

    // Treatment time based on priority
    if (triage == 1) {            // ICU
        sleep_time = rand() % 11 + 5;   // 5–15
    } else if (triage == 2) {     // General
        sleep_time = rand() % 7 + 2;    // 2–8
    } else {                      // Isolation
        sleep_time = rand() % 8 + 3;    // 3–10
    }

    printf("Patient %d treatment started\n", patient_id);

    sleep(sleep_time);

    printf("Patient %d discharged after %d sec\n",
           patient_id, sleep_time);

    // Notify admissions via FIFO (Updated to send PatientID,BedID)
    int fd = open("/tmp/discharge_fifo", O_WRONLY);
    if (fd != -1) {
        dprintf(fd, "%d,%d\n", patient_id, bed_id); 
        close(fd);
    }

    return 0;
}
