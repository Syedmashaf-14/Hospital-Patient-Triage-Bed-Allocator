````md
# Hospital Patient Triage and Bed Allocator System

A comprehensive Operating Systems simulation project built entirely in C for Linux/POSIX environments.

This project simulates a high-concurrency hospital triage and admission system to demonstrate practical implementation of core Operating System concepts such as:

- Process Management
- Inter-Process Communication (IPC)
- CPU Scheduling
- POSIX Threads
- Synchronization
- Dynamic Memory Management
- Paging & Fragmentation
- Virtual Memory Mapping

---

## Table of Contents

- [Project Overview](#project-overview)
- [Core Operating System Concepts](#core-operating-system-concepts)
- [System Architecture](#system-architecture)
- [Phase 1 — Process Management & IPC](#phase-1--process-management--ipc)
- [Phase 2 — CPU Scheduling Simulation](#phase-2--cpu-scheduling-simulation)
- [Phase 3 — POSIX Threads & Synchronization](#phase-3--posix-threads--synchronization)
- [Phase 4 — Dynamic Memory Management](#phase-4--dynamic-memory-management)
- [Technical Stack](#technical-stack)
- [Project Structure](#project-structure)
- [Compilation](#compilation)
- [Running the Hospital Server](#running-the-hospital-server)
- [Stress Testing](#stress-testing)
- [Memory Management Simulator](#memory-management-simulator)
- [Example Features Demonstrated](#example-features-demonstrated)
- [Educational Purpose](#educational-purpose)
- [Future Improvements](#future-improvements)
- [Authors](#authors)
- [License](#license)

---

# Project Overview

This project behaves like a low-level backend operating-system server that manages:

- Patient admissions
- Bed allocation
- Resource synchronization
- Waitlists
- Scheduling
- Memory allocation

The system was developed in 4 major phases, each focused on a different Operating Systems concept.

---

# Core Operating System Concepts

- Process Creation
- Inter-Process Communication (IPC)
- CPU Scheduling Algorithms
- Multithreading
- Mutex Locks
- Semaphores
- Condition Variables
- Dynamic Memory Allocation
- Paging & Fragmentation
- Virtual Memory Mapping

---

# System Architecture

```text
               +----------------------+
               |  Patient Simulators  |
               +----------+-----------+
                          |
                          | FIFO PIPE
                          v
              +-----------------------+
              |  Receptionist Thread  |
              +-----------+-----------+
                          |
                          v
              +-----------------------+
              |  Scheduler Thread     |
              +-----------+-----------+
                          |
          +---------------+---------------+
          |                               |
          v                               v
+-------------------+         +-------------------+
| ICU Nurse Thread  |         | Ward Nurse Thread |
+-------------------+         +-------------------+
```

---

# Phase 1 — Process Management & IPC

## Objectives

Build a multi-process hospital admission environment.

## Features

### Process Creation

Implemented using:

```c
fork()
execv()
```

### IPC Mechanism

Used Linux Named Pipes (FIFOs):

```bash
/tmp/triage_pipe
/tmp/discharge_fifo
```

### Concurrent Stress Testing

Multiple patient processes launched simultaneously using Bash scripting.

## Important Files

```bash
patient_simulator.c
triage_server.c
stress_test.sh
```

---

# Phase 2 — CPU Scheduling Simulation

## Objectives

Handle patient overflow when hospital beds are full.

## Features

### Priority Queue Waitlist

- Linked-list based implementation

### Scheduling Algorithms

Implemented:

- FCFS Scheduling
- Priority Scheduling

### Performance Metrics

Calculated:

- Average Waiting Time
- Average Turnaround Time

### Gantt Chart Style Logs

```text
| P1 | P2 | P3 | P4 |
0    2    5    8   10
```

## Important File

```bash
scheduler_sim.c
```

---

# Phase 3 — POSIX Threads & Synchronization

## Objectives

Transform the system into a highly concurrent multithreaded server.

## Features

### Thread Pool Architecture

Implemented:

- Receptionist Thread
- Scheduler Thread
- Nurse Threads

### Mutex Locks

Protected shared hospital bed resources using:

```c
pthread_mutex_t
```

### Semaphores

Used:

```c
sem_t
```

To enforce ward capacity limits.

Example:

- Maximum ICU beds = 4

### Condition Variables

Implemented:

```c
pthread_cond_t
```

To avoid busy waiting.

## Problems Solved

- Producer-Consumer Problem
- Race Conditions
- Resource Contention

---

# Phase 4 — Dynamic Memory Management

## Objectives

Model hospital beds as memory blocks.

## Features

### Contiguous Allocation Algorithms

Implemented:

- First-Fit
- Best-Fit
- Worst-Fit

Runtime Selection:

```bash
./memory_sim --strategy best
```

### Coalescing & Defragmentation

Automatically merges adjacent free blocks:

- Left Coalescing
- Right Coalescing

### Paging Simulation

Implemented:

- Fixed-size pages
- Internal Fragmentation calculation

### Virtual Memory Logging

Used:

```c
mmap()
msync()
```

To bypass standard file I/O overhead.

## Important File

```bash
memory_sim.c
```

---

# Technical Stack

| Technology | Usage |
|---|---|
| C (C99/C11) | Core Development |
| GCC | Compilation |
| GNU Make | Build Automation |
| pthread | Multithreading |
| semaphore | Synchronization |
| mmap | Virtual Memory |
| FIFO Pipes | IPC |
| Linux/POSIX | Environment |

---

# Project Structure

```bash
.
├── Makefile
├── patient_simulator.c
├── triage_server.c
├── scheduler_sim.c
├── memory_sim.c
├── script/
│   ├── start_hospital.sh
│   └── stress_test.sh
├── logs/
└── README.md
```

---

# Compilation

```bash
make clean
make
```

---

# Running the Hospital Server

```bash
./script/start_hospital.sh
```

---

# Stress Testing

```bash
./script/stress_test.sh
```

This simulates high-concurrency patient admissions.

---

# Memory Management Simulator

## Best-Fit

```bash
./memory_sim --strategy best
```

## First-Fit

```bash
./memory_sim --strategy first
```

## Worst-Fit

```bash
./memory_sim --strategy worst
```

---

# Example Features Demonstrated

| Category | Concepts |
|---|---|
| Process Management | fork(), execv() |
| IPC | FIFO Named Pipes |
| Scheduling | FCFS, Priority |
| Synchronization | Mutex, Semaphores |
| Concurrency | POSIX Threads |
| Memory | Paging, Fragmentation |
| Virtual Memory | mmap(), msync() |

---

# Educational Purpose

This project was developed to provide practical implementation of advanced Operating Systems concepts typically taught theoretically in university courses.

It demonstrates how Linux system programming can be used to build scalable concurrent systems.

---

# Future Improvements

- Shared Memory IPC
- GUI Monitoring Dashboard
- Real-time Scheduling
- Database Integration
- Network-based Admissions
- Deadlock Detection
- Distributed Hospital Cluster

---


