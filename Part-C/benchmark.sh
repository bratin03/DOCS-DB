#!/bin/bash

# Ensure the script has 3 arguments
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <num_clients> <num_requests> <request_type>"
    exit 1
fi

# Input parameters
num_clients=$1
num_requests=$2
request_type=$3

# Validate the request type
if [[ "$request_type" != "GET" && "$request_type" != "SET" ]]; then
    echo "Invalid request type. Please specify 'GET' or 'SET'."
    exit 1
fi

# Run the redis-benchmark command with the specified parameters
redis-benchmark -h 127.0.0.1 -p 6379 -c "$num_clients" -n "$num_requests" -t "$request_type"
