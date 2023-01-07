import os.path

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


loader = common.GTFSLoader(SRC_DIR)

stops = loader.import_stops()
coord_normalizer = common.CoordNormalizer(stops)
del stops

data = loader.import_denormalized()


days = ['mon', 'tue', 'wed', 'thu', 'fri', 'sat', 'sun']

START_T = loader.parse_time(START_T) + days.index(START_DAY) * 24 * 3600
END_T = loader.parse_time(END_T) + days.index(END_DAY) * 24 * 3600



# Filter out unwanted entries
data.drop(data.loc[(data['departure_time'] < START_T) | (data['arrival_time'] > END_T), :].index, inplace=True)


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

    bf.write(f'0 {INITIAL_X} {INITIAL_Y} ')
    if bonn[-1][0] != 0:
        bf.write(f'{bonn[0][0]} {INITIAL_X} {INITIAL_Y} ')
    for t, lat, lon in bonn:
        bf.write(f'{t} {lon:.0f} {lat:.0f} ')
    bf.write(f'{bonn[-1][0]} {FINAL_X} {FINAL_Y}\n')

    i += 1

f.close()
bf.close()
