#include <iostream>
#include <getopt.h>
#include "../include/RecursiveGrep.h"

int main(int argc, char*argv[]) {
    /* Default values of arguments */
    std::string dir = "./", logFile = argv[0], resultFile = argv[0];
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
            if (nrOfThreads <= 0){
                //help(); TODO
                std::cerr << "Help" << std::endl;
            }
        default:
            break;
        }
    }

    /* If there's inavlid number of arguments shows informations how to use program and returns 1 */
    if (argc - optind != 1) {
        //help(); TODO
        std::cerr << "Help" << std::endl;
        return 1;
    }

    std::string pattern = argv[optind];

    RecursiveGrep rg(pattern, dir, logFile, resultFile, nrOfThreads);
    std::cout << rg.toString();
    return 0;
}
