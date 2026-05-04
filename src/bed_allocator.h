#ifndef BED_ALLOCATOR_H
#define BED_ALLOCATOR_H

#include <time.h>

#define SHM_KEY 0xBEDF00D // Shared memory key
#define FIFO_NAME "/tmp/discharge_fifo" // Named pipe

/* Patient record passed via IPC */
typedef struct {
    int patient_id;
    char name[64];
    int age;
    int severity;      /* 1-10 raw severity from triage */
    int priority;      /* 1-5 computed triage priority */
    int care_units;    /* memory units required[cite: 1] */
    time_t arrival_time;
} PatientRecord;

/* Single bed partition in the ward memory model[cite: 1] */
typedef struct {
    int partition_id;
    int start_unit;    /* index in ward array[cite: 1] */
    int size;          /* number of care units[cite: 1] */
    int is_free;       /* 1 = FREE, 0 = OCCUPIED[cite: 1] */
    int patient_id;    /* -1 if free[cite: 1] */
    char bed_type[16]; /* "ICU", "GENERAL", "ISOLATION"[cite: 1] */
} BedPartition;

#endif
