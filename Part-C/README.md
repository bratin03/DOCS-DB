### Instructions

1. Build the project by running the Makefile:
```sh
make
```

2. Run the benchmarks:

```sh
chmod +x run_benchmark.sh
./run_benchmark.sh
```

This script will output redis-benchmark results to benchmark_results/, with filenames indicating the parameters used (e.g., benchmark_10000_reqs_10_conn_no_dpdk.txt). Each file will contain the results for SET and GET commands under the specified configurations.
