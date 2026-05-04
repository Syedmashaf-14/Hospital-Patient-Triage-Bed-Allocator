/* 
 * ============================================================
 * Spring 2026
 * CL2006 Operating Systems Lab FAST-NUCES, CFD Campus 
 * ============================================================
 * Project : Hospital Patient Triage & Bed Allocator 
 * File    : admissions.c 
 * Group   : Group XX 
 * Members : Mashaf Zaidi (23F-0529)
 * Date    : 2026-05-05
 * Purpose : Central admissions manager - process spawning, 
 *           IPC, thread pool, scheduling, and bed allocation. 
 * Compile : gcc -Wall -pthread -o admissions src/admissions.c 
 * ============================================================ 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_BEDS 10
#define SHM_KEY 1234
#define MAX_QUEUE 50

// Bed Partitions (0-indexed)
#define ICU_START 0
#define ICU_END 3     // 4 ICU Beds
#define ISO_START 4
#define ISO_END 7     // 4 Isolation Beds
#define GEN_START 8
#define GEN_END 9     // 2 General Beds

// ================= GLOBAL IPC & SYNC VARIABLES =================
volatile sig_atomic_t running = 1;
int shmid;
int *beds;

// Bed Bitmap Sync
pthread_mutex_t bed_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bed_cond = PTHREAD_COND_INITIALIZER;

// Priority Queue Data Structure
typedef struct PatientNode {
    int id;
    char name[50];
    int priority;
    struct PatientNode* next;
} PatientNode;

PatientNode* queue_head = NULL;

// Queue Sync (Producer-Consumer)
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t queue_slots; // Bounded semaphore: open slots
sem_t queue_items; // Bounded semaphore: items in queue

// Capacity Semaphores (Limits)
sem_t icu_sem;
sem_t iso_sem;
sem_t gen_sem;

// Nurse Communication (Discharge routing)
typedef struct {
    int pending_discharges[MAX_BEDS];
    int count;
    pthread_mutex_t mtx;
    pthread_cond_t cv;
    sem_t* capacity_sem;
} NursePartition;

NursePartition nurse_icu, nurse_iso, nurse_gen;

// ================= HELPER FUNCTIONS =================
void handle_sigterm(int sig) { (void)sig; running = 0; }
void sigchld_handler(int sig) { (void)sig; while (waitpid(-1, NULL, WNOHANG) > 0); }

void enqueue(int id, const char* name, int priority) {
    PatientNode* new_node = (PatientNode*)malloc(sizeof(PatientNode));
    new_node->id = id;
    strcpy(new_node->name, name);
    new_node->priority = priority;
    new_node->next = NULL;

    if (queue_head == NULL || queue_head->priority > priority) {
        new_node->next = queue_head;
        queue_head = new_node;
    } else {
        PatientNode* current = queue_head;
        while (current->next != NULL && current->next->priority <= priority) {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}

PatientNode* dequeue() {
    if (queue_head == NULL) return NULL;
    PatientNode* temp = queue_head;
    queue_head = queue_head->next;
    return temp;
}

// ================= THREAD 1: RECEPTIONIST (PRODUCER) =================
void* receptionist_thread(void* arg) {
    (void)arg ;//to avoid the warning
    FILE *triage_fp = fopen("/tmp/triage_pipe", "r+");
    if (!triage_fp) { perror("Triage FIFO error"); exit(1); }
    char buffer[256];

    while (running) {
        if (fgets(buffer, sizeof(buffer), triage_fp) != NULL) {
            char name[50];
            int age, severity, priority;
            sscanf(buffer, "%[^,],%d,%d,%d", name, &age, &severity, &priority);
            int patient_id = rand() % 1000;

            printf("[RECEPTIONIST] Received %s (Priority %d). Enqueuing...\n", name, priority);

            // Bounded Semaphore Producer Logic
            sem_wait(&queue_slots);
            pthread_mutex_lock(&queue_mutex);
            enqueue(patient_id, name, priority);
            pthread_mutex_unlock(&queue_mutex);
            sem_post(&queue_items); // Signal scheduler
        }
        usleep(50000);
    }
    fclose(triage_fp);
    return NULL;
}

// ================= THREAD 2: SCHEDULER (CONSUMER) =================
void* scheduler_thread(void* arg) {
    (void)arg;
    while (running) {
        // Wait for patient in queue
        sem_wait(&queue_items);
        
        pthread_mutex_lock(&queue_mutex);
        PatientNode* p = dequeue();
        pthread_mutex_unlock(&queue_mutex);
        sem_post(&queue_slots);

        if (!p) continue;

        // Determine Required Bed Partition
        int start_idx, end_idx;
        sem_t* required_sem;
        char partition_name[20];

        if (p->priority == 1) {
            start_idx = ICU_START; end_idx = ICU_END; required_sem = &icu_sem; strcpy(partition_name, "ICU");
        } else if (p->priority == 2) {
            start_idx = ISO_START; end_idx = ISO_END; required_sem = &iso_sem; strcpy(partition_name, "Isolation");
        } else {
            start_idx = GEN_START; end_idx = GEN_END; required_sem = &gen_sem; strcpy(partition_name, "General");
        }

        // 1. Wait on capacity semaphore (Blocks if ward is full)
        printf("[SCHEDULER] Patient %s waiting for %s bed...\n", p->name, partition_name);
        sem_wait(required_sem); 

        // 2. Lock bitmap and find Best-Fit bed
        pthread_mutex_lock(&bed_mutex);
        int assigned_bed = -1;
        while (assigned_bed == -1) {
            for (int i = start_idx; i <= end_idx; i++) {
                if (beds[i] == 0) {
                    assigned_bed = i;
                    beds[i] = 1;
                    break;
                }
            }
            // If condition logic safeguard (Wait on cond var if out of sync)
            if (assigned_bed == -1) {
                pthread_cond_wait(&bed_cond, &bed_mutex);
            }
        }
        pthread_mutex_unlock(&bed_mutex);

        // 3. Trigger fork + exec
        pid_t pid = fork();
        if (pid == 0) {
            char p_id[20], triage[20], bed[20];
            sprintf(p_id, "%d", p->id);
            sprintf(triage, "%d", p->priority);
            sprintf(bed, "%d", assigned_bed + 1); // 1-indexed for display
            char *args[] = {"./patient_simulator", p_id, triage, bed, NULL};
            execv("./patient_simulator", args);
            exit(1);
        } else {
            printf("[SCHEDULER] Admitted %s to %s Bed %d (PID=%d)\n", p->name, partition_name, assigned_bed + 1, pid);
        }
        free(p);
    }
    return NULL;
}

// ================= THREAD 3: NURSE (BED MONITORS) =================
void* nurse_thread(void* arg) {
    (void)arg;
    NursePartition* partition = (NursePartition*)arg;

    while (running) {
        pthread_mutex_lock(&partition->mtx);
        while (partition->count == 0 && running) {
            pthread_cond_wait(&partition->cv, &partition->mtx);
        }
        if (!running) { pthread_mutex_unlock(&partition->mtx); break; }

        // Pop the discharged bed ID
        int b_id = partition->pending_discharges[--partition->count];
        pthread_mutex_unlock(&partition->mtx);

        // Free the bed in shared memory
        pthread_mutex_lock(&bed_mutex);
        beds[b_id] = 0; // Attempt coalescing / free bed
        printf("[NURSE] Freed Bed %d. Signaling scheduler.\n", b_id + 1);
        
        // Signal capacity and alert scheduler
        sem_post(partition->capacity_sem);
        pthread_cond_broadcast(&bed_cond);
        pthread_mutex_unlock(&bed_mutex);
    }
    return NULL;
}

// ================= DISCHARGE ROUTER (Reads FIFO, alerts Nurses) =================
void* discharge_router_thread(void* arg) {
    (void)arg;
    int discharge_fd = open("/tmp/discharge_fifo", O_RDONLY | O_NONBLOCK);
    if (discharge_fd < 0) return NULL;
    char buf[100];

    while (running) {
        int n = read(discharge_fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            int p_id, b_id;
            if (sscanf(buf, "%d,%d", &p_id, &b_id) == 2) {
                int array_idx = b_id - 1;
                NursePartition* target;

                if (array_idx >= ICU_START && array_idx <= ICU_END) target = &nurse_icu;
                else if (array_idx >= ISO_START && array_idx <= ISO_END) target = &nurse_iso;
                else target = &nurse_gen;

                // Send to appropriate nurse
                pthread_mutex_lock(&target->mtx);
                target->pending_discharges[target->count++] = array_idx;
                pthread_cond_signal(&target->cv);
                pthread_mutex_unlock(&target->mtx);
            }
        }
        usleep(100000);
    }
    close(discharge_fd);
    return NULL;
}

// ================= MAIN =================
void init_nurse(NursePartition* n, sem_t* cap_sem) {
    n->count = 0;
    pthread_mutex_init(&n->mtx, NULL);
    pthread_cond_init(&n->cv, NULL);
    n->capacity_sem = cap_sem;
}

int main() {
    signal(SIGTERM, handle_sigterm);
    signal(SIGCHLD, sigchld_handler);
    srand(time(NULL));

    // Init Shared Memory
    shmid = shmget(SHM_KEY, MAX_BEDS * sizeof(int), 0666 | IPC_CREAT);
    beds = (int *)shmat(shmid, NULL, 0);
    for (int i = 0; i < MAX_BEDS; i++) beds[i] = 0;

    // Init Semaphores
    sem_init(&queue_slots, 0, MAX_QUEUE);
    sem_init(&queue_items, 0, 0);
    sem_init(&icu_sem, 0, 4); // 4 ICU Beds Max
    sem_init(&iso_sem, 0, 4); // 4 Isolation Beds Max
    sem_init(&gen_sem, 0, 2); // 2 General Beds Max

    init_nurse(&nurse_icu, &icu_sem);
    init_nurse(&nurse_iso, &iso_sem);
    init_nurse(&nurse_gen, &gen_sem);

    printf("Multithreaded Admissions Server Online.\n");

    // Spawn Thread Pool
    pthread_t t_reception, t_scheduler, t_router;
    pthread_t t_nurse_icu, t_nurse_iso, t_nurse_gen;

    pthread_create(&t_reception, NULL, receptionist_thread, NULL);
    pthread_create(&t_scheduler, NULL, scheduler_thread, NULL);
    pthread_create(&t_router, NULL, discharge_router_thread, NULL);
    pthread_create(&t_nurse_icu, NULL, nurse_thread, &nurse_icu);
    pthread_create(&t_nurse_iso, NULL, nurse_thread, &nurse_iso);
    pthread_create(&t_nurse_gen, NULL, nurse_thread, &nurse_gen);

    // Wait for shutdown (SIGTERM)
    while (running) { pause(); }

    printf("\nShutting down IPC and joining threads...\n");
    
    // Cleanup IPC
    shmdt(beds);
    shmctl(shmid, IPC_RMID, NULL);
    sem_destroy(&queue_slots);
    sem_destroy(&queue_items);
    sem_destroy(&icu_sem);
    sem_destroy(&iso_sem);
    sem_destroy(&gen_sem);

    return 0;
}
