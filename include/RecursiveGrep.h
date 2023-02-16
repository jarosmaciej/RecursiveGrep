#ifndef RECURSIVEGREP_H
#define RECURSIVEGREP_H

#include <vector>
#include <thread>
#include <utility>

struct singleGrepInfo {
    std::string fileName;
    std::vector<std::pair<int, std::string>> linesWithPattern; // First pair's parameter is a line number, second - it's content
    unsigned lineCounter; // with pattern
    singleGrepInfo() : lineCounter(0){}
};

class RecursiveGrep {
    std::string pattern;
    std::string dir;
    std::string logFile;
    std::string resultFile;
    int nrOfThreads;

    std::vector<singleGrepInfo> filesWithPattern;
    int sumOfPatterns;
    int searchedFiles;
    double timeElapsed;

public:
    RecursiveGrep(std::string, std::string, std::string, std::string, int nrOfThreads);

    void searchFiles();    
    singleGrepInfo grep(const std::string&);
    bool isDir(const std::string&);
    void createLogFile();
    void createResultFile();
    std::string toString();
    
};

#endif
