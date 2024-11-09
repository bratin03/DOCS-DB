#include "config.h"
#include "pages.h"
#include <iostream>
using namespace std;

int main() {

    PageManager pageManager;

    string config_file_pathv = pageManager.getPrevDirectory() + "/config.txt";
    ConfigManager config(config_file_pathv);
    string temp = config.get("pageCount");
    pageCount = stoull(temp);

    cout << "Page Count: " << pageCount << endl;

    // for(int i = 0; i < 10; i++) {
    //     createPage(config);
    // }

    // for(int i = 0; i < 5; i++) {
    //     removePage(i);
    // }

    std::cout << "\nCurrent Configuration:\n";
    config.printConfig();

    return 0;
}
