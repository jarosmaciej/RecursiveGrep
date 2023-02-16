#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <stack>

#include "../include/RecursiveGrep.h"
#include "../include/ThreadPool.h"

using std::chrono::duration_cast;
using std::chrono::milliseconds;

RecursiveGrep::RecursiveGrep(std::string pattern, std::string dir, std::string logFile, std::string resultFile, int nrOfThreads) : pattern(pattern), dir(dir), logFile(logFile), resultFile(resultFile), nrOfThreads(nrOfThreads), sumOfPatterns(0), searchedFiles(0), timeElapsed(0){

    auto start = std::chrono::steady_clock::now();

    searchFiles();

    auto end = std::chrono::steady_clock::now();

    timeElapsed = duration_cast<milliseconds>(end - start).count();
}

void RecursiveGrep::searchFiles() {
    ThreadPool pool(nrOfThreads);
    std::vector<std::future<singleGrepInfo> > results;
    std::stack<std::string> stack;
    stack.push(dir);

    while (!stack.empty()) {
        std::string path = stack.top();
        stack.pop();
        DIR* directory = opendir(path.c_str());
        if (!directory) {
            std::cerr  << path << " directory cannot be opened"<< std::endl;
            continue;
        }

        struct dirent* dp;
        while ((dp = readdir(directory)) != nullptr) {
            if (dp->d_type == DT_DIR) {
                if (dp->d_name[0] == '.') {
                    continue;
                }

                std::string subdir = path + "/" + dp->d_name;
                if (is_dir(subdir)) {
                    stack.push(subdir);
                }
            } else if (dp->d_type == DT_REG) {
                std::string fileName = path + "/" + dp->d_name;
                results.emplace_back(
                    pool.enqueue([this, fileName] {
                        // TODO pushback to this table for logfile
                        ++searchedFiles;
                        return grep(fileName);
                    })
                );
            }
        }

        closedir(directory);    
    }
}

singleGrepInfo RecursiveGrep::grep(const std::string& fileName) {
    singleGrepInfo stats;
    stats.fileName = fileName;
    std::ifstream file(fileName);
    if (file) {
        std::string line;
        int lineNum = 1;
        while (std::getline(file, line)) {
            if (line.find(pattern) != std::string::npos) {
                ++stats.lineCounter;
                ++sumOfPatterns;
                std::cout << line << " " << fileName << std::endl << std::endl;// TODO delete
                stats.linesWithPattern.push_back(std::pair<int, std::string>(lineNum, line));
            }
            lineNum++;
        }
    }
    return stats;
}

bool RecursiveGrep::is_dir(const std::string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
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
