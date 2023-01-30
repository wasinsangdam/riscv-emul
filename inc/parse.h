#ifndef __PARSE_H__
#define __PARSE_H__

#include <iostream>
#include <string>
#include <stdexcept>

void arg_parse(int argc, char* argv[], std::string* file_name, bool* print_opt);
void usage();
void help();

bool check_bin(std::string arg);
void check_file(std::string arg, std::string* file_name);
void check_print(std::string arg, bool* print_opt);

#endif