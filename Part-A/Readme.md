## DOCS-DB (Part-A)

# Structure of The Project
```
- `build.sh` - Shell script to build the project.
- `CMakeLists.txt` - CMake configuration file.
- `docs/` - Directory containing documentation files.
- `Doxyfile` - Configuration file for Doxygen.
- `Readme.md` - Project readme file.
- `repl.cpp` - Source file for the REPL (Read-Eval-Print Loop).
- `repl.txt` - Example REPL input file.
- `setup.sh` - Shell script to set up the project.
- `src/` - Directory containing source code files.
- `test/` - Directory containing test files.
```

# Running the Project
>> 1. Run the `setup.sh` script to set up the project.
>> 2. Run the `build.sh` script to build the project.
>> 3. Go to the `build` directory and run the `repl` executable to start the REPL. This takes as first argument the path to a repl input file. Example input files are provided in the `repl.txt` file. It also takes an optional second argument to specify the output file. If no output file is specified, the output will be printed to the console.

# Testing the Project
>> 1. After building the project, go to the `build/test` directory. This directory contains the test executables. There are total 5 test executables. Run each of them to test the corresponding functionality.

# Generating Documentation
>> 1. Run the `doxygen` command in the root directory of the project. This will generate the documentation in the `docs` directory using the configuration specified in the `Doxyfile`.
>> 2. There is also a pdf file in the `docs` directory which contains the design of the project.

### Note:
>> Please install required dependencies before running the project. It needs boost library to be installed in the system. For any other installation issues, fixes are generally available on the internet.