import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

import networkx as nx

import os
import os.path
SRC_DIR = 'muenchen'
OUT_FILE = '../proj/simulations/sbahn/muenchen_sbahn.txt'

SCALE_FACTOR = 1e5

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

print("Lon", min_lon, max_lon)
print("Lat", min_lat, max_lat)

G = nx.Graph()
s = set()
for _, row in stops.iterrows():
    G.add_node(row['stop_id'], 
        name=row['stop_name'],
        pos=(row['stop_lon']*SCALE_FACTOR - min_lon,
        -row['stop_lat']*SCALE_FACTOR + max_lat)
    )
    s.add(row['stop_id'])


data = pd.merge(stop_times, trips.loc[:, ['trip_id', 'route_id', 'service_id']], on='trip_id', how='inner')\
    .merge(routes.loc[:, ['route_id', 'route_short_name']], on='route_id', how='inner')\
    .merge(calendar.loc[:, ['service_id', 'tuesday']], on='service_id', how='inner')\
    .sort_values(by=['trip_id', 'stop_sequence'])

def sbahn_to_enum(route: str) -> int:
    return 1 << int(route[1:])

last_stop = None
for _, row in data.iterrows():
    if not (row['tuesday'] == 1):
        continue

    cur_stop = row['stop_id']

    if row['stop_sequence'] != 0:
        if not G.has_edge(last_stop, cur_stop):
            G.add_edge(last_stop, cur_stop, routes=0)
        G.edges[last_stop, cur_stop]['routes'] |= sbahn_to_enum(row['route_short_name'])

    last_stop = cur_stop


# Hardcoded edges to nuke :)
edges_to_nuke = (
    (7849, 2932),
    (7849, 4086),
    (7036, 7640),
    (7036, 2755),
    (7036, 3784),
    (7036, 4618),
    (1630, 4618),
    (13736, 1018),
    (9045, 7283),
    (6224, 7283),
    (1018, 1754)
)

for a, b in edges_to_nuke:
    if G.has_edge(a, b):
        G.remove_edge(a, b)
    elif G.has_edge(b, a):
        G.remove_edge(b, a)
    else:
        print(f'Error: Edge between {a} and {b} does not exist')

    
print('Nodes:', G.number_of_nodes())
print('Edges:', G.number_of_edges())

f = open(OUT_FILE, 'w')
f.write(f'{(max_lat - min_lat):.0f} {(max_lon - min_lon):.0f}\n')
f.write(f'{G.number_of_nodes()}\n')
for n in G.nodes():
    degree = G.degree[n]
    x = G.nodes[n]['pos'][0]
    y = G.nodes[n]['pos'][1]
    name = G.nodes[n]['name'].replace('ä', 'ae').replace('ü', 'ue').replace('ö', 'oe').replace('ß', 'ss')
    f.write(f'{n} {degree} {x:.0f} {y:.0f} {name}\n')

f.write(f'{G.number_of_edges()}\n')
for start, end, edge_routes in G.edges.data('routes'):
    f.write(f'{start} {end} {edge_routes}\n')
f.close()
