#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>

#include "../include/RecursiveGrep.h"

using std::chrono::duration_cast;
using std::chrono::milliseconds;

RecursiveGrep::RecursiveGrep(std::string pattern, std::string dir, std::string logFile, std::string resultFile, int nrOfThreads) : dir(dir), logFile(logFile), resultFile(resultFile), nrOfThreads(nrOfThreads), sumOfPatterns(0), searchedFiles(0), timeElapsed(0){
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    timeElapsed = duration_cast<milliseconds>(end - start).count();
}

void RecursiveGrep::createLogFile() {

}

void RecursiveGrep::createResultFile() {

}

std::string RecursiveGrep::toString() {
    std::ostringstream stats;
    stats << "Searched files: " << searchedFiles << std::endl;
    stats << "Files with pattern: " << filesWithPattern.size() << std::endl;
    stats << "Patterns number: " << sumOfPatterns << std::endl;
    stats << "Result file: " << resultFile << std::endl;
    stats << "Log file: " << logFile << std::endl;
    stats << "Used threads: " << nrOfThreads << std::endl;
    stats << "Elapsed time (ms): " << timeElapsed << std::endl;
    return stats.str();
}

singleGrepInfo RecursiveGrep::grep(std::string fileName, std::string pattern) {
    singleGrepInfo stats;
    return stats;
}
