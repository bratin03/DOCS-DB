/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#include "src/lsm_tree/lsm_tree.h"
#include <iostream>
#include <fstream>
#include <sstream>

// #define TEST_MODE // This flag is only for testing purposes

enum commands
{
    SET,
    GET,
    DEL,
    EXIT,
    NONE,
};

/**
 * @brief Converts a string command to its corresponding enum value.
 * @param in_str The input command string.
 * @return The corresponding `commands` enum value.
 */
commands cmd_to_enum(const std::string &in_str)
{
    if (in_str == "SET" || in_str == "set")
        return SET;
    if (in_str == "GET" || in_str == "get")
        return GET;
    if (in_str == "DEL" || in_str == "del")
        return DEL;
    if (in_str == "EXIT" || in_str == "exit")
        return EXIT;
    return NONE;
}

/**
 * @brief Processes commands from an input file and executes them using a thread pool.
 * @param filename The name of the input file containing commands.
 * @param outfile The output file stream for logging results.
 */
void command_loop(const std::string &filename, std::ostream &outfile)
{
    lsm_tree db;
    bool run = true;

    std::ifstream infile(filename); // Open the file for reading
    if (!infile.is_open())
    {
        outfile << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;

    while (std::getline(infile, line) && run) // Read each line from the file
    {
        std::istringstream iss(line);
        std::string command, key, value;
        iss >> command; // Read the first word (command)

        switch (cmd_to_enum(command))
        {
        case SET:
            if (iss >> key && std::getline(iss, value))
            {
                // Extract value inside quotes if it's in the format: "value"
                size_t start = value.find('"');
                size_t end = value.rfind('"');

                if (start != std::string::npos && end != std::string::npos && start < end)
                {
                    value = value.substr(start + 1, end - start - 1);
                    db.put(key, value);
                }
                else
                {
                    outfile << "Invalid format for SET. Use: SET <key> \"<value>\"" << std::endl;
                }
            }
            else
            {
                outfile << "Invalid format for SET. Use: SET <key> \"<value>\"" << std::endl;
            }
            break;

        case GET:
            if (iss >> key)
            {
                if (key.empty())
                {
                    outfile << "Key cannot be empty." << std::endl;
                }
                else
                {
#ifdef TEST_MODE
                    // Check if key and value are the same
                    std::string value = db.get(key);
                    if (key != value)
                    {
                        // Fail and exit
                        std::cout << "Failed for key: " << key << std::endl;
                        std::cout << "Expected: " << key << std::endl;
                        std::cout << "Got: " << value << std::endl;
                        exit(1);
                    }
#endif
                    outfile << "Key: " << key << ", Value: " << db.get(key) << std::endl;
                }
            }
            else
            {
                outfile << "Invalid format for GET. Use: GET <key>" << std::endl;
            }
            break;

        case DEL:
            if (iss >> key)
            {
                if (key.empty())
                {
                    outfile << "Key cannot be empty." << std::endl;
                }
                else
                {
                    db.remove(key);
                }
            }
            else
            {
                outfile << "Invalid format for DEL. Use: DEL <key>" << std::endl;
            }
            break;

        case EXIT:
            run = false;
            break;

        default:
            outfile << "Command '" << command << "' not found." << std::endl;
            break;
        }
    }

    infile.close(); // Close the input file when done
}

/**
 * @brief Main entry point for the REPL program.
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @return The exit status of the program.
 */
int main(int argc, const char *argv[])
{
    // Check if the correct number of arguments are provided
    if (argc != 2 && argc != 3)
    {
        // Input file must be provided
        // Output file is optional
        std::cerr << "Invalid number of arguments" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <input_file> [output_file]" << std::endl;
        exit(1);
    }

    std::string filename = argv[1];

    if (argc == 3)
    {
        std::ofstream outfile(argv[2]);
        command_loop(filename, outfile);
        outfile.close();
    }
    else
    {
        command_loop(filename, std::cout);
    }

    return 0;
}
