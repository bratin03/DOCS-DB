#include "pages.h"
// #include "config.h"

// Returns the parent directory of the current directory
// It will basically return '<path_to_DOCS_DB>/utils'
uint64_t pageCount = 0;


string getPrevDirectory() {
    char current_directory[PATH_MAX];
    if(getcwd(current_directory, sizeof(current_directory)) == NULL) {
        cout << "Error in getting current directory\n";
        return "";
    }
    char* last_slash = strrchr(current_directory, '/');

    if(last_slash == NULL) {
        cout << "Error in getting current directory\n";
        return "";
    }
    *last_slash = '\0';
    return string(current_directory);
}



void createPage(ConfigManager& config) {

    pageCount++;
    config.set("pageCount", to_string(pageCount));


    hash<uint64_t> hash_fn;
    uint64_t hashValue = hash_fn(pageCount) % HASHMOD;

    string dirPath = getPrevDirectory() + "/tmp/" + to_string(hashValue);
    cout << "Directory path: " << dirPath << endl;
    bool op = filesystem::create_directories(dirPath);
    cout << "Created directory: " << op << endl;

    // Inside this directory, create a file with the name as pageCount
    string filePath = dirPath + "/" + to_string(pageCount);
    ofstream file(filePath);
    file.close();
}


void removePage(uint64_t pageID) {
    // Remove the file with the name as pageID
    string dirPath = getPrevDirectory() + "/tmp/" + to_string(pageID % HASHMOD);
    string filePath = dirPath + "/" + to_string(pageID);
    filesystem::remove(filePath);
}

