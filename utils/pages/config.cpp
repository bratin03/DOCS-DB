#include "config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
using namespace std;

// Constructor - initializes the config file and loads its content
ConfigManager::ConfigManager(const string& filename) : config_file(filename) {
    load();
}

// Destructor - saves the configuration back to the file when the program ends
ConfigManager::~ConfigManager() {
    save();
}

// Load the configuration from the file
void ConfigManager::load() {
    ifstream file(config_file);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string key, value;
            if (getline(iss, key, '=') && getline(iss, value)) {
                config[key] = value;
            }
        }
        file.close();
    } else {
        cerr << "Could not open config file: " << config_file << "\n";
    }
}

// Save the configuration to the file
void ConfigManager::save() const {
    ofstream file(config_file);
    if (file.is_open()) {
        for (const auto& [key, value] : config) {
            file << key << "=" << value << "\n";
        }
        file.close();
    } else {
        cerr << "Could not write to config file: " << config_file << "\n";
    }
}

// Get a value by key (returns default if key not found)
string ConfigManager::get(const string& key, const string& default_value) const {
    auto it = config.find(key);
    return (it != config.end()) ? it->second : default_value;
}

// Set a value for a key
void ConfigManager::set(const string& key, const string& value) {
    config[key] = value;
}

// Print all key-value pairs in the configuration
void ConfigManager::printConfig() const {
    for (const auto& [key, value] : config) {
        cout << key << "=" << value << "\n";
    }
}
