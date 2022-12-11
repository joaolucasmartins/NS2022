#!/bin/python

from sys import argv
import pandas as pd

def openDatasets(config_name):
    sca = pd.read_csv(config_name + "_sca.csv")
    vec = pd.read_csv(config_name + "_vec.csv")

    # Convert string lists into actual lists
    return sca, vec

def generatePlots(sca_df, vec_df):
    filterByClients = lambda x: x["module"].str.contains("client\[")
    filterByClientRouter = lambda x: x["module"].str.contains("clientR")
    filterByTrains = lambda x: x["module"].str.contains("train\[")
    filterByTrainRouter = lambda x: x["module"].str.contains("trainR")
    print("A")

def usage():
    print("usage: generatePlots config_name")

if (__name__ == "__main__"):
    if (len(argv) != 2):
        usage()
    sca_df, vec_df = openDatasets(argv[1])
    generatePlots(sca_df, vec_df)