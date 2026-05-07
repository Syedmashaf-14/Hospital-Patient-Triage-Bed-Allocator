Hospital Patient Triage and Bed Allocator System

A comprehensive Operating Systems simulation project built entirely in C for Linux/POSIX environments.
This project simulates a high-concurrency hospital triage and admission system to demonstrate practical implementation of core Operating System concepts such as:

Process Management
Inter-Process Communication (IPC)
CPU Scheduling
POSIX Threads
Synchronization
Dynamic Memory Allocation
Paging & Fragmentation
Virtual Memory Mapping

Instead of a traditional frontend application, this system acts as a low-level backend server that manages patient flow, hospital bed allocation, and resource synchronization using system-level Linux APIs.

Features
Multi-process patient admission simulation
FIFO-based IPC communication
Concurrent multithreaded hospital server
Priority-based patient scheduling
Producer-Consumer synchronization model
Bed allocation using memory management algorithms
Paging and fragmentation simulation
POSIX shared synchronization primitives
Memory-mapped logging system
Stress testing for concurrency validation
Project Architecture

The system was developed in 4 major phases, each targeting a different Operating Systems concept.

Phase 1 — Process Management & IPC

This phase establishes the foundational multi-process environment.

Implemented Concepts
Process Creation
Used fork() and execv() to spawn independent patient simulator processes dynamically.
Inter-Process Communication (IPC)
Communication handled through Linux Named Pipes (FIFOs):
/tmp/triage_pipe
/tmp/discharge_fifo
Asynchronous Execution
Bash scripts generate multiple background processes simultaneously.
Simulates real-world concurrent hospital admissions.
Key Files
patient_simulator.c
triage_server.c
stress_test.sh
Phase 2 — CPU Scheduling Simulation

This phase manages incoming patients when the hospital reaches full capacity.

Implemented Concepts
Waitlist Management
Dynamic linked-list based Priority Queue implemented for patient waiting lists.
Scheduling Algorithms

Implemented scheduling simulations for:

First-Come First-Served (FCFS)
Priority Scheduling
Performance Metrics

The simulator calculates:

Average Waiting Time
Average Turnaround Time
Execution Visualization
Generates Gantt-chart style logs for analysis.
Key File
scheduler_sim.c
Phase 3 — POSIX Threads & Synchronization

The architecture was upgraded into a highly concurrent multithreaded server.

Implemented Concepts
Thread Pool Architecture

Dedicated threads were created using pthreads:

Receptionist Thread (Producer)
Scheduler Thread (Consumer)
Nurse Threads
Mutex Synchronization
Shared hospital bed bitmap protected using pthread_mutex_t.
Semaphores
sem_t used to enforce ward capacity limits.

Example:

ICU ward maximum capacity = 4 beds
Condition Variables
pthread_cond_t used to eliminate busy waiting.
Threads sleep and wake immediately when beds become available.
Synchronization Problems Solved
Producer-Consumer Problem
Race Conditions
Resource Contention
Phase 4 — Dynamic Memory Management

The hospital ward is modeled as a contiguous block of memory.

Implemented Concepts
Contiguous Memory Allocation

Runtime-selectable allocation strategies:

First-Fit
Best-Fit
Worst-Fit
Coalescing & Defragmentation

Adjacent free memory blocks are automatically merged during discharge:

Left Coalescing
Right Coalescing
Paging Simulation
Fixed-size bed pages implemented.
Internal fragmentation calculated and reported.
Virtual Memory Logging
Admission/discharge logs written using:
mmap()
msync()

This bypasses standard file I/O overhead.

Key File
memory_sim.c
Technical Stack
Component	Technology
Language	C (C99/C11)
OS Environment	Linux / POSIX
Compiler	GCC
Build Tool	GNU Make
Threading	pthread
Synchronization	semaphore
Memory APIs	mmap, msync
IPC	FIFO / Named Pipes
Project Structure
.
├── Makefile
├── scheduler_sim.c
├── memory_sim.c
├── patient_simulator.c
├── triage_server.c
├── script/
│   ├── start_hospital.sh
│   └── stress_test.sh
├── logs/
└── README.md
Compilation

Navigate to the project root directory and compile using:

make clean
make
Running the Hospital Server (Phases 1–3)

Start the multithreaded hospital server:

./script/start_hospital.sh
Stress Testing

To simulate high-concurrency patient admissions:

./script/stress_test.sh

This script launches multiple patient simulator processes concurrently.

Running Memory Management Simulator (Phase 4)

Run the memory allocation simulation independently:

Best-Fit
./memory_sim --strategy best
First-Fit
./memory_sim --strategy first
Worst-Fit
./memory_sim --strategy worst
Sample Concepts Demonstrated
Operating Systems Topics Covered
Process Creation
Process Synchronization
CPU Scheduling
Producer-Consumer Problem
IPC Mechanisms
Mutex Locks
Semaphores
Condition Variables
Dynamic Memory Allocation
Paging
Fragmentation
Virtual Memory
Memory Mapping
Educational Purpose

This project was designed to provide practical hands-on implementation of advanced Operating Systems concepts typically taught theoretically in university courses.

It demonstrates how low-level Linux system programming can be used to build scalable concurrent systems.

Future Improvements

Possible future enhancements include:

Shared Memory IPC
Real-time Scheduling Algorithms
GUI Monitoring Dashboard
Network-based Patient Admission
Database Integration
Distributed Hospital Cluster Simulation
Deadlock Detection Module
Authors

Operating Systems Semester Project
Developed using Linux System Programming and POSIX APIs.
