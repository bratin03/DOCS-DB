#!/bin/bash

# Variables
SERVER="./server"
BENCHMARK="redis-benchmark"
RESULTS_DIR="result"

# Ensure the results directory exists
mkdir -p $RESULTS_DIR

# Start the DOCS DB server in the background
echo "Starting DOCS DB server..."
$SERVER &
SERVER_PID=$!

# Wait for server to start
sleep 2

# Array of request counts
REQUESTS=(10000 100000 1000000)

# Array of parallel connections
CONNECTIONS=(10 100 1000)

# Function to run a benchmark test and save output
run_benchmark() {
    local requests=$1
    local connections=$2
    local with_dpdk=$3

    # Run the redis-benchmark for both SET and GET commands
    echo "Running benchmark: Requests=$requests, Connections=$connections, DPDK=$with_dpdk"
    $BENCHMARK -h 127.0.0.1 -p 6379 -n $requests -c $connections -t set,get > "$RESULTS_DIR/result_${requests}_${connections}_${with_dpdk}.txt"
}

# Run benchmarks with and without DPDK
for dpdk_enabled in 0 1; do
    if [[ "$dpdk_enabled" -eq 1 ]]; then
        echo "Running benchmarks with DPDK enabled"
        # Commands to start the server with DPDK (modify as needed if you have a DPDK setup script)
        # Example: sudo ./server --dpdk
    else
        echo "Running benchmarks without DPDK"
        # Stop and restart server without DPDK
        kill $SERVER_PID
        $SERVER &
        SERVER_PID=$!
        sleep 2
    fi

    # Run the benchmarks for each configuration of requests and connections
    for req_count in "${REQUESTS[@]}"; do
        for conn_count in "${CONNECTIONS[@]}"; do
            run_benchmark $req_count $conn_count $dpdk_enabled
        done
    done
done

# Kill the server after benchmarking
kill $SERVER_PID

echo "Benchmarking complete. Results saved in $RESULTS_DIR."
