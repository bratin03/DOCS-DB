#!/bin/bash

# Function to check delay and packet loss configuration
check_netem_settings() {
    echo "Checking delay and packet loss settings on NS3..."
    tc_output=$(ip netns exec NS3 tc qdisc show dev veth2-br)
    
    if echo "$tc_output" | grep -q "delay 100ms"; then
        echo "Delay of 100ms is correctly configured on NS3."
    else
        echo "Error: Delay is not configured correctly on NS3."
    fi
    
    if echo "$tc_output" | grep -q "loss 5%"; then
        echo "Packet loss of 5% is correctly configured on NS3."
    else
        echo "Error: Packet loss is not configured correctly on NS3."
    fi
}

# Test ping from NS1 to NS3
echo "Testing connectivity from NS1 to NS3..."
if ip netns exec NS1 ping -c 5 10.0.0.4 > /dev/null; then
    echo "Ping from NS1 to NS3 successful."
else
    echo "Ping from NS1 to NS3 failed."
fi

# Test ping from NS3 to NS1
echo "Testing connectivity from NS3 to NS1..."
if ip netns exec NS3 ping -c 5 10.0.0.1 > /dev/null; then
    echo "Ping from NS3 to NS1 successful."
else
    echo "Ping from NS3 to NS1 failed."
fi

# Check delay and packet loss
check_netem_settings

# Test ping with statistics to observe delay and packet loss
echo "Testing delay and packet loss from NS1 to NS3 with detailed ping statistics..."
ping_output=$(ip netns exec NS1 ping -c 10 10.0.0.4)
echo "$ping_output"

# Extract packet loss percentage and round-trip time from the ping statistics
packet_loss=$(echo "$ping_output" | grep -oP '\d+(?=% packet loss)')
avg_rtt=$(echo "$ping_output" | grep -oP '(?<=rtt min/avg/max/mdev = ).*(?= ms)' | cut -d '/' -f 2)

echo "Observed packet loss: $packet_loss%"
echo "Observed average round-trip time: $avg_rtt ms"

# Adjusted tolerance ranges
if (( packet_loss >= 5 && packet_loss <= 15 )); then
    echo "Packet loss is within the acceptable range."
else
    echo "Warning: Packet loss is not within the expected range."
fi

if (( $(echo "$avg_rtt >= 80" | bc -l) && $(echo "$avg_rtt <= 120" | bc -l) )); then
    echo "Delay is within the acceptable range."
else
    echo "Warning: Delay is not within the expected range."
fi
