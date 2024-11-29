# DOCS-DB (Part-B)

## Structure of The Project

- `bin/`: Directory containing binary files.
- `config/`: Directory containing configuration files.
- `docs/`: Directory containing documentation files.
- `Doxyfile`: Configuration file for Doxygen.
- `log/`: Directory containing log files.
- `Makefile`: File containing build instructions.
- `Readme.md`: Project readme file.
- `scripts/`: Directory containing various scripts.
- `src/`: Directory containing source code files.


### Note: Since DPDK can not be run on a virtual NIC, for running our code we use a virtual machine. The server can be run on the virtual machine with and without DPDK. The client can be run on the host machine. We build a virtual machine with a NIC that can be configured to run with DPDK. 

## Creating a Virtual Network
>> 1. Run the `setup.sh` script to set up the namespaces and `test_setup.sh` to test the setup.

>> 2. Run the `test_setup.sh` script to test the setup.


## Building the Project
>> 1. Run the `Makefile` to build the project. It needs DPDK and F-stack to be installed on the system.
>> 2. Alternatively, you can run the `compile-server.sh` and `compile-client.sh` scripts to compile the server and client respectively. This will not compile the DPDK server. 
>> 3. For changing the IP address of the normal server, you can change the IP address in the `server.cpp` file.
>> 4. For changing the IP address of the DPDK server, you can change the IP address in the 'config/config.ini' file.
>> 5. Use the IP of the server in the client file `client.cpp` to connect to the server.

## Server without DPDK
>> 1. Run the `server` file ans also retrieve the PID of the server.
>> 2. You can do so by running as './server & echo $! > server.pid'

## Server with DPDK
>> 1. To run the server with DPDK, run the `dpdk-server.sh` script. Make sure to change the IP address in the `config/config.ini` file and other configurations as required.
>> 2. Retrieve the PID of the server by running the command `ps -aux | grep dpdk_server`.

## Running the Client
>> 1. Use the `client.sh` script to run the clients. It takes as input the number of clients to run and the number of packets to send from each client.
>> 2. It also prints the average latency and bandwidth of the system.

## Monitoring Memory and CPU Usage
>> 1. Run the `server.sh` script to monitor the memory and CPU usage of the server. It takes as command line argument the command to run the server. It prints the average memory and CPU usage of the server after it is stopped.
>> 2. You can also run the `memory_cpu.sh` script to monitor the memory and CPU usage of the server. It takes as command line argument the PID of the server. It prints the average memory and CPU usage of the server after it is stopped.

## Generating Documentation
>> 1. Run the `doxygen` command in the root directory of the project. This will generate the documentation in the `docs` directory using the configuration specified in the `Doxyfile`.
>> 2. There is also a pdf file in the `docs` directory which contains the design of the project.
