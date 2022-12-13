#!/bin/python

from sys import argv
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

def openDatasets(config_name):
    sca = pd.read_csv(config_name + "_sca.csv")
    vec = pd.read_csv(config_name + "_vec.csv")

    # Convert string lists into actual lists
    return sca, vec


def generatePlots(sca_df, vec_df):
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
        ("appLayerOutputThroughput_fromClients", 100, vec_df[(appLayerThroughput(vec_df)) & (filterByClients(vec_df)) & (filterByOutgoingTraffic(vec_df))], "Bytes"),
        ("appLayerOutputThroughput_toServers", 100, vec_df[(appLayerThroughput(vec_df)) & (filterByServer(vec_df)) & (filterByIncomingTraffic(vec_df))], "Bytes"),
        ("linkLayerOutputThroughput_fromClients", 0, vec_df[(linkLayerThroughput(vec_df)) & (filterByClients(vec_df))], "Bytes"),
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

    for title, NBins, data, format_type in linePlots:
        plotLine(data, NBins, title, [format_type])
    
    for title, data in histPlots:
        drawHist(data, title)

    print(sca_df, vec_df)

def plotLine(data, N, label, units):
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
        x.append(np.array(data.vectime))
        y.append(np.array(data.vecvalue))
    else:
        x = [np.array(i.vectime) for _, i in data.iterrows()]
        y = [np.array(i.vecvalue) for _, i in data.iterrows()]

    fig, ax = plt.subplots()
    ax.set_title(label)

    for data_x, data_y, unit in zip(x, y, units):
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


def drawHist(data, title, kde=True):
    # Get merged data into np array
    values = np.concatenate(data.vecvalue.to_numpy())
    
    df = pd.DataFrame(values).melt(var_name='column', value_name='data')
    sns.displot(data=df,
        x="data",
        col="column",
        kde=kde
    ).set(title=title)


def calcAppUtilization(throughput_df, channel_capacity, incoming):
    utilization_df = throughput_df.copy()
    utilization_df["vecvalue"] = utilization_df["vecvalue"].apply(lambda x: [i/channel_capacity for i in x])
    if incoming:
        direction = "incoming"
    else:
        direction = "outgoing"
    utilization_df["name"] = direction + "appUtilization" + ":vector"
    return utilization_df


def usage():
    print("usage: generatePlots config_name")


if (__name__ == "__main__"):
    if (len(argv) != 2):
        usage()
    sca_df, vec_df = openDatasets(argv[1])
    print(vec_df)
    #generatePlots(sca_df, vec_df)