
#include "../inc/parse.h"
#include "../inc/error.h"

void arg_parse(int argc, char* argv[], std::string* file_name, bool* print_opt) {

    std::string str_help = "--help";
    std::string str_h    = "-h";

    switch (argc) {
        // No argument
        case 1 : {
            usage();
            help();
            invalid_arg("Usage is wrong.\n");

            break;
        }
        // 1 argument
        case 2 : {
            std::string arg1 = argv[1];
            
            if (arg1 == str_help || arg1 == str_h) {
                help();
                exit(0);
            }

            check_file(arg1, file_name);
            *print_opt = false;

            break;
        }
        // 2 arguments
        case 3 : {
            std::string arg1 = argv[1];
            std::string arg2 = argv[2];

            check_file(arg1, file_name);
            check_print(arg2, print_opt);

            break;
        }
        
        // No more possibility
        default : {
            usage();
            help();
            invalid_arg("Usage is wrong.\n");

            break;
        }

    }
}

void usage() {
    std::cout << "Usage : ./main --binary=<binary.bin> --print=[bool]\n";
    std::cout << "        ./main -b=<binary.bin> -p=[bool]\n\n";
    std::cout << "help  : ./main --help\n";
    std::cout << "      : ./main -h\n";
}

void help() {
    std::cout << "--binary=<file> | -b=<file>\t\t File name to test.\n";
    std::cout << "                           \t\t <file> must be binary file.\n";
    std::cout << "                           \t\t <file> file extension must be *.bin.\n";
    std::cout << "                           \t\t This option is required.\n\n";
    
    std::cout << "--print=[bool] | -p=[bool] \t\t Determine whether to print result.\n";
    std::cout << "                           \t\t [bool] must be true or false with small letters.\n";
    std::cout << "                           \t\t This option is optional.\n\n";

    std::cout << "--help | -h                \t\t Display this information.\n";
}

bool check_bin(std::string arg) {
    if (arg.substr(arg.size() - 3, arg.size()) == "bin") 
        return true;
    else 
        return false;
}

void check_file(std::string arg, std::string* file_name) {
    std::string opt_binary = "--binary=";
    std::string opt_b = "-b=";

    // if binary file exists with long option
    if (arg.find(opt_binary) != std::string::npos) {
        arg = arg.substr(opt_binary.size());

        if (check_bin(arg)) {
            file_name->append(arg);
        }
        else {
            usage();
            help();
            invalid_arg("<file> extension must be *.bin\n");
        }
    }
    // if binary file exists with short option
    else if (arg.find(opt_b) != std::string::npos) {
        arg = arg.substr(opt_b.size());

        if (check_bin(arg)) {
            file_name->append(arg);
        }
        else {
            usage();
            help();
            invalid_arg("<file> extension must be *.bin\n");
        }
    }
    // No other possibility
    else {
        usage();
        help();
        invalid_arg("Usage is wrong.\n");
    }
}


void check_print(std::string arg, bool* point_opt) {
    std::string opt_print = "--print=";
    std::string opt_p     = "-p=";
    std::string opt_true  = "true";
    std::string opt_false = "false";

    // if print option exists with long option
    if (arg.find(opt_print) != std::string::npos) {
        arg = arg.substr(opt_print.size());

        if (arg == opt_true) 
            *point_opt = true;
        else 
            *point_opt = false;
        
    }
    // if print option exits with short option
    else if (arg.find(opt_p) != std::string::npos) {
        arg = arg.substr(opt_p.size());

        if (arg == opt_true) 
            *point_opt = true;
        else
            *point_opt = false;
    }
    // No other possibility
    else {
        usage();
        help();
        invalid_arg("Usage is wrong.\n");
    }

}



