# DOCS-DB (Part-C)

## Structure of The Project

Project Information:
- `README.md` - Project readme file.
- `Makefile` - Makefile to compile the project.
- `requirements.txt` - File listing the required Python packages.
- `Doxyfile` - Configuration file for Doxygen.

Directories:
- `bin/` - Directory containing binary files.
- `config/` - Directory containing configuration files.
- `docs/` - Directory containing documentation files.
- `src/` - Directory containing source code files.

Shell scripts:
- `benchmark.sh` - Shell script for benchmarking the server.
- `convert.sh` - Shell script to convert files.
- `c_wrapper.sh` - Shell script to compile the C++ wrapper.
- `dpdk-server.sh` - Shell script to run the server with DPDK.
- `temp.sh` - Temporary shell script.


## Running server without DPDK

### Setup

1. Create a virtual environment and activate it.

```bash
python3 -m venv venv
source venv/bin/activate
```

2. Install the required packages.

```bash
pip install -r requirements.txt
```

3. Run `convert.sh`

4. Add the path of the `Part-A/build/src` to `LD_LIBRARY_PATH` in the `~/.bashrc` file.

```bash
export LD_LIBRARY_PATH=<path-to-docs-db>/DOCS-DB/Part-A/build/src:$LD_LIBRARY_PATH
```

5. Run the following command to start the server.

```bash
python server.py
```

Starts the server at `127.0.0.1:6379`.

6. Open another terminal and run the following command to start the client.

```bash
python client.py
```

## Running server with DPDK

### Setup
1. Use the `c_wrapper.sh` script to compile the wrapper for using C++ code in C

2. Use the `Makefile` to compile the DPDK code.

3. Use the `dpdk-server.sh` script to run the server.

4. Create a virtual environment and activate it.

```bash
python3 -m venv venv
source venv/bin/activate
```

5. Install the required packages.

```bash
pip install -r requirements.txt
```

6. Open another terminal and run the following command to start the client.

```bash
python client.py
```

## Benchmarking the server

### Benchmarking

1. Install the package `redis-benchmark` using the following command.

```bash
sudo apt-get install redis-tools
```

2. Run the following command to start the benchmarking.


- `GET` requests with 1000 parallel connections and 1000000 requests.

```bash
redis-benchmark -h 127.0.0.1 -p 6379 -c 1000 -n 100000 -t GET
```

- `SET` requests with 1000 parallel connections and 1000000 requests.

```bash
redis-benchmark -h 127.0.0.1 -p 6379 -c 1000 -n 100000 -t SET
```

>> Note: Set diferent IP address in the `config.ini` file or in the source code as required.