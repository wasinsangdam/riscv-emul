#!/usr/bin/env python3

import sys
import re
import os

def run_test(binary_dir) :
    bins = [b for b in os.listdir(binary_dir)]
    bins.sort()
    for b in bins :
        os.system("./main" + " -b=" + binary_dir + "/" + b)
        

if __name__ == "__main__" :
    if len(sys.argv) != 2 :
        print("Usage : ./main <binary_dir>")
        exit(1)
    
    binary_dir = sys.argv[1]
    run_test(binary_dir)