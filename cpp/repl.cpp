#include "src/lsm_tree/lsm_tree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
// #define TEST_MODE

enum commands
{
    SET,
    GET,
    DEL,
    EXIT,
    NONE,
};

commands cmd_to_enum(const std::string &in_str)
{
    if (in_str == "SET" || in_str == "set")
        return SET;
    if (in_str == "GET" || in_str == "get")
        return GET;
    if (in_str == "DEL" || in_str == "del")
        return DEL;
    return NONE;
}

void command_loop(const std::string &filename, std::ofstream &outfile)
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
    outfile << "REPL started. Reading from file: " << filename << std::endl;

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
                        std::cout << "Failed: " << key << std::endl;
                        exit(1);
                    }
#endif  
                    outfile << "GET: " << db.get(key) << std::endl;
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

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ofstream outfile("output.txt");

    command_loop(filename, outfile);

    return 0;
}