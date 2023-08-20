#include "../inc/parse.h"
#include "../inc/error.h"

void arg_parse(int argc, char* argv[], std::string* file_name, bool* print_opt) {
    
    int c;
    
    static struct option long_options[] = {
        {"binary"   , required_argument , 0, 'b'},
        {"print"    , no_argument       , 0, 'p'},
        {"help"     , no_argument       , 0, 'h'},
        {0, 0, 0, 0 }
    };
    
    int option_index = 0;

    while ((c = getopt_long(argc, argv, "b:ph", long_options, &option_index)) != -1) {
        switch (c) {
            case 'b' :
                if (optarg) {
                    std::string optarg_s(optarg);
                    *file_name = optarg_s;
                }
                break;
            case 'p' : 
                *print_opt = true;
                break;
            case 'h' :
                help();
                usage();
                exit(0);
                break;
            case '?' :
                usage();
                help();
                exit(0);
                break;
        }
    }
}

void usage() {
    std::cout << "Usage : ./main --binary <binary.bin> --print \n";
    std::cout << "        ./main -b <binary.bin> -p \n\n";
    std::cout << "help  : ./main --help\n";
    std::cout << "      : ./main -h\n";
}

void help() {
    std::cout << "--binary <file> | -b <file>\t\t File name to test.\n";
    std::cout << "                           \t\t <file> must be binary file.\n";
    std::cout << "                           \t\t <file> file extension must be *.bin.\n";
    std::cout << "                           \t\t This option is required.\n\n";
    
    std::cout << "--print | -p               \t\t Determine whether to print result.\n";
    std::cout << "                           \t\t This option is optional.\n\n";

    std::cout << "--help | -h                \t\t Display this information.\n";
}