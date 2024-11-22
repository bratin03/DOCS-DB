sudo mkdir -p /data/f-stack

sudo git clone --branch master https://github.com/F-Stack/f-stack.git /data/f-stack

sudo apt-get install libnuma-dev
sudo pip3 install pyelftools --upgrade

cd /data/f-stack/dpdk/

sudo apt install meson
sudo apt install ninja-build

sudo meson -Denable_kmods=true -Ddisable_libs=flow_classify -Ddisable_drivers=crypto/openssl build

sudo ninja -C build

sudo ninja -C build install

sudo ldconfig

sudo su
echo 1024 >/sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
exit

sudo mkdir /mnt/huge

sudo mount -t hugetlbfs nodev /mnt/huge

sudo su

echo 0 >/proc/sys/kernel/randomize_va_space

exit

sudo modprobe uio

sudo insmod /data/f-stack/dpdk/build/kernel/linux/igb_uio/igb_uio.ko

sudo insmod /data/f-stack/dpdk/build/kernel/linux/kni/rte_kni.ko carrier=on

python3 /data/f-stack/dpdk/usertools/dpdk-devbind.py --status



sudo ifconfig wlp1s0 down
sudo python3 /data/f-stack/dpdk/usertools/dpdk-devbind.py --bind=igb_uio wlp1s0

sudo apt-get install gawk

sudo apt install gcc make libssl-dev

cd /data

sudo wget https://pkg-config.freedesktop.org/releases/pkg-config-0.29.2.tar.gz

sudo tar xzvf pkg-config-0.29.2.tar.gz

cd pkg-config-0.29.2

sudo ./configure --with-internal-glib

sudo make

sudo make install

sudo mv /usr/bin/pkg-config /usr/bin/pkg-config.bak

sudo ln -s /usr/local/bin/pkg-config /usr/bin/pkg-config

export FF_PATH=/data/f-stack

export PKG_CONFIG_PATH=/usr/lib64/pkgconfig:/usr/local/lib64/pkgconfig:/usr/lib/pkgconfig:/usr/local/lib/x86_64-linux-gnu/pkgconfig/

cd /data/f-stack/lib/

sudo PKG_CONFIG_PATH=$PKG_CONFIG_PATH make
make install
