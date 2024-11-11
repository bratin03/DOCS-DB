#include "../config/config.h"
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

    for (int i = 0; i < 5; i++)
    {
        uint32_t pageID = pageManager.createPage(config);
        void * start = pageManager.getPage(pageID);
        
        cout << "Page ID: " << pageID << endl;
    }


    cout << "\nCurrent Configuration:\n";
    config.printConfig();
    



    return 0;
}
