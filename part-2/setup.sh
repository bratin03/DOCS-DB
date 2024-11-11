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
ip netns exec NS3 ip link set vTo deal with the increasingly severe DDoS attacks the authorized DNS server of Tencent Cloud DNSPod switched from Gigabit Ethernet to 10-Gigabit at the end of 2012. We faced several options: one is to continue to use the original network stack in the Linux kernel, another is to use kernel bypass techniques. After several rounds of investigation; we finally chose to develop our next generation of DNS server based on DPDK. The reason is DPDK provides ultra-high performance and can be seamlessly extended to 40G, or even 100G NIC, in the future.

After several months of development and testing, DKDNS, high-performance DNS server based on DPDK officially released in October 2013. It's capable of achieving up to 11 million QPS with a single 10GE port and 18.2 million QPS with two 10GE ports. And then we developed a user-space TCP/IP stack called F-Stack that can process 0.6 million RPS with a single 10GE port.

With the fast growth of Tencent Cloud more and more of our services needed higher network access performance. Meanwhile, F-Stack was continuing to improve, being driven by our business growth, and, ultimately developed into a general network access framework. But our initial TCP/IP stack couldn't meet the needs of these services. Continuing to develop and maintain a complete high performance network stack would have been too expensive. After evaluating several plans; we finally determined to port FreeBSD's (13.0 stable) TCP/IP stack into F-Stack. Not only does this allow us to stop reinventing the wheel, we can take advantage of the the improvements the FreeBSD community will bring in the future. Thanks to libplebnet and libuinet this work became a lot easier.

With the rapid development of all kinds of applications, in order to help different APPs quick and easily use F-Stack, F-Stack has integrated Nginx, Redis and other commonleth2-br up
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
