#!/bin/bash

# Check if a PID is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <PID>"
    exit 1
fi

# Extract the PID from input
PID="$1"

# Check if the process exists
if ! ps -p $PID >/dev/null; then
    echo "Error: Process with PID $PID not found!"
    exit 1
fi

echo "Monitoring process with PID: $PID"
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

    # Wait for some time before checking again
    sleep 0.05
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
