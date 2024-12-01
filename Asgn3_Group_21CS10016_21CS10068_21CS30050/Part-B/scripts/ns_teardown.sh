#!/bin/bash

# Delete traffic control settings in NS3
echo "Removing traffic control settings in NS3..."
ip netns exec NS3 tc qdisc del dev veth2-br root

# Bring down and delete bridge in NS2
echo "Deleting bridge in NS2..."
ip netns exec NS2 ip link set br0 down
ip netns exec NS2 brctl delbr br0

# Bring down interfaces in namespaces
echo "Bringing down interfaces in namespaces..."
ip netns exec NS1 ip link set veth1 down
ip netns exec NS2 ip link set veth1-br down
ip netns exec NS2 ip link set veth2 down
ip netns exec NS3 ip link set veth2-br down

# Delete veth pairs
echo "Deleting veth pairs..."
ip link delete veth1
ip link delete veth2

# Delete namespaces
echo "Deleting namespaces NS1, NS2, and NS3..."
ip netns delete NS1
ip netns delete NS2
ip netns delete NS3

echo "Cleanup complete!"
