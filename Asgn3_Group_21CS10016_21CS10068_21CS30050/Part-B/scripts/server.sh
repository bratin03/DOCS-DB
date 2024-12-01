#!/bin/bash

# Check if a command is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <command>"
    exit 1
fi

# Extract the command and arguments from input
command="$@"

# Run the command in the background
$command &
PID=$!

# Wait for the process to start
sleep 0.5

# Check if the PID is valid
if ! ps -p $PID >/dev/null; then
    echo "Error: Process with PID $PID not found!"
    exit 1
fi

echo "Running command: $command with PID: $PID"
echo "Logging CPU and memory usage for process PID: $PID"

# Initialize variables for calculating averages
total_cpu=0
total_mem=0
count=0

# Create a directory to store logs if it doesn't exist
mkdir -p "./log"

# Log file to store usage details
LOG_FILE="./log/process_usage.log"

echo "Time, CPU(%), Memory(RSS KB)" >"$LOG_FILE" # Header for the log file

# Monitor the process until it exits
while true; do
    if ! ps -p $PID >/dev/null; then
        echo "Process $PID has exited."
        break
    fi

    # Get CPU and memory usage using ps command
    cpu_mem_data=$(ps -p $PID -o %cpu,%mem,rss --no-headers)
    cpu=$(echo $cpu_mem_data | awk '{print $1}')
    mem=$(echo $cpu_mem_data | awk '{print $2}')
    rss=$(echo $cpu_mem_data | awk '{print $3}')

    # Log the data with timestamp
    timestamp=$(date +"%Y-%m-%d %H:%M:%S")
    echo "$timestamp, $cpu, $rss" >>"$LOG_FILE"

    # Accumulate the CPU and memory usage for averaging later
    total_cpu=$(echo "$total_cpu + $cpu" | bc)
    total_mem=$(echo "$total_mem + $rss" | bc)
    count=$((count + 1))

    # Wait for 0.1 seconds before the next check (increased frequency)
    sleep 0.02
done

# Calculate and display the averages
if [ $count -gt 0 ]; then
    avg_cpu=$(echo "$total_cpu / $count" | bc -l)
    avg_mem=$(echo "$total_mem / $count" | bc)
    echo "Average CPU Usage: $avg_cpu%"
    echo "Average Memory Usage (RSS): $avg_mem KB"
else
    echo "No data collected."
fi

# Remove the log file
rm -f "$LOG_FILE"
