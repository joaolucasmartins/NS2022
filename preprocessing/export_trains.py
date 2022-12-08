import pandas as pd
import numpy as np

import os
import os.path
SRC_DIR = 'muenchen'
DEST_FILE = 'trains.txt'
BONN_DEST_FILE = 'bonn.txt'

SCALE_FACTOR = 1e5

START_DAY = 'monday'
END_DAY = 'monday'
ROUTES = ['S1', 'S2', 'S3', 'S4', 'S6', 'S7', 'S8', 'S20']
START_T = '00:00:00'
END_T = '26:00:00'


# Import data
routes_cols = {
    'route_long_name': str,
    'route_short_name': str,
    'agency_id': np.int64,
    'route_type': np.int64,
    'route_id': np.int64
}
routes = pd.read_csv(os.path.join(SRC_DIR, 'routes.txt'), dtype=routes_cols)

trips_cols = {
    'route_id': np.int64,
    'service_id': np.int64,
    'direction_id': np.int64,
    'trip_id': np.int64
}
trips = pd.read_csv(os.path.join(SRC_DIR, 'trips.txt'), dtype=trips_cols)

stops_cols = {
    'stop_name': str,
    'stop_id': np.int64,
    'stop_lat': np.float64,
    'stop_lon': np.float64
}
stops = pd.read_csv(os.path.join(SRC_DIR, 'stops.txt'), dtype=stops_cols)

stop_times_cols = {
    'trip_id': np.int64,
    'arrival_time': str,
    'departure_time': str,
    'stop_id': np.int64,
    'stop_sequence': np.int64,
    'pickup_type': np.float64,
    'drop_off_type': np.float64
}
stop_times = pd.read_csv(os.path.join(SRC_DIR, 'stop_times.txt'), dtype=stop_times_cols)

calendar_cols = {
    'monday': np.int64,
    'tuesday': np.int64,
    'wednesday': np.int64,
    'thursday': np.int64,
    'friday': np.int64,
    'saturday': np.int64,
    'sunday': np.int64,
    'start_date': np.int64,
    'end_date': np.int64,
    'service_id': np.int64
}
calendar = pd.read_csv(os.path.join(SRC_DIR, 'calendar.txt'), dtype=calendar_cols)



# Find position values to normalize
min_lon = stops['stop_lon'].min() * SCALE_FACTOR
max_lon = stops['stop_lon'].max() * SCALE_FACTOR
min_lat = stops['stop_lat'].min() * SCALE_FACTOR
max_lat = stops['stop_lat'].max() * SCALE_FACTOR


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

# stops.set_index('stop_id', inplace=True, drop=False)
data = pd.merge(stop_times, trips.loc[:, ['trip_id', 'route_id', 'service_id']], on='trip_id', how='inner')\
    .merge(routes.loc[:, ['route_id', 'route_short_name']], on='route_id', how='inner')\
    .merge(calendar, on='service_id', how='inner')\
    .sort_values(by=['trip_id', 'stop_sequence'])


del stop_times
del trips
del routes
del calendar


days = ['monday', 'tuesday', 'wednesday', 'thursday', 'friday', 'saturday', 'sunday']
cols_to_drop = days.copy() + ['pickup_type', 'drop_off_type', 'service_id', 'route_id', 'start_date', 'end_date']
cols_to_keep = data.columns.drop(cols_to_drop)


# SLOW: Duplicate entries and update values 
new_data = []
for _, row in data.iterrows():
    for off, d in enumerate(days):
        at = parseTime(row['arrival_time'])
        dt = parseTime(row['departure_time'])
        if row[d] == 1:
            new_row = row.drop(cols_to_drop)
            new_row['arrival_time'] = at + off * 3600 * 24
            new_row['departure_time'] = dt + off * 3600 * 24
            new_data.append(new_row.values)

data = pd.DataFrame(new_data, columns=cols_to_keep)
del new_data
data = data.merge(stops.loc[:, ['stop_id', 'stop_lat', 'stop_lon']], how='inner', on='stop_id')
del stops
data.sort_values(['trip_id', 'arrival_time'], inplace=True)



START_T = parseTime(START_T) + days.index(START_DAY)
END_T = parseTime(END_T) + days.index(END_DAY)

trains =  []
# Export train data
old_row, bonnmotion, trip_start_t = None, None, None
for _, row in data.iterrows():
    lat = normalize_lat(row['stop_lat'])
    lon = normalize_lon(row['stop_lon'])

    if row['stop_sequence'] == 0:
        # New trip
        if trip_start_t != None:
            # save previous route, start time, finish time
            trains.append((trip_start_t, old_row['departure_time'] - START_T, old_row['route_short_name'], bonnmotion))
        
        # Reset
        trip_start_t = row['arrival_time'] - START_T
        bonnmotion = []

    # Continuing trip
    if row['arrival_time'] != row['departure_time']:
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
f = open(DEST_FILE, 'w')
bf = open(BONN_DEST_FILE, 'w')
f.write(f'{len(trains)}\n')
for start_time, end_time, route, bonn in trains:
    f.write(f'{start_time} {end_time} {int(route[1:])}\n')
    
    for t, lat, lon in bonn:
        bf.write(f'{t} {lon:.0f} {lat:.0f} ')
    bf.write('\n')

f.close()
bf.close()
