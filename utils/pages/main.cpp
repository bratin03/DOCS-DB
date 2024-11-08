#include "config.h"
#include "pages.h"
#include <iostream>
using namespace std;

int main() {

    string config_file_pathv = getPrevDirectory() + "/config.txt";
    ConfigManager config(config_file_pathv);
    string temp = config.get("pageCount");
    pageCount = stoull(temp);

    cout << "Page Count: " << pageCount << endl;

    // for(int i = 0; i < 10; i++) {
    //     createPage(config);
    // }

    for(int i = 0; i < 5; i++) {
        removePage(i);
    }

    cout << "Page Count: " << pageCount << endl;

    // Print entire config for verification
    std::cout << "\nCurrent Configuration:\n";
    config.printConfig();

    // The configuration will be saved when the program exits (via destructor)
    return 0;
}
