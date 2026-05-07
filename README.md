Aapka project kaafi professional aur technical hai. GitHub README ke liye maine icons/emojis ko remove kar diya hai aur formatting ko mazeed clean aur structured banaya hai taake focus sirf content aur technology par rahe.

Aap isse copy karke apni `README.md` mein use kar sakte hain:

---

# Hospital Patient Triage and Bed Allocator System

A comprehensive **Operating Systems simulation project** built entirely in **C for Linux/POSIX environments**.  
This project simulates a high-concurrency hospital triage and admission system to demonstrate practical implementation of core Operating System concepts.

## Table of Contents
- [Project Overview](#project-overview)
- [Core Operating System Concepts](#core-operating-system-concepts)
- [System Architecture](#system-architecture)
- [Development Phases](#development-phases)
- [Technical Stack](#technical-stack)
- [Project Structure](#project-structure)
- [Compilation and Execution](#compilation-and-execution)
- [Future Improvements](#future-improvements)

---

## Project Overview
This system acts as a low-level backend server that manages patient admissions, bed allocation, and resource synchronization. It is designed to handle high-concurrency scenarios using system-level Linux APIs.

---

## Core Operating System Concepts
The implementation covers the following technical domains:
*   Process Creation & Management
*   Inter-Process Communication (IPC) via FIFOs
*   CPU Scheduling (FCFS, Priority)
*   Multithreading (POSIX Threads)
*   Synchronization (Mutex, Semaphores, Condition Variables)
*   Dynamic Memory Management (First-Fit, Best-Fit, Worst-Fit)
*   Paging and Internal Fragmentation
*   Virtual Memory Mapping (`mmap`)

---

## System Architecture

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

## Development Phases

### Phase 1: Process Management & IPC
Builds a multi-process environment where independent patient simulators are spawned using `fork()` and `execv()`. Communication is handled through Linux Named Pipes (FIFOs) located at `/tmp/triage_pipe`.

### Phase 2: CPU Scheduling Simulation
Handles patient overflow via a Linked-list based Priority Queue.
*   **Algorithms:** FCFS and Priority Scheduling.
*   **Metrics:** Calculates Average Waiting Time and Turnaround Time.
*   **Logs:** Generates Gantt-chart style execution logs.

### Phase 3: POSIX Threads & Synchronization
Transforms the architecture into a multithreaded server utilizing a Thread Pool (Receptionist, Scheduler, and Nurse threads).
*   **Synchronization:** Uses `pthread_mutex_t` for resource protection and `sem_t` for capacity limits.
*   **Efficiency:** Employs `pthread_cond_t` to solve the Producer-Consumer problem without busy-waiting.

### Phase 4: Dynamic Memory Management
Models hospital wards as contiguous memory blocks.
*   **Allocation:** Supports First-Fit, Best-Fit, and Worst-Fit strategies.
*   **Optimization:** Implements automatic Left/Right Coalescing and Defragmentation.
*   **I/O:** Uses `mmap()` and `msync()` for virtual memory logging to reduce standard I/O overhead.

---

## Technical Stack

| Component | Technology |
| :--- | :--- |
| Language | C (C99/C11) |
| Environment | Linux / POSIX |
| Compiler | GCC |
| Build Tool | GNU Make |
| Threading | pthreads |
| Synchronization | Semaphores, Mutex, Condition Variables |
| IPC | Named Pipes (FIFOs) |
| Memory | mmap, msync |

---

## Project Structure
```text
.
├── Makefile                   # Build configuration
├── patient_simulator.c       # Client simulation logic
├── triage_server.c           # Main server implementation
├── scheduler_sim.c           # Scheduling algorithms
├── memory_sim.c              # Memory management simulation
├── script/
│   ├── start_hospital.sh     # Server startup script
│   └── stress_test.sh        # Concurrency testing script
├── logs/                      # System logs
└── README.md
```

---

## Compilation and Execution

### Build
```bash
make clean
make
```

### Running the Server
```bash
./script/start_hospital.sh
```

### Running Stress Tests
```bash
./script/stress_test.sh
```

### Memory Simulation (Standalone)
```bash
./memory_sim --strategy [best | first | worst]
```

---

## Future Improvements
*   Implementation of Shared Memory IPC.
*   Real-time Scheduling Algorithms.
*   Network-based Patient Admission (Socket Programming).
*   Deadlock Detection Module.
*   Distributed Hospital Cluster Simulation.

---

## Authors
**Operating Systems Semester Project**  
Developed using Linux System Programming and POSIX APIs.

## License
This project is intended for educational and academic use only.
