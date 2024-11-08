#ifndef CONFIG_H
#define CONFIG_H
#include <map>
using namespace std;


#include <string>

class ConfigManager {
private:
    string config_file;

    // Internal map for storing key-value pairs
    map<string, string> config;

    // Load the configuration from the file
    void load();

    // Save the configuration to the file
    void save() const;

public:
    // Constructor
    ConfigManager(const string& filename);

    // Destructor
    ~ConfigManager();

    // Get a value by key (returns default if key not found)
    string get(const string& key, const string& default_value = "") const;

    // Set a value for a key
    void set(const string& key, const string& value);

    // Print all configuration for debugging
    void printConfig() const;
};

#endif // CONFIG_H
