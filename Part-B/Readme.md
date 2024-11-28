# DOCS-DB (Part-B)

## Structure of The Project

- `docs/`: Directory containing documentation files.
- `Doxyfile`: Configuration file for Doxygen.
- `Readme.md`: Project readme file.
- `Makefile`: File containing build instructions.
- `src/`: Directory containing source code files.
- `setup.sh`: Script to set up the namespaces.
- `test_setup.sh`: Script to test the setup.
- `memory_cpu.sh`: Script to monitor memory and CPU usage.
- `server.sh`: Script to run and monitor the server.
- `client.sh`: Script to run the client.


### Note: Since DPDK can not be run on a virtual NIC, for running our code we use a virtual machine. The server can be run on the virtual machine with and without DPDK. The client can be run on the host machine. We build a virtual machine with a NIC that can be configured to run with DPDK. 

## Creating a Virtual Network
>> 1. Run the `setup.sh` script to set up the namespaces and `test_setup.sh` to test the setup.

>> 2. Run the `test_setup.sh` script to test the setup.


## Building the Project
>> 1. Run the `Makefile` to build the project.

## Server without DPDK
>> 1. Run the `server` file ans also retrieve the PID of the server.
>> 2. You can do so by running as './server & echo $! > server.pid'

## Server without DPDK
<!-- To be added -->

## Running the Client
>> 1. Use the `client.sh` script to run the clients. It takes as input the number of clients to run and the number of packets to send from each client.
>> 2. It also prints the average latency and bandwidth of the system.

## Monitoring Memory and CPU Usage
>> 1. Run the `server.sh` script to monitor the memory and CPU usage of the server. It takes as command line argument the command to run the server. It prints the average memory and CPU usage of the server after it is stopped.
>> 2. You can also run the `memory_cpu.sh` script to monitor the memory and CPU usage of the server. It takes as command line argument the PID of the server. It prints the average memory and CPU usage of the server after it is stopped.

## Generating Documentation
>> 1. Run the `doxygen` command in the root directory of the project. This will generate the documentation in the `docs` directory using the configuration specified in the `Doxyfile`.
>> 2. There is also a pdf file in the `docs` directory which contains the design of the project.
