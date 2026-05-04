#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

volatile sig_atomic_t running = 1;
int total_patients = 0;

void handle_sigterm(int sig) {
    (void)sig;
    running = 0;
}

int main() {
    signal(SIGTERM, handle_sigterm);

    printf("Admissions Manager Online. Waiting for triage data...\n");

    FILE *fp = fopen("/tmp/triage_pipe", "r+");
    if (!fp) {
        perror("Error opening FIFO");
        exit(1);
    }

    char buffer[256];

    while (running) {
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            printf("[ADMISSION] Received: %s", buffer);
            total_patients++;
        }
    }

    printf("\nShutting down Admissions...\n");
    printf("Total patients served: %d\n", total_patients);

    fclose(fp);
    return 0;
}
