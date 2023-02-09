#!/bin/python

from sys import argv
from os import mkdir, path
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

def convertToCsv(config_path, config_name, config_run):
    import subprocess
    p = path.join(config_path, config_name, "-" + config_run)
    cmd = "opp_scavetool x " + p + ".{0} -o {1}_{0}.csv"
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

    #convertVecToList(sca_df, "binedges")
    #convertVecToList(sca_df, "binvalues")
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
    harqErrorRate = lambda x: (x["name"] == "harqErrorRateUl:vector")
    rlcDelay = lambda x: (x["name"] == "rlcDelayUl:vector")
    rlcPacketLoss = lambda x: (x["name"] == "rlcPacketLossTotal:vector")
    distance = lambda x: (x["name"] == "distance:vector")
    vec_metrics = (linkLayerThroughput, appLayerThroughput, clientResponseDelay,
        serverSentTrainUpdates, serverDroppedTrainUpdates, serverReceivedTrainUpdates,
        rlcDelay, rlcPacketLoss, distance, harqErrorRate
    )

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
    #if "type" in select_sca:
        #select_sca = select_sca.drop(["type", "attrname", "attrvalue", "value", "underflows", "overflows"], axis=1)

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

    try:
        mkdir(path.join("plots", config_name))
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
    harqErrorRate = lambda x: (x["name"] == "harqErrorRateUl:vector")
    rlcDelay = lambda x: (x["name"] == "rlcDelayUl:vector")
    rlcPacketLoss = lambda x: (x["name"] == "rlcPacketLossTotal:vector")
    distance = lambda x: (x["name"] == "distance:vector")

    #clientEndToEndDelay = lambda x: (x["name"] == "endToEndDelay:histogram")  & (x["module"].str.contains("client")) & (("type" not in x) or (x["type"] == "histogram"))
    filterByIncomingTraffic = lambda x: x["name"].str.contains("incoming")
    filterByOutgoingTraffic = lambda x: x["name"].str.contains("outgoing")
    # By Module
    filterByClients = lambda x: x["module"].str.contains("client\[")
    filterByClientRouter = lambda x: x["module"].str.contains("clientR")
    filterByTrains = lambda x: x["module"].str.contains("train\[")
    filterByTrainRouter = lambda x: x["module"].str.contains("trainR")
    filterByServer = lambda x: x["module"].str.contains("server")

    linePlots = [
        (("appLayerOutputThroughput_fromClients", "Simulation Time (s)", "Client Application Layer Throughput"),
            100, vec_df[(appLayerThroughput(vec_df)) & (filterByClients(vec_df)) & (filterByOutgoingTraffic(vec_df))][:1], "Bytes/s", None),
        (("appLayerOutputThroughput_toServers", "Simulation Time (s)", "Server Application Layer Throughput"),
            100, vec_df[(appLayerThroughput(vec_df)) & (filterByServer(vec_df)) & (filterByIncomingTraffic(vec_df))], "Bytes/s", None),
        (("linkLayerOutputThroughput_fromClients", "Simulation Time (s)", "Client Link Layer Throughput"),
            0, vec_df[(linkLayerThroughput(vec_df)) & (filterByClients(vec_df))][:1], "Bytes/s", None),
        (("linkLayerOutputThroughput_toServers", "Simulation Time (s)", "Server Link Layer Throughput"),
            0, vec_df[(linkLayerThroughput(vec_df)) & (filterByServer(vec_df))], "Bytes/s", None),
        (("appLayerUtilization_toServers", "Simulation Time (s)", "Client-Server Channel Utilization"),
            100, vec_df[(appLayerUtilization(vec_df)) & (filterByServer(vec_df))], "%", None),
        (("sentTrainUpdates_Server", "Simulation Time (s)", "Number of Sent Train Updates by Server"),
            0, vec_df[serverSentTrainUpdates(vec_df)], "", None),
        (("droppedTrainUpdates_Server", "Simulation Time (s)", "Number of Dropped Train Updates by Server"),
            0, vec_df[serverDroppedTrainUpdates(vec_df)], "", None),
        (("receivedTrainUpdates_Server", "Simulation Time (s)", "Number of Received Train Updates by Server"),
            0, vec_df[serverReceivedTrainUpdates(vec_df)], "", None),
        (("clientDistance", "Simulation Time (s)", "Distance between client and its tower"),
            0, vec_df[distance(vec_df) & filterByClients(vec_df)], "m", ["Random Stationary", "Random Walking", "Following Train"]),
        (("harqErrorRate", "Simulation Time (s)", "Harq error rate of client"),
            0, vec_df[harqErrorRate(vec_df) & filterByClients(vec_df)], "", ["Random Stationary", "Random Walking", "Following Train"]),
        (("rlcDelay", "Simulation Time (s)", "RLC delay of client 0"),
            0, vec_df[rlcDelay(vec_df) & filterByClients(vec_df)], "s", ["Random Stationary", "Random Walking", "Following Train"]),
        (("rlcPacketLoss", "Simulation Time (s)", "RLC Packet loss of client"),
            0, vec_df[rlcPacketLoss(vec_df) & filterByTrains(vec_df)], "", ["Random Stationary", "Random Walking", "Following Train"])
    ]

    histPlots = [
        (("clientResponseDelay", "Simulation Time", "Client-Sever Response Delay"),
            vec_df[clientResponseDelay(vec_df)]),
        #("clientEndToEndDelay", sca_df[clientEndToEndDelay(sca_df)])
    ]

    for text, NBins, data, unit, legend in linePlots:
        plotLine(text, NBins, data, unit, config_name, legend=legend)

    for text, data in histPlots:
        drawHist(text, data, config_name)


def plotLine(text_info, N, data, unit, config_name, legend=None):
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

    title, x_text, y_text = text_info

    fig, ax = plt.subplots()

    for data_x, data_y in zip(x, y):
        n_x, n_y = binData(data_x, data_y, N)
        if (unit == "%"):
            formattery = PercentFormatter()
            ax.yaxis.set_major_formatter(formattery)
        elif (unit != ""):
            formattery = EngFormatter(unit=unit)
            ax.yaxis.set_major_formatter(formattery)
        sns.lineplot(x=n_x, y=n_y, ax=ax)

    if (legend == "Module"):
        legend = data[["module"]].to_numpy()
    if (legend is not None):
        ax.legend(labels=legend)
    ax.set_xlabel(x_text)
    ax.set_ylabel(y_text)

    plt.savefig(path.join("plots", config_name, config_name + "_" + title),
        bbox_inches='tight', dpi=400)


def drawHist(text_info, data, config_name):
    # Get merged data into np array
    values = np.concatenate(data.vecvalue.to_numpy())
    values.sort()

    q25, q75 = np.percentile(values, [25, 75])
    bin_width = 2 * (q75 - q25) * len(values) ** (-1/3)
    bins = round((values.max() - values.min()) / bin_width)

    if np.unique(values).size > 1:
        kde = True
    else:
        kde = False

    title, x_text, y_text = text_info
    sns.displot(values, bins=7)

    plt.savefig(path.join("plots", config_name, config_name + "_" + title),
        bbox_inches='tight', dpi=400)

def saveCsv(sca_df, vec_df, config_name):
    sca_df.to_csv(config_name + "_sca.csv")
    vec_df.to_csv(config_name + "_vec.csv")

def usage():
    print("usage: parseData config_path config_name config_run")

if (__name__ == "__main__"):
    if (len(argv) != 4):
        usage()
    convertToCsv(argv[1], argv[2], argv[3])
    sca_df, vec_df = openDatasets(argv[2])
    sca_df, vec_df = filterNans(sca_df, vec_df)
    sca_df, vec_df = convertValsToList(sca_df, vec_df)
    sca_df, vec_df = filterMetrics(sca_df, vec_df)
    vec_df = calcServerAppUtilization(vec_df)
    #addVecValuesToSca(sca_df)
    addStatistics(vec_df, "vecvalue")
    generatePlots(sca_df,  vec_df, argv[2])
    saveCsv(sca_df, vec_df, argv[2])
