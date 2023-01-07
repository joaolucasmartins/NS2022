import pandas as pd

import networkx as nx

SRC_DIR = 'muenchen'
OUT_FILE = '../proj/simulations/sbahn/muenchen_sbahn.txt'

import common

loader = common.GTFSLoader(SRC_DIR)

# Import data
routes = loader.import_routes()
trips = loader.import_trips()
stops = loader.import_stops()
stop_times = loader.import_stop_times()
calendar = loader.import_calendar()

coord_normalizer = common.CoordNormalizer(stops)

G = nx.Graph()
s = set()
for _, row in stops.iterrows():
    G.add_node(row['stop_id'], 
        name=row['stop_name'],
        pos=(coord_normalizer.normalize_lon(row['stop_lon']), coord_normalizer.normalize_lat(row['stop_lat']))
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
f.write(f'{coord_normalizer.DIST_X:.0f} {coord_normalizer.DIST_Y:.0f}\n')
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
