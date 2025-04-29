#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

const int NUM_PROCESSES = 500;

struct Process {
	int id;
	int arrivalTime;
	int priority;
	int burstTime;

	// --- Simulation Tracking ---
	int startTime = -1;
	int completionTime = -1;
	int waitingTime = 0;
	int turnaroundTime = 0;
	int responseTime = -1;

	bool operator<(const Process& other) const {
		if (arrivalTime != other.arrivalTime) {
			return arrivalTime < other.arrivalTime;
		}
		return id < other.id;
	}
};

bool readProcesses(const std::string& fileName, std::vector<Process>& processes);
void printStatistics(int completedCount, double totalElapsedTime, double totalWaitingTime, double totalTurnaroundTime,
					 double totalResponseTime);
void simulateFIFO(std::vector<Process>& processes);
void simulateSJF(std::vector<Process>& processes);

int main() {
	std::vector<Process> allProcesses;
	std::string fileName = "datafile1.txt";

	if (!readProcesses(fileName, allProcesses)) {
		return 1;
	}

	if (allProcesses.size() < NUM_PROCESSES) {
		std::cerr << "ERROR: Input file contains fewer than " << NUM_PROCESSES << " processes." << std::endl;
		return 1;
	}

	allProcesses.resize(NUM_PROCESSES);

	// Sort all processes by arrival time initially.
	std::sort(allProcesses.begin(), allProcesses.end());

	int choice;
	std::cout << "Select Scheduling Algorithm:" << std::endl;
	std::cout << "1. FIFO (First-In, First-Out)" << std::endl;
	std::cout << "2. SJF (Shortest Job First - Non-Preemptive)" << std::endl;
	std::cout << "Enter choice (1 or 2): ";
	std::cin >> choice;

	std::vector<Process> simulationProcesses = allProcesses;

	switch (choice) {
		case 1:
			std::cout << "\n--- Running FIFO Simulation ---" << std::endl;
			simulateFIFO(simulationProcesses);
			break;
		case 2:
			std::cout << "\n--- Running SJF (Non-Preemptive) Simulation ---" << std::endl;
			simulateSJF(simulationProcesses);
			break;
		default:
			std::cerr << "Invalid choice." << std::endl;
			return 1;
	}

	return 0;
}

bool readProcesses(const std::string& fileName, std::vector<Process>& processes) {
	std::ifstream inFile(fileName);
	if (!inFile) {
		std::cerr << "ERROR: Cannot open file: " << fileName << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(inFile, line)) {
		std::stringstream ss(line);
		Process p;
		if (ss >> p.id >> p.arrivalTime >> p.priority >> p.burstTime) {
			if (p.arrivalTime < 0 || p.burstTime <= 0) {
				std::cerr << "WARNING: Skipping invalid process data (ID: " << p.id << ", Arrival: " << p.arrivalTime
						  << ", Burst: " << p.burstTime << ")" << std::endl;
				continue;
			}

			processes.push_back(p);
		} else {
			std::cerr << "WARNING: Skipping malformed line: " << line << std::endl;
		}
	}

	inFile.close();
	std::cout << "Read " << processes.size() << " process from " << fileName << "." << std::endl;
	return true;
}

void printStatistics(int completedCount, double totalElapsedTime, double totalWaitingTime, double totalTurnaroundTime,
					 double totalResponseTime) {
	if (completedCount == 0) {
		std::cout << "\nNo processes completed." << std::endl;
		return;
	}

	double throughput = (totalElapsedTime > 0) ? (static_cast<double>(completedCount) / totalElapsedTime) : 0;
	double cpuUtilization = totalWaitingTime / completedCount;
	double avgWaitingTime = totalWaitingTime / completedCount;
	double avgTurnaroundTime = totalTurnaroundTime / completedCount;
	double avgResponseTime = totalResponseTime / completedCount;

	std::cout << "\n--- Simulation Statistics ---" << std::endl;
	std::cout << "Number of processes executed: " << completedCount << std::endl;
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "Total elapsed time: " << totalElapsedTime << " units" << std::endl;
	std::cout << std::fixed << std::setprecision(4);
	std::cout << "Throughput: " << throughput << " processes/unit time" << std::endl;
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "CPU utilization: " << cpuUtilization << "%" << std::endl;
	std::cout << "Average waiting time: " << avgWaitingTime << " units" << std::endl;
	std::cout << "Average turnaround time: " << avgTurnaroundTime << " units" << std::endl;
	std::cout << "Average response time: " << avgResponseTime << " units" << std::endl;
}

void simulateFIFO(std::vector<Process>& processes) {
	std::queue<int> readyQueue;
	int currentTime = 0;
	int completedCount = 0;
	size_t nextArrivalIndex = 0;
	int currentProcessIndex = -1;
	int cpuBusyUntil = 0;

	double totalBurstSum = 0;
	double totalWaitingTime = 0;
	double totalTurnaroundTime = 0;
	double totalResponseTime = 0;

	// Pre-calculate total burst time.
	for (const auto& p : processes) {
		totalBurstSum += p.burstTime;
	}

	while (completedCount < NUM_PROCESSES) {
		while (nextArrivalIndex < processes.size() && processes[nextArrivalIndex].arrivalTime <= currentTime) {
			readyQueue.push(nextArrivalIndex);
			nextArrivalIndex++;
		}
		if (currentProcessIndex != -1 && currentTime >= cpuBusyUntil) {
			// Process completion
			Process& p = processes[currentProcessIndex];
			p.completionTime = currentTime;
			p.turnaroundTime = p.completionTime - p.arrivalTime;

			totalTurnaroundTime += p.turnaroundTime;
			totalWaitingTime += p.waitingTime;
			totalResponseTime += p.responseTime;

			completedCount++;
			currentProcessIndex = -1;
		}

		if (currentProcessIndex == -1 && !readyQueue.empty()) {
			currentProcessIndex = readyQueue.front();
			readyQueue.pop();

			Process& p = processes[currentProcessIndex];

			if (p.startTime == -1) {
				p.startTime = currentTime;
				p.responseTime = p.startTime - p.arrivalTime;
			}
			p.waitingTime = currentTime - p.arrivalTime;

			cpuBusyUntil = currentTime + p.burstTime;
		}

		if (completedCount == NUM_PROCESSES) {
			break;
		}

		if (currentProcessIndex == -1 && readyQueue.empty()) {
			if (nextArrivalIndex < processes.size()) {
				if (processes[nextArrivalIndex].arrivalTime > currentTime) {
					currentTime = processes[nextArrivalIndex].arrivalTime;
				} else {
					currentTime++;
				}
			} else {
				break;
			}
		} else {
			int nextEventTime = std::numeric_limits<int>::max();
			if (currentProcessIndex != -1) {
				nextEventTime = std::min(nextEventTime, cpuBusyUntil);
			}
			if (nextArrivalIndex < processes.size()) {
				nextEventTime = std::min(nextEventTime, processes[nextArrivalIndex].arrivalTime);
			}

			if (nextEventTime > currentTime) {
				currentTime = nextEventTime;
			} else {
				currentTime++;
			}
		}
	}

	double totalElapsedTime = static_cast<double>(currentTime);

	printStatistics(completedCount, totalElapsedTime, totalWaitingTime, totalTurnaroundTime, totalResponseTime);
}

void simulateSJF(std::vector<Process>& processes) {
	std::vector<int> readyQueueIndices;

	int currentTime = 0;
	int completedCount = 0;
	size_t nextArrivalIndex = 0;
	int currentProcessIndex = -1;
	int cpuBusyUntil = 0;

	double totalBurstSum = 0;
	double totalWaitingTime = 0;
	double totalTurnaroundTime = 0;
	double totalResponseTime = 0;

	for (const auto& p : processes) {
		totalBurstSum += p.burstTime;
	}

	while (completedCount < NUM_PROCESSES) {
		while (nextArrivalIndex < processes.size() && processes[nextArrivalIndex].arrivalTime <= currentTime) {
			readyQueueIndices.push_back(nextArrivalIndex);
			nextArrivalIndex++;
		}

		if (currentProcessIndex != -1 && currentTime >= cpuBusyUntil) {
			Process& p = processes[currentProcessIndex];
			p.completionTime = currentTime;
			p.turnaroundTime = p.completionTime - p.arrivalTime;

			totalTurnaroundTime += p.turnaroundTime;
			totalWaitingTime += p.waitingTime;
			totalResponseTime += p.responseTime;

			completedCount++;
			currentProcessIndex = -1;
		}

		if (currentProcessIndex == -1 && !readyQueueIndices.empty()) {
			auto shortestJobIt =
				std::min_element(readyQueueIndices.begin(), readyQueueIndices.end(), [&](int idx1, int idx2) {
					if (processes[idx1].burstTime != processes[idx2].burstTime) {
						return processes[idx1].burstTime < processes[idx2].burstTime;
					}
					if (processes[idx1].arrivalTime != processes[idx2].arrivalTime) {
						return processes[idx1].arrivalTime < processes[idx2].arrivalTime;
					}
					return processes[idx1].id < processes[idx2].id;
				});

			currentProcessIndex = *shortestJobIt;
			readyQueueIndices.erase(shortestJobIt);

			Process& p = processes[currentProcessIndex];

			if (p.startTime == -1) {
				p.startTime = currentTime;
				p.responseTime = p.startTime - p.arrivalTime;
			}
			p.waitingTime = currentTime - p.arrivalTime;

			cpuBusyUntil = currentTime + p.burstTime;
		}

		if (completedCount == NUM_PROCESSES) {
			break;
		}

		if (currentProcessIndex == -1 && readyQueueIndices.empty()) {
			if (nextArrivalIndex < processes.size()) {
				if (processes[nextArrivalIndex].arrivalTime > currentTime) {
					currentTime = processes[nextArrivalIndex].arrivalTime;
				} else {
					currentTime++;
				}
			} else {
				break;
			}
		} else {
			int nextEventTime = std::numeric_limits<int>::max();
			if (currentProcessIndex != -1) {
				nextEventTime = std::min(nextEventTime, cpuBusyUntil);
			}
			if (nextArrivalIndex < processes.size()) {
				nextEventTime = std::min(nextEventTime, processes[nextArrivalIndex].arrivalTime);
			}

			if (nextEventTime > currentTime) {
				currentTime = nextEventTime;
			} else {
				currentTime++;
			}
		}
	}

	double totalElapsedTime = static_cast<double>(currentTime);

	printStatistics(completedCount, totalElapsedTime, totalWaitingTime, totalTurnaroundTime, totalResponseTime);
}
