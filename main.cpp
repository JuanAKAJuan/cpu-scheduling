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
void printStatistics(int completedCount, double totalElapsedTime, double totalBurstTime, double totalWaitingTime,
					 double totalTurnaroundTime, double totalResponseTime);
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
