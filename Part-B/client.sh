#!/bin/bash

# Check if the input number of clients and packets is provided
if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Usage: $0 <number_of_clients> <number_of_packets>"
    exit 1
fi

# Number of clients to run and number of packets per client
num_clients=$1
num_packets=$2

# Directory to store logs
log_dir="./log"

# Create the log directory if it doesn't exist
mkdir -p "$log_dir"

# Run clients in parallel, passing the number of packets to each client
for (( i=1; i<=num_clients; i++ )); do
    # Run each client in the background
    ./client $i $num_packets &
done

# Wait for all background clients to finish
wait

# Initialize variables to store total bandwidth and latency
total_bandwidth=0
total_latency=0

# Read the log files and calculate the average bandwidth and latency
for (( i=1; i<=num_clients; i++ )); do
    log_file="$log_dir/metrics_$i.log"
    
    if [ -f "$log_file" ]; then
        # Read the values from the log file
        bandwidth=$(head -n 1 "$log_file")
        latency=$(head -n 2 "$log_file" | tail -n 1)

        # Check if bandwidth and latency are numeric
        if [[ "$bandwidth" =~ ^[0-9]+([.][0-9]+)?$ ]] && [[ "$latency" =~ ^[0-9]+([.][0-9]+)?$ ]]; then
            # Add to totals
            total_bandwidth=$(echo "$total_bandwidth + $bandwidth" | bc)
            total_latency=$(echo "$total_latency + $latency" | bc)
        else
            echo "Error: Invalid values in log file $log_file"
            exit 1
        fi
    else
        echo "Log file for client $i not found!"
        exit 1
    fi
done

# Calculate the averages
avg_bandwidth=$(echo "$total_bandwidth / $num_clients" | bc -l)
avg_latency=$(echo "$total_latency / $num_clients" | bc -l)

# Display the results
echo "Average Bandwidth: $avg_bandwidth bytes/s"
echo "Average Latency: $avg_latency microseconds"

# Remove the log files
for (( i=1; i<=num_clients; i++ )); do
    log_file="$log_dir/metrics_$i.log"
    rm -f "$log_file"
done