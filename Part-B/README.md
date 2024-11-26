## Namespace and Bridge

### Install the packages 

1. `brctl` for bridge tools
```sh
sudo apt-get install bridge-utils
```

### Setup Network Namespace and Bridge



To run the `script.sh` file, use the following command:
```sh
chmod +x setup.sh
sudo ./script.sh
chmod +x test_setup.sh
sudo ./test_setup.sh
```

For testing the above setup:
```sh
chmod +x test_setup.sh
sudo ./test_setup.sh
```


#### Note

The pings should be sucessful upon running the test. The delay and packet loss may come out of range sometimes.


## FStack Setup

```sh
sudo mkdir -p /data/f-stack
sudo git clone https://github.com/F-Stack/f-stack.git /data/f-stack

```

```sh

sudo apt update
sudo apt install -y build-essential libnuma-dev python3 python3-pip libssl-dev


```

```sh
pip3 install pyelftools --upgrade

```

```sh

sudo apt install -y meson ninja-buildpython3-setuptools 

```


```sh
cd /data/f-stack/dpdk
meson -Denable_kmods=true -Ddisable_libs=flow_classify -Ddisable_drivers=crypto/openssl build
ninja -C build
sudo ninja -C build install


```


```sh
sudo su
echo 0 > /proc/sys/kernel/randomize_va_space
exit
```

```sh
echo 1024 | sudo tee /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
sudo mkdir -p /mnt/huge
sudo mount -t hugetlbfs nodev /mnt/huge
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space


```
Offload NIC

```sh

sudo modprobe uio
sudo insmod /data/f-stack/dpdk/build/kernel/linux/igb_uio/igb_uio.ko
sudo insmod /data/f-stack/dpdk/build/kernel/linux/kni/rte_kni.ko carrier=on


```

