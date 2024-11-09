#include "config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "../debug/debug.h"
using namespace std;

#ifdef LOG
const string LOG_CONIFIG_FILE = "/tmp/utils_config.log";
#endif

// Constructor - initializes the config file and loads its content
ConfigManager::ConfigManager(const string &filename) : config_file(filename)
{
    load();
}

// Destructor - saves the configuration back to the file when the program ends
ConfigManager::~ConfigManager()
{
    save();
}

// Load the configuration from the file
void ConfigManager::load()
{
    ifstream file(config_file);
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            istringstream iss(line);
            string key, value;
            if (getline(iss, key, '=') && getline(iss, value))
            {
                config[key] = value;
#ifdef LOG
                log(LOG_CONIFIG_FILE, INFO, "Key: " + key + " Value: " + value);
#endif
            }
        }
        file.close();
    }
    else
    {
#ifdef LOG
        log(LOG_CONIFIG_FILE, ERROR, "Could not read from config file: " + config_file);
#endif
    }
}

// Save the configuration to the file
void ConfigManager::save() const
{
    ofstream file(config_file);
    if (file.is_open())
    {
        for (const auto &[key, value] : config)
        {
            file << key << "=" << value << "\n";
#ifdef LOG
            log(LOG_CONIFIG_FILE, INFO, "Key: " + key + " Value: " + value);
#endif
        }
        file.close();
    }
    else
    {
#ifdef LOG
        log(LOG_CONIFIG_FILE, ERROR, "Could not write to config file: " + config_file);
#endif
    }
}

// Get a value by key (returns default if key not found)
string ConfigManager::get(const string &key, const string &default_value) const
{
    auto it = config.find(key);
#ifdef LOG
    if (it == config.end())
    {
        log(LOG_CONIFIG_FILE, WARNING, "Key not found: " + key);
    }
    else
    {
        log(LOG_CONIFIG_FILE, INFO, "Retrieved key: " + key + " Value: " + it->second);
    }
#endif
    return (it != config.end()) ? it->second : default_value;
}

// Set a value for a key
void ConfigManager::set(const string &key, const string &value)
{
#ifdef LOG
    log(LOG_CONIFIG_FILE, INFO, "Updating key: " + key + " Value: " + value);
#endif
    config[key] = value;
}

// Print all key-value pairs in the configuration
void ConfigManager::printConfig() const
{
    for (const auto &[key, value] : config)
    {
        cout << key << "=" << value << "\n";
    }
}
