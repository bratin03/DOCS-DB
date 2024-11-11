#!/bin/bash

# Create namespaces
echo "Creating namespaces NS1, NS2, and NS3..."
ip netns add NS1
ip netns add NS2
ip netns add NS3

# Create veth pairs
echo "Creating veth pairs for NS1, NS2, and NS3..."
ip link add veth1 type veth peer name veth1-br
ip link add veth2 type veth peer name veth2-br
ip link add veth3 type veth peer name veth3-br

# Assign veth pairs to namespaces
echo "Assigning veth interfaces to namespaces..."
ip link set veth1 netns NS1
ip link set veth1-br netns NS2
ip link set veth2 netns NS2
ip link set veth2-br netns NS3

# Assign IP addresses to interfaces
echo "Assigning IP addresses to interfaces..."
ip netns exec NS1 ip addr add 10.0.0.1/24 dev veth1
ip netns exec NS2 ip addr add 10.0.0.2/24 dev veth1-br
ip netns exec NS2 ip addr add 10.0.0.3/24 dev veth2
ip netns exec NS3 ip addr add 10.0.0.4/24 dev veth2-br

# Bring interfaces up
echo "Bringing up interfaces..."
ip netns exec NS1 ip link set veth1 up
ip netns exec NS2 ip link set veth1-br up
ip netns exec NS2 ip link set veth2 up
ip netns exec NS3 ip link set veth2-br up
ip netns exec NS1 ip link set lo up
ip netns exec NS2 ip link set lo up
ip netns exec NS3 ip link set lo up

# Set up bridge in NS2
echo "Setting up bridge in NS2..."
ip netns exec NS2 brctl addbr br0
ip netns exec NS2 brctl addif br0 veth1-br
ip netns exec NS2 brctl addif br0 veth2
ip netns exec NS2 ip link set br0 up

# Add a default route in NS1 and NS3 to allow communication via NS2
echo "Setting up default routes..."
ip netns exec NS1 ip route add default via 10.0.0.2
ip netns exec NS3 ip route add default via 10.0.0.3

# Enable IP forwarding in NS2 to allow routing between NS1 and NS3
echo "Enabling IP forwarding in NS2..."
ip netns exec NS2 sysctl -w net.ipv4.ip_forward=1

# Introduce delay and packet loss on NS3
echo "Introducing delay (100ms) and packet loss (5%) on NS3..."
ip netns exec NS3 tc qdisc add dev veth2-br root netem delay 100ms loss 5%

echo "Network setup complete!"
