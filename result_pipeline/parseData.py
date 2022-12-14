#!/bin/python

from sys import argv
from os import mkdir
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

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
        def convertStrToVec(x):
            import math
            if isinstance(x, float) and math.isnan(x):
                return x
            else:
                return np.asarray(list(x.split(" ")), dtype=float)
        df[colname] = df[colname].apply(convertStrToVec)

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


def calcServerAppUtilization(vec_df):
    def calcAppUtilization(throughput_df, channel_capacity, incoming):
        utilization_df = throughput_df.copy()
        utilization_df["vecvalue"] = utilization_df["vecvalue"].apply(lambda x: np.asarray([i/channel_capacity for i in x]))
        if incoming:
            direction = "incoming"
        else:
            direction = "outgoing"
        utilization_df["name"] = direction + "appUtilization" + ":vector"
        return utilization_df

    appLayerThroughput = lambda x: (x["name"].str.contains("DataRate"))
    filterByServer = lambda x: x["module"].str.contains("server")
    filterByIncomingTraffic = lambda x: x["name"].str.contains("incoming")
    # Calculate utilization and append it to vec dataframe
    serverAppThroughputIncoming = vec_df[(appLayerThroughput(vec_df)) & (filterByServer(vec_df)) & (filterByIncomingTraffic(vec_df))]
    utilization_df = calcAppUtilization(serverAppThroughputIncoming, 100e6, incoming=True)
    return pd.concat([utilization_df, vec_df]).sort_index()

def addVecValuesToSca(sca_df):
    # Merge Hists https://stackoverflow.com/questions/47085662/merge-histograms-with-different-ranges
    def extract_vals(hist):
        values = [[y]*int(x) for x, y in zip(hist[0], hist[1])]
        return np.asarray([z for s in values for z in s])

    sca_df["vecvalue"] = sca_df.apply(lambda x: extract_vals((x.binvalues, x.binedges)), axis=1)

def addStatistics(df, colname):
    import numpy as np
    df[colname + "_std"] = df[colname].apply(lambda x: np.std(x))
    df[colname + "_max"] = df[colname].apply(lambda x: np.max(x))
    df[colname + "_min"] = df[colname].apply(lambda x: np.min(x))
    df[colname + "_mean"] = df[colname].apply(lambda x: np.mean(x))
    df[colname + "_avg"] = df[colname].apply(lambda x: np.average(x))

def generatePlots(sca_df, vec_df, config_name):
    try:
        mkdir("plots")
    except OSError as _:
        pass
    # Set filters

    # By metric - This code is duplicated with the other script, a class to do this would be better
    linkLayerThroughput = lambda x: (x["name"] == "txPk:vector(packetBytes)") & (("type" not in x) or (x["type"] == "vector"))
    appLayerUtilization = lambda x: (x["name"].str.contains("Utilization")) & (("type" not in x) or (x["type"] == "vector"))
    appLayerThroughput = lambda x: (x["name"].str.contains("DataRate")) & (("type" not in x) or (x["type"] == "vector")) #  (x["module"].str.contains("Router")) => Use this to filter only router
    clientResponseDelay = lambda x: x["name"] == "timeToResponse"
    serverSentTrainUpdates = lambda x: (x["name"] == "serverSentTrainUpdates")
    serverDroppedTrainUpdates = lambda x: (x["name"] == "serverDroppedTrainUpdates")
    serverReceivedTrainUpdates = lambda x: (x["name"] == "serverReceivedTrainUpdates")
    clientEndToEndDelay = lambda x: (x["name"] == "endToEndDelay:histogram")  & (x["module"].str.contains("client")) & (("type" not in x) or (x["type"] == "histogram"))
    filterByIncomingTraffic = lambda x: x["name"].str.contains("incoming")
    filterByOutgoingTraffic = lambda x: x["name"].str.contains("outgoing")
    # By Module
    filterByClients = lambda x: x["module"].str.contains("client\[")
    filterByClientRouter = lambda x: x["module"].str.contains("clientR")
    filterByTrains = lambda x: x["module"].str.contains("train\[")
    filterByTrainRouter = lambda x: x["module"].str.contains("trainR")
    filterByServer = lambda x: x["module"].str.contains("server")

    linePlots = [
        ("appLayerOutputThroughput_fromClients", 100, vec_df[(appLayerThroughput(vec_df)) & (filterByClients(vec_df)) & (filterByOutgoingTraffic(vec_df))][:1], "Bytes"),
        ("appLayerOutputThroughput_toServers", 100, vec_df[(appLayerThroughput(vec_df)) & (filterByServer(vec_df)) & (filterByIncomingTraffic(vec_df))], "Bytes"),
        ("linkLayerOutputThroughput_fromClients", 0, vec_df[(linkLayerThroughput(vec_df)) & (filterByClients(vec_df))][:1], "Bytes"),
        ("linkLayerOutputThroughput_toServers", 0, vec_df[(linkLayerThroughput(vec_df)) & (filterByServer(vec_df))], "Bytes"),
        ("appLayerUtilization_toServers", 100, vec_df[(appLayerUtilization(vec_df)) & (filterByServer(vec_df))], "%"),
        ("sentTrainUpdates_Server", 0, vec_df[serverSentTrainUpdates(vec_df)], "Nº Updates"),
        ("droppedTrainUpdates_Server", 0, vec_df[serverDroppedTrainUpdates(vec_df)], "Nº Updates"),
        ("receivedTrainUpdates_Server", 0, vec_df[serverReceivedTrainUpdates(vec_df)], "Nº Updates")
    ]

    histPlots = [
        ("clientResponseDelay", vec_df[clientResponseDelay(vec_df)]),
        ("clientEndToEndDelay", sca_df[clientEndToEndDelay(sca_df)])
    ]

    for title, NBins, data, unit in linePlots:
        plotLine(data, NBins, title, unit, config_name)
    
    for title, data in histPlots:
        drawHist(data, title, config_name)


def plotLine(data, N, title, unit, config_name):
    from matplotlib.ticker import EngFormatter, PercentFormatter

    def binData(x, y, N):
        if (N > len(y) or N == 0):
            n_x = x
            n_y = y
        else:
            n_y = np.average(y[:len(y) - len(y) % N].reshape(-1, N), axis=1)
            n_x = x[:len(x) - len(x) % N:N]
            if (len(y) % N) != 0:
                np.append(n_y, np.average(y[len(y) - len(y) % N:-1]))
                np.append(n_x, x[:-1])
        return n_x, n_y

    if isinstance(data, pd.Series):
        x, y = list(), list()
        x.append(data.vectime)
        y.append(data.vecvalue)
    else:
        x = [i.vectime for _, i in data.iterrows()]
        y = [i.vecvalue for _, i in data.iterrows()]

    fig, ax = plt.subplots()
    ax.set_title(title)

    for data_x, data_y in zip(x, y):
        n_x, n_y = binData(data_x, data_y, N)
        formatterx = EngFormatter(unit="s")
        if (unit == "%"):
            formattery = PercentFormatter()
        else:
            formattery = EngFormatter(unit=unit)
        ax.xaxis.set_major_formatter(formatterx)
        ax.yaxis.set_major_formatter(formattery)
        sns.lineplot(x=n_x, y=n_y, ax=ax)

    ax.legend(labels=data[["module"]].to_numpy())
    plt.savefig("plots/" + config_name + "_" + title, dpi=400)


def drawHist(data, title, config_name):
    # Get merged data into np array
    values = np.concatenate(data.vecvalue.to_numpy())
    if np.unique(values).size > 1:
        kde = True
    else:
        kde = False
    
    df = pd.DataFrame(values).melt(var_name='column', value_name='data')
    sns.displot(data=df,
        x="data",
        col="column",
        kde=kde
    ).set(title=title)

    plt.savefig("plots/" + config_name + "_" + title, dpi=400)


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
    sca_df, vec_df = filterNans(sca_df, vec_df)
    sca_df, vec_df = convertValsToList(sca_df, vec_df)
    sca_df, vec_df = filterMetrics(sca_df, vec_df)
    vec_df = calcServerAppUtilization(vec_df)
    addVecValuesToSca(sca_df)
    addStatistics(vec_df, "vecvalue")
    generatePlots(sca_df,  vec_df, argv[2])
    saveCsv(sca_df, vec_df, argv[2])
