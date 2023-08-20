#!/usr/bin/env python3

import sys
import re
import os

def run_test(test_dir) :
    bins = [b for b in os.listdir(test_dir)]
    bins.sort()
    for b in bins :
        os.system("./main" + " -b" + test_dir + "/" + b)
        

if __name__ == "__main__" :
    if len(sys.argv) != 2 :
        print("Usage : ./test.py <test_dir>")
        exit(1)
    
    test_dir = sys.argv[1]
    run_test(test_dir)