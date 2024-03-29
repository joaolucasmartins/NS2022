import os.path
import math

import numpy as np

import export_trains_argparse
import common

args = export_trains_argparse.parse()

SRC_DIR = args.srcDir
DEST_DIR = args.destDir

CONFIG_NAME = args.config

DEST_FILE = CONFIG_NAME + '.ini'
BONN_DEST_FILE = CONFIG_NAME + '.movements'

START_DAY = args.sDay
START_T = args.sTime
END_DAY = args.eDay
END_T = args.eTime

ROUTES = args.routes


INITIAL_X = args.ix
INITIAL_Y = args.iy

FINAL_X = args.fx
FINAL_Y = args.fy

TIME_SCALE = args.timeScale


loader = common.GTFSLoader(SRC_DIR)

stops = loader.import_stops()
coord_normalizer = common.CoordNormalizer(stops)
del stops

data = loader.import_denormalized()


days = ['mon', 'tue', 'wed', 'thu', 'fri', 'sat', 'sun']

START_T = loader.parse_time(START_T) + days.index(START_DAY) * 24 * 3600
END_T = loader.parse_time(END_T) + days.index(END_DAY) * 24 * 3600

BUFFER_T = 10 * 60

# Filter out unwanted entries
data.drop(data.loc[(data['departure_time'] < START_T) | (data['arrival_time'] > END_T + BUFFER_T), :].index, inplace=True)
data.drop(data.loc[~(data['route_short_name'].isin(ROUTES)), :].index, inplace=True)


# Apply time scale
START_T = math.floor(START_T / TIME_SCALE)
END_T = math.ceil(END_T / TIME_SCALE)

data['arrival_time'] =  (data.loc[:, 'arrival_time'] / TIME_SCALE).astype(np.int64)
data['departure_time'] =  (data.loc[:, 'departure_time'] / TIME_SCALE).astype(np.int64)



trains =  []
# Export train data
old_row = {'trip_id': None}
bonnmotion, trip_start_t = None, None
for _, row in data.iterrows():
    lat, lon = coord_normalizer.normalize(row)

    if row['trip_id'] != old_row['trip_id']:
        # New trip
        if trip_start_t != None:
            # save previous route, start time, finish time
            trains.append((trip_start_t, old_row['departure_time'] - START_T, old_row['route_short_name'], bonnmotion))
        
        # Reset
        trip_start_t = max(0, row['arrival_time'] - START_T)
        bonnmotion = []

        if (trip_start_t < 0):
            print(row['trip_id'], row['arrival_time'], START_T)

    # Continuing trip
    if row['arrival_time'] != row['departure_time'] and row['arrival_time'] >= START_T:
        bonnmotion.append((row['arrival_time'] - START_T, lat, lon))
    bonnmotion.append((row['departure_time'] - START_T, lat, lon))

    old_row = row

# Save last trip
if trip_start_t != None:
    # save previous route, start time, finish time
    trains.append((trip_start_t, old_row['departure_time'] - START_T, old_row['route_short_name'], bonnmotion))

# Remove trains with start time equal to end time
trains = [t for t in trains if t[0] != t[1]]

# Sort by start time
trains.sort(key=lambda row: row[0])

# Logging
print(f'Generated config {CONFIG_NAME} with {len(trains)} trains')

# Export to file
f = open(os.path.join(DEST_DIR, DEST_FILE), 'w')
bf = open(os.path.join(DEST_DIR, BONN_DEST_FILE), 'w')
f.write(f'*.nTrains = {len(trains)}\n')
f.write(f'*.train[*].mobility.traceFile = "mobility/{BONN_DEST_FILE}"\n\n')
f.write(f'sim-time-limit = {END_T - START_T}s\n\n')

i = 0
for start_time, end_time, route, bonn in trains:
    # f.write(f'{start_time} {end_time} {int(route[1:])}\n')
    f.write(f'*.train[{i}].trackId = {int(route[1:])}\n')
    f.write(f'*.train[{i}].app[0].startTime = {start_time}s\n')
    f.write(f'*.train[{i}].app[0].stopTime = {end_time}s\n')


    if bonn[0][0] != 0:
        bf.write(f'0 {INITIAL_X} {INITIAL_Y} ')
        bf.write(f'{bonn[0][0]} {INITIAL_X} {INITIAL_Y} ')
    for t, lat, lon in bonn:
        bf.write(f'{t} {lon:.0f} {lat:.0f} ')
    bf.write(f'{bonn[-1][0]} {FINAL_X} {FINAL_Y}\n')

    i += 1

f.close()
bf.close()
