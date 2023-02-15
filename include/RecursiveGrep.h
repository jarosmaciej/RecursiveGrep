#ifndef RECURSIVEGREP_H
#define RECURSIVEGREP_H

#include <vector>
#include <thread>
#include <utility>

struct singleGrepInfo {
    std::string fileName;
    std::thread::id threadId;
    std::vector<std::pair<int, std::string>> linesWithPattern;
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

    void createLogFile();
    void createResultFile();
    std::string toString();
    singleGrepInfo grep(std::string, std::string);
};

#endif
