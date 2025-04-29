# C++ CPU Scheduling Simulation

This project simulates two common CPU scheduling algorithms: First-In, First-Out (FIFO) and non-preemptive Shortest Job First (SJF).

## Description
The program simulates the arrival and execution of processes on a single CPU. It does not interact with the operating system's actual scheduler but rather models the behavior based on process arrival times and CPU burst lengths.

Key features:
* **Process Representation:** Each process is defined by an ID, arrival time, priority (read but not used in FIFO/SJF), and CPU burst time.
* **Input:** Reads process data from an external file (`datafile1.txt`). Assumes the file contains data for at least 500 processes.
* **Scheduling Algorithms Implemented:**
    * **FIFO:** Processes are executed strictly in the order they arrive in the ready queue.
    * **SJF (Non-Preemptive):** When the CPU becomes free, the process in the ready queue with the shortest CPU burst time is selected. Once a process starts, it runs to completion without interruption.
* **Simulation:** Tracks the passage of simulated time (in CPU burst units) and manages ready queues.
* **Statistics:** After simulating the execution of a fixed number (500) of processes, it calculates and prints key performance metrics:
    * Total elapsed simulation time
    * Throughput
    * CPU utilization
    * Average waiting time
    * Average turnaround time
    * Average response time

## Dependencies
* A C++ compiler supporting C++11 or later (e.g., g++, clang++)
* `make` (for using the provided Makefile)

## How to run the code
Compile the code using make,
```bash
make
```

then run the executable and choose either `1` for FIFO or `2` for SJF.
```bash
./scheduler
```
