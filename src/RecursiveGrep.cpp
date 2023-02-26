#include <chrono>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <stack>

#include "../include/RecursiveGrep.h"
#include "../include/ThreadPool.h"

using std::chrono::duration_cast;
using std::chrono::milliseconds;

RecursiveGrep::RecursiveGrep(std::string pattern, std::string dir, std::string logFile, std::string resultFile, int nrOfThreads) : pattern(pattern), dir(dir), logFile(logFile), resultFile(resultFile), nrOfThreads(nrOfThreads), sumOfPatterns(0), searchedFiles(0), timeElapsed(0){

    auto start = std::chrono::steady_clock::now();

    searchFiles();

    /* Sorts vector of files with pattern's info by number of patterns found */
    std::sort(filesWithPattern.begin(), filesWithPattern.end(), 
        [](const singleGrepInfo & a, const singleGrepInfo & b) -> bool { 
            return a.lineCounter > b.lineCounter; 
        });

    createResultFile();

    /* Sorts vector of pairs (thread ids from pool and vector of file names where processed by this thread files with pattern are stored) */
    std::sort(threadsStats->begin(), threadsStats->end(),
        [](const auto& a, const auto& b) {
            return a.second.size() > b.second.size();
        });

    createLogFile();

    auto end = std::chrono::steady_clock::now();

    timeElapsed = duration_cast<milliseconds>(end - start).count();
}

void RecursiveGrep::searchFiles() {
    ThreadPool pool(nrOfThreads);
    std::vector<std::future<singleGrepInfo> > results;
    std::stack<std::string> stack;
    stack.push(dir);

    /* Before searching the files vector - important for creating a log file - needs to be initialized */
    std::vector<std::thread::id> ids = pool.getIds();
    threadsStats = new std::vector<std::pair<std::thread::id, std::vector<std::string>>>(initThreadsStats(ids));

    /* This searches iterativly directores and if it finds a file - one of threads from thread pool uses grep method on it */
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
                if (isDir(subdir)) {
                    stack.push(subdir);
                }
            } else if (dp->d_type == DT_REG) {
                std::string filePath = path + "/" + dp->d_name;
                results.emplace_back(
                    pool.enqueue([this, filePath] {
                        ++searchedFiles;
                        singleGrepInfo sgi = grep(filePath);
                        if (sgi.lineCounter > 0)
                            filesWithPattern.push_back(sgi);
                        return sgi;
                    })
                );
            }
        }

        closedir(directory);
    }
}

/* Method that works quite like a "normal" grep and return struct "singleGrepInfo" where all important informations about this process is stored */
singleGrepInfo RecursiveGrep::grep(const std::string& filePath) {
    singleGrepInfo stats;
    bool hasPattern = false;
    /* Gets  searched file name from relative path */
    char* baseName = strdup(filePath.c_str());
    /* Gets absolute path converted from reliative path */
    char* absPath = realpath(filePath.c_str(), NULL);    
    stats.fileName = basename(baseName);
    stats.absolutePath = absPath;
    free(baseName);
    free(absPath);
    std::ifstream file(filePath);
    if (file) {
        std::string line;
        int lineNum = 1;
        while (std::getline(file, line)) {
            if (line.find(pattern) != std::string::npos) {
                ++stats.lineCounter;
                ++sumOfPatterns; //not thread safe TODO
                hasPattern = true;
                stats.linesWithPattern.push_back(std::pair<int, std::string>(lineNum, line));
            }
            lineNum++;
        }
    }
    /* If file has pattern push it to current's thread (which processed this file) vector */
    if (hasPattern){
        auto currentThreadId = std::this_thread::get_id();
        auto it = std::find_if(threadsStats->begin(), threadsStats->end(),
            [currentThreadId](const auto& pair){
                return pair.first == currentThreadId;
            });
        it->second.push_back(stats.fileName);
    }
    return stats;
}

/* Method initialize vector of pairs (given thread ids and vector of file names where processed files with pattern will be stored) */
std::vector<std::pair<std::thread::id, std::vector<std::string>>> RecursiveGrep::initThreadsStats(std::vector<std::thread::id> ids){
    std::vector<std::pair<std::thread::id, std::vector<std::string>>> tStats(nrOfThreads);

    std::transform(ids.begin(), ids.end(), tStats.begin(),
        [](const std::thread::id& t) {
            return std::make_pair(t, std::vector<std::string>());
        });
    return tStats;
}

bool RecursiveGrep::isDir(const std::string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

/* Method creates a log file from filled vector "threadsStats" */
void RecursiveGrep::createLogFile() {
    std::ofstream outputFile(logFile);
    if (outputFile) {
        for (auto& threadStats : *threadsStats) {
            outputFile << threadStats.first << ":";
            bool isFirstFile = true;
            for (const auto& fileName : threadStats.second) {
                if (!isFirstFile) {
                    outputFile << ", ";
                }
                outputFile << fileName;
                isFirstFile = false;
            }
            outputFile << std::endl;
        }
        outputFile.close();
    }
}

/* Method creates a txt file from filled vector "filesWithPattern" */
void RecursiveGrep::createResultFile() {
    std::ofstream outputFile(resultFile);
    if (outputFile) {
        for (const auto& it : filesWithPattern) {
            for (const auto& line : it.linesWithPattern) {
                outputFile << it.absolutePath << ":" << line.first << ": " << line.second << std::endl;
            }
        }
        outputFile.close();
    }
}

/* Method return string with some informations about the class */
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

RecursiveGrep::~RecursiveGrep(){
    delete threadsStats;
}
