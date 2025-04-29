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
	int arrival_time;
	int priority;
	int burst_time;

	// --- Simulation Tracking ---
	int start_time = -1;
	int completion_time = -1;
	int waiting_time = 0;
	int turnaround_time = 0;
	int response_time = -1;

	bool operator<(const Process& other) const {
		if (arrival_time != other.arrival_time) {
			return arrival_time < other.arrival_time;
		}
		return id < other.id;
	}
};

bool read_processes(const std::string& file_name, std::vector<Process>& processes);
void print_statistics(int completed_count, double total_elapsed_time, double total_waiting_time,
					  double total_turnaround_time, double total_response_time);
void simulate_fifo(std::vector<Process>& processes);
void simulate_sjf(std::vector<Process>& processes);

int main() {
	std::vector<Process> all_processes;
	std::string file_name = "datafile1.txt";

	if (!read_processes(file_name, all_processes)) {
		return 1;
	}

	if (all_processes.size() < NUM_PROCESSES) {
		std::cerr << "ERROR: Input file contains fewer than " << NUM_PROCESSES << " processes." << std::endl;
		return 1;
	}

	all_processes.resize(NUM_PROCESSES);

	// Sort all processes by arrival time initially.
	std::sort(all_processes.begin(), all_processes.end());

	int choice;
	std::cout << "Select Scheduling Algorithm:" << std::endl;
	std::cout << "1. FIFO (First-In, First-Out)" << std::endl;
	std::cout << "2. SJF (Shortest Job First - Non-Preemptive)" << std::endl;
	std::cout << "Enter choice (1 or 2): ";
	std::cin >> choice;

	std::vector<Process> simulation_processes = all_processes;

	switch (choice) {
		case 1:
			std::cout << "\n--- Running FIFO Simulation ---" << std::endl;
			simulate_fifo(simulation_processes);
			break;
		case 2:
			std::cout << "\n--- Running SJF (Non-Preemptive) Simulation ---" << std::endl;
			simulate_sjf(simulation_processes);
			break;
		default:
			std::cerr << "Invalid choice." << std::endl;
			return 1;
	}

	return 0;
}

bool read_processes(const std::string& file_name, std::vector<Process>& processes) {
	std::ifstream in_file(file_name);
	if (!in_file) {
		std::cerr << "ERROR: Cannot open file: " << file_name << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(in_file, line)) {
		std::stringstream ss(line);
		Process p;
		if (ss >> p.id >> p.arrival_time >> p.priority >> p.burst_time) {
			if (p.arrival_time < 0 || p.burst_time <= 0) {
				std::cerr << "WARNING: Skipping invalid process data (ID: " << p.id << ", Arrival: " << p.arrival_time
						  << ", Burst: " << p.burst_time << ")" << std::endl;
				continue;
			}

			processes.push_back(p);
		} else {
			std::cerr << "WARNING: Skipping malformed line: " << line << std::endl;
		}
	}

	in_file.close();
	std::cout << "Read " << processes.size() << " process from " << file_name << "." << std::endl;
	return true;
}

void print_statistics(int completed_count, double total_elapsed_time, double total_waiting_time,
					  double total_turnaround_time, double total_response_time) {
	if (completed_count == 0) {
		std::cout << "\nNo processes completed." << std::endl;
		return;
	}

	double throughput = (total_elapsed_time > 0) ? (static_cast<double>(completed_count) / total_elapsed_time) : 0;
	double cpu_utilization = total_waiting_time / completed_count;
	double avg_waiting_time = total_waiting_time / completed_count;
	double avg_turnaround_time = total_turnaround_time / completed_count;
	double avg_response_time = total_response_time / completed_count;

	std::cout << "\n--- Simulation Statistics ---" << std::endl;
	std::cout << "Number of processes executed: " << completed_count << std::endl;
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "Total elapsed time: " << total_elapsed_time << " units" << std::endl;
	std::cout << std::fixed << std::setprecision(4);
	std::cout << "Throughput: " << throughput << " processes/unit time" << std::endl;
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "CPU utilization: " << cpu_utilization << "%" << std::endl;
	std::cout << "Average waiting time: " << avg_waiting_time << " units" << std::endl;
	std::cout << "Average turnaround time: " << avg_turnaround_time << " units" << std::endl;
	std::cout << "Average response time: " << avg_response_time << " units" << std::endl;
}

void simulate_fifo(std::vector<Process>& processes) {
	std::queue<int> ready_queue;
	int current_time = 0;
	int completed_count = 0;
	size_t next_arrival_index = 0;
	int current_process_index = -1;
	int cpu_busy_until = 0;

	double total_burst_sum = 0;
	double total_waiting_time = 0;
	double total_turnaround_time = 0;
	double total_response_time = 0;

	// Pre-calculate total burst time.
	for (const auto& p : processes) {
		total_burst_sum += p.burst_time;
	}

	while (completed_count < NUM_PROCESSES) {
		while (next_arrival_index < processes.size() && processes[next_arrival_index].arrival_time <= current_time) {
			ready_queue.push(next_arrival_index);
			next_arrival_index++;
		}
		if (current_process_index != -1 && current_time >= cpu_busy_until) {
			// Process completion
			Process& p = processes[current_process_index];
			p.completion_time = current_time;
			p.turnaround_time = p.completion_time - p.arrival_time;

			total_turnaround_time += p.turnaround_time;
			total_waiting_time += p.waiting_time;
			total_response_time += p.response_time;

			completed_count++;
			current_process_index = -1;
		}

		if (current_process_index == -1 && !ready_queue.empty()) {
			current_process_index = ready_queue.front();
			ready_queue.pop();

			Process& p = processes[current_process_index];

			if (p.start_time == -1) {
				p.start_time = current_time;
				p.response_time = p.start_time - p.arrival_time;
			}
			p.waiting_time = current_time - p.arrival_time;

			cpu_busy_until = current_time + p.burst_time;
		}

		if (completed_count == NUM_PROCESSES) {
			break;
		}

		if (current_process_index == -1 && ready_queue.empty()) {
			if (next_arrival_index < processes.size()) {
				if (processes[next_arrival_index].arrival_time > current_time) {
					current_time = processes[next_arrival_index].arrival_time;
				} else {
					current_time++;
				}
			} else {
				break;
			}
		} else {
			int next_event_time = std::numeric_limits<int>::max();
			if (current_process_index != -1) {
				next_event_time = std::min(next_event_time, cpu_busy_until);
			}
			if (next_arrival_index < processes.size()) {
				next_event_time = std::min(next_event_time, processes[next_arrival_index].arrival_time);
			}

			if (next_event_time > current_time) {
				current_time = next_event_time;
			} else {
				current_time++;
			}
		}
	}

	double total_elapsed_time = static_cast<double>(current_time);

	print_statistics(completed_count, total_elapsed_time, total_waiting_time, total_turnaround_time,
					 total_response_time);
}

void simulate_sjf(std::vector<Process>& processes) {
	std::vector<int> ready_queue_indices;

	int current_time = 0;
	int completed_count = 0;
	size_t next_arrival_index = 0;
	int current_process_index = -1;
	int cpu_busy_until = 0;

	double total_burst_sum = 0;
	double total_waiting_time = 0;
	double total_turnaround_time = 0;
	double total_response_time = 0;

	for (const auto& p : processes) {
		total_burst_sum += p.burst_time;
	}

	while (completed_count < NUM_PROCESSES) {
		while (next_arrival_index < processes.size() && processes[next_arrival_index].arrival_time <= current_time) {
			ready_queue_indices.push_back(next_arrival_index);
			next_arrival_index++;
		}

		if (current_process_index != -1 && current_time >= cpu_busy_until) {
			Process& p = processes[current_process_index];
			p.completion_time = current_time;
			p.turnaround_time = p.completion_time - p.arrival_time;

			total_turnaround_time += p.turnaround_time;
			total_waiting_time += p.waiting_time;
			total_response_time += p.response_time;

			completed_count++;
			current_process_index = -1;
		}

		if (current_process_index == -1 && !ready_queue_indices.empty()) {
			auto shortest_job_it =
				std::min_element(ready_queue_indices.begin(), ready_queue_indices.end(), [&](int idx1, int idx2) {
					if (processes[idx1].burst_time != processes[idx2].burst_time) {
						return processes[idx1].burst_time < processes[idx2].burst_time;
					}
					if (processes[idx1].arrival_time != processes[idx2].arrival_time) {
						return processes[idx1].arrival_time < processes[idx2].arrival_time;
					}
					return processes[idx1].id < processes[idx2].id;
				});

			current_process_index = *shortest_job_it;
			ready_queue_indices.erase(shortest_job_it);

			Process& p = processes[current_process_index];

			if (p.start_time == -1) {
				p.start_time = current_time;
				p.response_time = p.start_time - p.arrival_time;
			}
			p.waiting_time = current_time - p.arrival_time;

			cpu_busy_until = current_time + p.burst_time;
		}

		if (completed_count == NUM_PROCESSES) {
			break;
		}

		if (current_process_index == -1 && ready_queue_indices.empty()) {
			if (next_arrival_index < processes.size()) {
				if (processes[next_arrival_index].arrival_time > current_time) {
					current_time = processes[next_arrival_index].arrival_time;
				} else {
					current_time++;
				}
			} else {
				break;
			}
		} else {
			int next_event_time = std::numeric_limits<int>::max();
			if (current_process_index != -1) {
				next_event_time = std::min(next_event_time, cpu_busy_until);
			}
			if (next_arrival_index < processes.size()) {
				next_event_time = std::min(next_event_time, processes[next_arrival_index].arrival_time);
			}

			if (next_event_time > current_time) {
				current_time = next_event_time;
			} else {
				current_time++;
			}
		}
	}

	double total_elapsed_time = static_cast<double>(current_time);

	print_statistics(completed_count, total_elapsed_time, total_waiting_time, total_turnaround_time,
					 total_response_time);
}
