#!/bin/bash

# Unmount and remove hugetlbfs
sudo umount /mnt/huge
sudo rmdir /mnt/huge

# Reset hugepages configuration
sudo su -c "echo 0 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages"

# Re-enable Address Space Layout Randomization
sudo su -c "echo 2 > /proc/sys/kernel/randomize_va_space"

# Remove loaded kernel modules
sudo rmmod rte_kni
sudo rmmod igb_uio

# Revert network interface binding
sudo ifconfig wlp1s0 up
sudo python3 /data/f-stack/dpdk/usertools/dpdk-devbind.py --unbind wlp1s0

# Restore original pkg-config
sudo rm /usr/bin/pkg-config
sudo mv /usr/bin/pkg-config.bak /usr/bin/pkg-config

# Remove F-Stack directory
sudo rm -rf /data/f-stack

# Clean up downloaded files and directories
sudo rm -rf /data/pkg-config-0.29.2*
