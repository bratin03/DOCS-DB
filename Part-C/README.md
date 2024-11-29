# DOCS-DB (Part-C)

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


### Running the code

1. Run the following command to start the server.

```bash
python server.py
```

Starts the server at `127.0.0.1:6379`.

2. Open another terminal and run the following command to start the client.

```bash
python client.py
```


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

## Running server with DPDK

### Setup
1. Use the `c_wrapper.sh` script to compile the wrapper for using C++ code in C
