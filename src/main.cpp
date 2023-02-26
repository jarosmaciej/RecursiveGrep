#include <getopt.h>

#include "../include/RecursiveGrep.h"

void help(std::string programName){
    std::cerr << "Usage: " << programName << " <pattern> [-d|--dir <start_dir>] [-l|--log_file <log_file_name>]" << std::endl
              << "                              [-r|--result_file <result_file_name>] [-t|--threads <num_threads>]" << std::endl << std::endl
              << "Parameters:" << std::endl
              << "  <pattern>          Required. A string to search for in the files." << std::endl
              << "  -d, --dir          The starting directory where the program looks for files (including subfolders)." << std::endl
              << "                     Default value: current directory." << std::endl
              << "  -l, --log_file     The name of the log file. The default name is \"<program_name>.log\"." << std::endl
              << "  -r, --result_file  The name of the file where the search results are saved. The default name is \"<program_name>.txt\"." << std::endl
              << "  -t, --threads      The number of threads in the thread pool. The default value is 4." << std::endl;
}

int main(int argc, char*argv[]) {
    /* Default values of arguments */
    std::string dir = ".";
    std::string programName = basename(argv[0]);
    std::string logFile = programName, resultFile = programName;
    int nrOfThreads = 4;

    static struct option longOpts[] = {
        {"dir", required_argument, nullptr, 'd'},
        {"log_file", required_argument, nullptr, 'l'},
        {"result_file", required_argument, nullptr, 'r'},
        {"threads", required_argument, nullptr, 't'},
        {nullptr, no_argument, nullptr, 0}
    };
    int c;
    while ((c = getopt_long(argc, argv, "d:l:r:t:", longOpts, 0)) != -1) {
        switch (c) {
        case 'd':
            dir = optarg;
            break;
        case 'l':
            logFile = optarg;
            break;
        case 'r':
            resultFile = optarg;
            break;
        case 't':
            nrOfThreads = atoi(optarg);
            /* Checks if nr of threads is a natural number */
            if (nrOfThreads <= 0){
                help(argv[0]);
                return 1;
            }
        default:
            break;
        }
    }

    /* If there's inavlid number of arguments shows informations how to use program and returns 1 */
    if (argc - optind != 1) {
        help(argv[0]);
        return 1;
    }

    std::string pattern = argv[optind];

    RecursiveGrep rg(pattern, dir, logFile + ".log", resultFile + ".txt", nrOfThreads);
    std::cout << rg.toString();
    return 0;
}
