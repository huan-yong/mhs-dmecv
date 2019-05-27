#!/usr/bin/python
# -*- coding: UTF-8 -*-

import commands
import time

if __name__ == "__main__":
    """main
    """
    start = time.time()
    in_dir = "dmecv-20-200"
    #1.read in_file
    file_handler = open("%s/input.txt" % (in_dir), "r")
    #file_handler = open("%s/input_test.txt" % (in_dir), "r")
    file_list = file_handler.readlines()
    file_handler.close()
    file_handler = open("%s/output.txt" % (in_dir), "w")
    #file_handler = open("%s/output_test.txt" % (in_dir), "w")
    for f in file_list:
        f = f.strip()
        cmd = "./mhs_dmecv %s/%s" % (in_dir, f)
        (status, output) = commands.getstatusoutput(cmd)
        print("%s" % (output))
        file_handler.write("%s\n" % (output))
    end = time.time()
    duration = end - start
    print("total running time: %ds" % (duration))
    file_handler.write("total running time: %ds\n" % (duration))
    file_handler.close()
