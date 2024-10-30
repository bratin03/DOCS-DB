# DOCS-DB

## Coding Practices
### 1. Use the `debug` module for logging.
a. All the logfiles are to be stored in the `logs` directory.

b. When logging from a file named x.cpp inside module y, the log file should be named `y_x.log`. For example, if logging from `src/pages.cpp` inside the `src` module, the log file should be named `logs/src_pages.log`.

c. The log function should be called with a `#ifdef DEBUG` wrapper. This is to ensure that the log function is only called in debug mode.

2. For storing pages, the pages should be created in a directory named `datastore`.