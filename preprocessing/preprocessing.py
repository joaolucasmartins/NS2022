#!/usr/bin/env python
# coding: utf-8


# In[1]:
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

import os
import os.path
import shutil
SRC_DIR = 'original'
DEST_DIR = 'muenchen'

import common

loader = common.GTFSLoader(SRC_DIR)
# In[2]:
routes = loader.import_routes()
trips = loader.import_trips()
agency = loader.import_agency()
stops = loader.import_stops()
stop_times = loader.import_stop_times()
routes = loader.import_routes()
calendar = loader.import_calendar()


# In[9]:

routes_sbahn = routes.loc[
    (routes['agency_id'] == 73) &
    (routes['route_long_name'].str.startswith('S')) &
    ~(routes['route_long_name'].str.fullmatch('S60')) &
    ~(routes['route_long_name'].str.fullmatch('S5'))
]
del routes
routes_sbahn

# In[10]:


trips_sbahn = pd.merge(routes_sbahn.loc[:, 'route_id'], trips, on='route_id', how='inner')
del trips
trips_sbahn


# In[11]:


stop_times_sbahn = pd.merge(trips_sbahn.loc[:, 'trip_id'], stop_times, on='trip_id', how='inner')
del stop_times
stop_times_sbahn


# In[12]:


stops_sbahn = pd.merge(stop_times_sbahn.loc[:, 'stop_id'].drop_duplicates(), stops, on='stop_id', how='inner')
del stops
stops_sbahn


# ## Remove trips/routes outside of Munich
# 
# The dataset contains routes in Nurnbern and Austria, which we want to filter out

# In[14]:
stops_to_drop = stops_sbahn.loc[(stops_sbahn['stop_lat'] < 47.8) | (stops_sbahn['stop_lat'] > 49)].copy()
stops_sbahn.drop(stops_to_drop.index, inplace=True)


# Now to propagate throughout the other dataframes...  
# We only wish to keep trips that are exclusively in Munich

# In[15]:


trips_to_drop = pd.merge(
    stops_to_drop.loc[:, 'stop_id'],
    stop_times_sbahn.loc[:, ['stop_id', 'trip_id']],
    how='inner',
    on='stop_id'
).loc[:, 'trip_id'].drop_duplicates()
print(len(trips_to_drop.index), 'trips to be deleted')


# In[16]:
trips_sbahn.drop(trips_sbahn.loc[trips_sbahn['trip_id'].isin(trips_to_drop)].index, inplace=True)


# In[17]:
services_to_keep = trips_sbahn['service_id'].drop_duplicates()
print(f'Removed {len(services_to_keep.index)} services')
calendar.drop(calendar.loc[~calendar['service_id'].isin(services_to_keep)].index, inplace=True)


# In[18]:


del stops_to_drop
del trips_to_drop
del services_to_keep


# # Cluster stops
# In[20]:
stops_sbahn['stop_name'] = stops_sbahn.loc[:, 'stop_name']\
    .str.removeprefix('München')\
    .str.removesuffix('(Oberbay)')\
    .str.strip(' -')


# In[21]:
# Manual overrides of odd station names
stops_sbahn.loc[stops_sbahn['stop_name'] == 'Ost', 'stop_name'] = 'Ostbahnhof'
stops_sbahn.loc[stops_sbahn['stop_name'] == 'Hbf Gl.27-36', 'stop_name'] = 'Hbf'
stops_sbahn.loc[stops_sbahn['stop_name'] == 'Hbf (tief)', 'stop_name'] = 'Hbf'
stops_sbahn.loc[stops_sbahn['stop_name'] == 'Schwabhausen(b Dachau)', 'stop_name'] = 'Schwabhausen'
stops_sbahn.loc[stops_sbahn['stop_name'] == 'Altomünster, Bahnhof', 'stop_name'] = 'Altomünster'


# In[22]:
CLUSTER_RANGE = 0.0055

class Cluster:
    def __init__(self) -> None:
        self.name = None
        self.id = -1
        self.lon = 0
        self.lat = 0
        self.stop_ids = set()
    
    def belongs(self, row: pd.Series):
        if self.name == 'Baierbrunn' and row['stop_name'] == 'Baierbrunn':
            return True

        return abs(self.lon - row['stop_lon']) <= CLUSTER_RANGE             and abs(self.lat - row['stop_lat']) <= CLUSTER_RANGE

    def add(self, index: int, row: pd.Series) -> None:
        n_members = len(self.stop_ids)

        self.lon = ((self.lon * n_members) + row['stop_lon']) / (n_members + 1)
        self.lat = ((self.lat * n_members) + row['stop_lat']) / (n_members + 1)

        name = row['stop_name']
        if self.name == None:
            self.id = index
            self.name = name
        elif self.name != name:
            print(f'Conflicting stop names "{name}" [{index}]  and "{self.name}" [{self.id}]')

        self.stop_ids.add(index)
    
    def get_dict(self) -> dict:
        d = { self.id: self.id }
        for stop in self.stop_ids:
            d[stop] = self.id
        return d


# In[23]:
st = stops_sbahn.set_index('stop_id')

clusters = []
for index, row in st.iterrows():

    cluster = None
    # Try to find match in existing cluster    
    for c in clusters:
        if c.belongs(row):
            if cluster == None:
                if c.name == row['stop_name']:
                    cluster = c
                else:
                    print(f'"{row["stop_name"]}" Joined cluster: "{c.name}"')
            else:
                if row['stop_name'] == c.name:
                    cluster = c
                elif row['stop_name'] == cluster.name:
                    cluster = cluster
                else:
                    print(f'"{row["stop_name"]}" Could fit multiple clusters: "{c.name}" and "{cluster.name}"')

 
    # If no cluster found, create another
    if cluster == None:
        cluster = Cluster()
        clusters.append(cluster)
    
    cluster.add(index, row)

# Create map of old -> new keys
# Build new dataframe from clusters
stop_update_map = dict()
data = []
for c in clusters:
    stop_update_map.update(c.get_dict())
    data.append((c.id, c.name, c.lon, c.lat))
stops_sbahn = pd.DataFrame(data=data, columns=['stop_id', 'stop_name', 'stop_lon', 'stop_lat'])

del st

print(f'Clustering resulted in {len(stops_sbahn.index)} stops')

# ### Propagate the new clusters to the stop_times dataframe

# In[25]:
for old, new in stop_update_map.items():
    if old != new:
        stop_times_sbahn.loc[stop_times_sbahn['stop_id'] == old, 'stop_id'] = new
del stop_update_map


# # Generate denormalized data

# In[26]:
def parseTime(s: str) -> int:
    return int(s[0:2]) * 3600 + int(s[3:5]) * 60 + int(s[6:8])


# In[27]:
data = pd.merge(stop_times_sbahn, trips_sbahn.loc[:, ['trip_id', 'route_id', 'service_id']], on='trip_id', how='inner')    .merge(routes_sbahn.loc[:, ['route_id', 'route_short_name']], on='route_id', how='inner')    .merge(calendar, on='service_id', how='inner')    .sort_values(by=['trip_id', 'stop_sequence'])

days = ['monday', 'tuesday', 'wednesday', 'thursday', 'friday', 'saturday', 'sunday']
cols_to_drop = days.copy() + ['pickup_type', 'drop_off_type', 'service_id', 'route_id', 'start_date', 'end_date', 'pickup_type', 'drop_off_type', 'service_id']
cols_to_keep = data.columns.drop(cols_to_drop)


# In[28]:


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
data = data.merge(stops_sbahn.loc[:, ['stop_id', 'stop_lat', 'stop_lon']], how='inner', on='stop_id')
data.sort_values(['trip_id', 'arrival_time'], inplace=True)


# # Export the data back out

# In[29]:


os.makedirs(DEST_DIR, exist_ok=True)

stops_sbahn.to_csv(os.path.join(DEST_DIR, 'stops.txt'), index=False, float_format="%.6f")
stop_times_sbahn.to_csv(os.path.join(DEST_DIR, 'stop_times.txt'), index=False)
trips_sbahn.to_csv(os.path.join(DEST_DIR, 'trips.txt'), index=False)
routes_sbahn.to_csv(os.path.join(DEST_DIR, 'routes.txt'), index=False)
calendar.to_csv(os.path.join(DEST_DIR, 'calendar.txt'), index=False)


# In[30]:


data.to_csv(os.path.join(DEST_DIR, 'denormalized.csv'), index=False)


# In[31]:


shutil.copy(src=os.path.join(SRC_DIR, 'calendar_dates.txt'),
    dst=os.path.join(DEST_DIR, 'calendar_dates.txt'));

