#include "config.h"
#include "pages.h"
#include <iostream>
using namespace std;

int main() {
    // Create an instance of ConfigManager

    string config_file_pathv = getPrevDirectory() + "/config.txt";

    ConfigManager config(config_file_pathv);

    // Access and modify configuration
    config.set("pageCount", "0");
    // config.set("theme", "dark");
    // config.set("window_width", "1024");
    // config.set("window_height", "768");

    // Access individual configuration settings
    std::cout << "pageCount: " << config.get("pageCount") << "\n";
    // std::cout << "Theme: " << config.get("theme") << "\n";

    // Print entire config for verification
    std::cout << "\nCurrent Configuration:\n";
    config.printConfig();

    // The configuration will be saved when the program exits (via destructor)
    return 0;
}
