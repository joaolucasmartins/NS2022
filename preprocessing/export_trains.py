import pandas as pd
import numpy as np

import os
import os.path
SRC_DIR = 'muenchen'
DEST_DIR = '../proj/simulations/mobility/'
DEST_FILE = 'train_config.ini'
BONN_DEST_FILE = 'bonn.movements'

SCALE_FACTOR = 1e5

START_DAY = 'monday'
END_DAY = 'monday'
ROUTES = ['S1', 'S2', 'S3', 'S4', 'S6', 'S7', 'S8', 'S20']
START_T = '07:00:00'
END_T = '08:00:00'

INITIAL_X = 1000
INITIAL_Y = 200

FINAL_X = 1200
FINAL_Y = 200

stops_cols = {
    'stop_name': str,
    'stop_id': np.int64,
    'stop_lat': np.float64,
    'stop_lon': np.float64
}
stops = pd.read_csv(os.path.join(SRC_DIR, 'stops.txt'), dtype=stops_cols)


data_cols = {
    'trip_id': np.int64,
    'arrival_time': np.int64,
    'departure_time': np.int64,
    'stop_id': np.int64,
    'stop_sequence': np.int64,
    'route_short_name': str,
    'stop_lat': np.float64,
    'stop_lon': np.float64
}
data = pd.read_csv(os.path.join(SRC_DIR, 'denormalized.csv'), dtype=data_cols)


# Find position values to normalize
min_lon = stops['stop_lon'].min() * SCALE_FACTOR
max_lon = stops['stop_lon'].max() * SCALE_FACTOR
min_lat = stops['stop_lat'].min() * SCALE_FACTOR
max_lat = stops['stop_lat'].max() * SCALE_FACTOR

del stops

def parseTime(s: str) -> int:
    return int(s[0:2]) * 3600 + int(s[3:5]) * 60 + int(s[6:8])

def normalize_lat(lat: float) -> float:
    global min_lat
    global max_lat
    return -lat*SCALE_FACTOR + max_lat

def normalize_lon(lon: float) -> float:
    global min_lon
    global max_lon
    return lon*SCALE_FACTOR - min_lon

days = ['monday', 'tuesday', 'wednesday', 'thursday', 'friday', 'saturday', 'sunday']

START_T = parseTime(START_T) + days.index(START_DAY) * 24 * 3600
END_T = parseTime(END_T) + days.index(END_DAY) * 24 * 3600



# Filter out unwanted entries
data.drop(data.loc[(data['departure_time'] < START_T) | (data['arrival_time'] > END_T), :].index, inplace=True)


trains =  []
# Export train data
old_row = {'trip_id': None}
bonnmotion, trip_start_t = None, None
for _, row in data.iterrows():
    lat = normalize_lat(row['stop_lat'])
    lon = normalize_lon(row['stop_lon'])

    if row['trip_id'] != old_row['trip_id']:
        # New trip
        if trip_start_t != None:
            # save previous route, start time, finish time
            trains.append((trip_start_t, old_row['departure_time'] - START_T, old_row['route_short_name'], bonnmotion))
        
        # Reset
        trip_start_t = row['arrival_time'] - START_T
        bonnmotion = []

    # Continuing trip
    if row['arrival_time'] != row['departure_time'] and row['arrival_time'] >= START_T:
        bonnmotion.append((row['arrival_time'] - START_T, lat, lon))
    bonnmotion.append((row['departure_time'] - START_T, lat, lon))

    old_row = row

# Save last trip
if trip_start_t != None:
    # save previous route, start time, finish time
    trains.append((trip_start_t, old_row['departure_time'] - START_T, old_row['route_short_name'], bonnmotion))

# Sort by start time
trains.sort(key=lambda row: row[0])


# Export to file
f = open(os.path.join(DEST_DIR, DEST_FILE), 'w')
bf = open(os.path.join(DEST_DIR, BONN_DEST_FILE), 'w')
f.write(f'*.nTrains = {len(trains)}\n\n')
i = 0
for start_time, end_time, route, bonn in trains:
    # f.write(f'{start_time} {end_time} {int(route[1:])}\n')
    f.write(f'*.train[{i}].trackId = {int(route[1:])}\n')
    f.write(f'*.train[{i}].app[0].startTime = {start_time}s\n')
    f.write(f'*.train[{i}].app[0].stopTime = {end_time}s\n')

    bf.write(f'0 {INITIAL_X} {INITIAL_Y} ')
    if bonn[-1][0] != 0:
        bf.write(f'{bonn[0][0]} {INITIAL_X} {INITIAL_Y} ')
    for t, lat, lon in bonn:
        bf.write(f'{t} {lon:.0f} {lat:.0f} ')
    bf.write(f'{bonn[-1][0]} {FINAL_X} {FINAL_Y}\n')

    i += 1

f.close()
bf.close()
