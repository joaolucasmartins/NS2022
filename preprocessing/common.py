from __future__ import annotations
from typing import Tuple

import os.path
import math

import pandas as pd
import numpy as np

class GTFSLoader:

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

    stop_times_cols = {
        'trip_id': np.int64,
        'arrival_time': str,
        'departure_time': str,
        'stop_id': np.int64,
        'stop_sequence': np.int64,
        'pickup_type': np.float64,
        'drop_off_type': np.float64
    }

    stops_cols = {
        'stop_name': str,
        'stop_id': np.int64,
        'stop_lat': np.float64,
        'stop_lon': np.float64
    }

    agency_cols = {
        'agency_id': np.int64,
        'agency_name': str,
        'agency_url': str,
        'agency_timezone': str,
        'agency_lang': str
    }

    trips_cols = {
        'route_id': np.int64,
        'service_id': np.int64,
        'direction_id': np.int64,
        'trip_id': np.int64
    }

    routes_cols = {
        'route_long_name': str,
        'route_short_name': str,
        'agency_id': np.int64,
        'route_type': np.int64,
        'route_id': np.int64
    }

    denormalized_cols = {
        'trip_id': np.int64,
        'arrival_time': np.int64,
        'departure_time': np.int64,
        'stop_id': np.int64,
        'stop_sequence': np.int64,
        'route_short_name': str,
        'stop_lat': np.float64,
        'stop_lon': np.float64
    }

    def __init__(self, src_dir: str) -> None:
        self.SRC_DIR = src_dir

    def import_calendar(self) -> pd.DataFrame:
        return pd.read_csv(os.path.join(self.SRC_DIR, 'calendar.txt'), dtype=self.calendar_cols)

    def import_stop_times(self) -> pd.DataFrame:
        return pd.read_csv(os.path.join(self.SRC_DIR, 'stop_times.txt'), dtype=self.stop_times_cols)

    def import_stops(self) -> pd.DataFrame:
        return pd.read_csv(os.path.join(self.SRC_DIR, 'stops.txt'), dtype=self.stops_cols)

    def import_agency(self) -> pd.DataFrame:
        return pd.read_csv(os.path.join(self.SRC_DIR, 'agency.txt'), dtype=self.agency_cols)

    def import_trips(self) -> pd.DataFrame:
        return pd.read_csv(os.path.join(self.SRC_DIR, 'trips.txt'), dtype=self.trips_cols)
    
    def import_routes(self) -> pd.DataFrame:
        return pd.read_csv(os.path.join(self.SRC_DIR, 'routes.txt'), dtype=self.routes_cols)

    def import_denormalized(self) -> pd.DataFrame:
        return pd.read_csv(os.path.join(self.SRC_DIR, 'denormalized.csv'), dtype=self.denormalized_cols)

    @staticmethod
    def parse_time(s: str) -> int:
        return int(s[0:2]) * 3600 + int(s[3:5]) * 60 + int(s[6:8])



class CoordNormalizer:

    # Hardcoded values, distances in meters
    DIST_X = 69.42e3 / 10
    DIST_Y = 58.85e3 / 10

    def __init__(self, stops: pd.DataFrame) -> None:
        self.__config_locations(stops)

    def __config_locations(self, stops: pd.DataFrame):
        self.min_lon = stops['stop_lon'].min()
        self.max_lon = stops['stop_lon'].max()
        self.min_lat = stops['stop_lat'].min()
        self.max_lat = stops['stop_lat'].max()
        
        self.X_SCALE_FACTOR = self.DIST_X / (self.max_lon - self.min_lon)
        self.Y_SCALE_FACTOR = self.DIST_Y / (self.max_lat - self.min_lat)

    def normalize(self, data: pd.Series | iter) -> Tuple:
        if isinstance(data, pd.Series):
            return (self.normalize_lat(data['stop_lat']), self.normalize_lon(data['stop_lon']))
        else:
            return (self.normalize_lat(data[0]), self.normalize_lon(data[1]))

    def normalize_lat(self, lat: float) -> float:
        return (self.max_lat - lat) * self.Y_SCALE_FACTOR

    def normalize_lon(self, lon: float) -> float:
        return (lon - self.min_lon) * self.X_SCALE_FACTOR



