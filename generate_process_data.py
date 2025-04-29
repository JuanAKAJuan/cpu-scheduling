import random


def generate_process_data(file_name="datafile1.txt", num_processes=500):
    """
    Generates simulated process data and writes it to a file.

    Args:
        file_name (str): The name of the file to write the data to.
        num_processes (int): The total number of processes to generate.
    """

    processes = []
    current_time = 0
    min_burst = 5
    max_burst = 100
    min_priority = 1
    max_priority = 10
    max_arrival_increment = 5

    print(f"Generating data for {num_processes} processes...")

    for i in range(1, num_processes + 1):
        process_id = i

        arrival_time = current_time + random.randint(0, max_arrival_increment)
        current_time = arrival_time

        priority = random.randint(min_priority, max_priority)
        burst_time = random.randint(min_burst, max_burst)

        processes.append((process_id, arrival_time, priority, burst_time))

    print(f"Writing data to {file_name}...")

    try:
        with open(file_name, "w") as f:
            for process in processes:
                f.write(f"{process[0]} {process[1]} {process[2]} {process[3]}\n")
            print(f"Successfully generated {file_name}")
    except IOError as e:
        print(f"Error writing to file {file_name}: {e}")


if __name__ == "__main__":
    generate_process_data()
