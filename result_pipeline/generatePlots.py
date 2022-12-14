#!/bin/python

from sys import argv
import pandas as pd
import numpy as np

def usage():
    print("usage: generatePlots config_name")


if (__name__ == "__main__"):
    if (len(argv) != 2):
        usage()
    sca_df, vec_df = openDatasets(argv[1])
    generatePlots(sca_df, vec_df)