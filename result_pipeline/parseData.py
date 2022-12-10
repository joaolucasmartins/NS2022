#!/bin/python

from sys import argv
import pandas as pd

def convertToCsv(config_path, config_name):
    import subprocess
    cmd = "opp_scavetool x " + config_path + config_name + ".{0} -o {1}_{0}.csv"
    export_types = ("sca", "vec")
    ps = []
    for t in export_types:
        process = subprocess.Popen(cmd.format(t, config_name).split())
        ps.append(process)
    for p in ps:
        p.wait()

def openDatasets(config_name):
    sca = pd.read_csv(config_name + "_sca.csv")
    vec = pd.read_csv(config_name + "_vec.csv")

    # Convert string lists into actual lists
    return sca, vec

def convertValsToList(sca_df, vec_df):
    def convertVecToList(df, colname):
        df[colname] = df[colname].apply(lambda x: [x] if isinstance(x, float) else list(map(float, x.split())))

    convertVecToList(sca_df, "binedges")
    convertVecToList(sca_df, "binvalues")
    convertVecToList(vec_df, "vectime")
    convertVecToList(vec_df, "vecvalue")
    return sca_df, vec_df

def filterNans(sca_df, vec_df):
    # Filter only entries that have data in them
    vec_df = vec_df[vec_df["vecvalue"].notna()]
    return sca_df, vec_df


def filterMetrics(sca_df, vec_df):
    # Vectors - parse vec dataset
    linkLayerThroughput = lambda x: (x["name"] == "txPk:vector(packetBytes)") & (type not in x or (x["type"] == "vector"))
    appLayerThroughput = lambda x: (x["name"].str.contains("DataRate")) & (x["type"] == "vector") #  (x["module"].str.contains("Router")) => Use this to filter only router
    clientResponseDelay = lambda x: x["name"] == "timeToResponse"
    serverSentTrainUpdates = lambda x: (x["name"] == "serverSentTrainUpdates")
    serverDroppedTrainUpdates = lambda x: (x["name"] == "serverDroppedTrainUpdates")
    serverReceivedTrainUpdates = lambda x: (x["name"] == "serverReceivedTrainUpdates")
    vec_metrics = (linkLayerThroughput, appLayerThroughput, clientResponseDelay, serverSentTrainUpdates, serverDroppedTrainUpdates, serverReceivedTrainUpdates)

    # Histograms - parse sca dataset
    clientEndToEndDelay = lambda x: (x["name"] == "endToEndDelay:histogram")  & (x["module"].str.contains("client")) & (x["type"] == "histogram")
    #sca_metrics = (clientEndToEndDelay, trainEndToEndDelay)
    sca_metrics = [clientEndToEndDelay]

    # Filter original data
    def select_f(x, y):
        res = False
        for cond in y:
            res = res | (cond(x))
        return x[res]

    select_vec = select_f(vec_df, vec_metrics)
    select_sca = select_f(sca_df, sca_metrics)

    # Filter only entries that have data in them
    select_vec = select_vec[select_vec["vecvalue"].notna()]

    if "type" in select_vec:
        select_vec = select_vec.drop(["type", "attrname", "attrvalue"], axis=1)
    if "type" in select_sca:
        select_sca = select_sca.drop(["type", "attrname", "attrvalue", "value", "underflows", "overflows"], axis=1)

    return select_sca, select_vec

def saveCsv(sca_df, vec_df, config_name):
    sca_df.to_csv(config_name + "_sca.csv")
    vec_df.to_csv(config_name + "_vec.csv")

def usage():
    print("usage: parseData config_path config_name")

if (__name__ == "__main__"):
    if (len(argv) != 3):
        usage()
    convertToCsv(argv[1], argv[2])
    sca_df, vec_df = openDatasets(argv[2])
    sca_df, vec_df = filterMetrics(sca_df, vec_df)
    saveCsv(sca_df, vec_df, argv[2])
