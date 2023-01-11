import argparse
import re

def _is_valid_time(s: str):
    date_pattern = re.compile(r'[0-2]\d:[0-5]\d:[0-5]\d')
    if date_pattern.match(s):
        return s
    else:
        raise argparse.ArgumentTypeError('Date must be in format \'00:00:00\'')

def _is_valid_day(s: str):
    days = ['mon', 'tue', 'wed', 'thu', 'fri', 'sat', 'sun']
    if s in days:
        return s.lower()
    else:
        raise argparse.ArgumentTypeError(f'Day must be one of [{" ,".join(days)}]')

def _is_valid_route(s: str):
    route_pattern = re.compile(r'[sS]\d+')
    if route_pattern.match(s):
        return s.upper() 
    else:
        raise argparse.ArgumentTypeError(f'Route must be in format of \'Sx\' (S1, S20, ...)')

def parse():
    parser = argparse.ArgumentParser(
        description = 'Generates a configuration for OMNeT++, based on the real Munich S-Bahn data',
    )

    parser.add_argument('config', type=str, help='name of configuration to be built')
    parser.add_argument('sDay', type=_is_valid_day, help='starting day (mon, tue, ...)')
    parser.add_argument('sTime', type=_is_valid_time, help='starting time (hh:mm:ss)')
    parser.add_argument('eDay', type=_is_valid_day, help='ending day (mon, tue, ...)')
    parser.add_argument('eTime', type=_is_valid_time, help='ending time (hh:mm:ss)')

    parser.add_argument('-r', type=_is_valid_route, dest='routes', action='append', help='Routes to export, ommit for all tracks')

    parser.add_argument('--srcDir', default='muenchen', type=str, help='source directory of data files')
    parser.add_argument('--destDir', default='../proj/simulations/mobility/', type=str, help='destination directory of the configuration')

    parser.add_argument('--timeScale', default=1, type=float, help='will divide by output time scale by TS')

    parser.add_argument('--ix', default=4100, type=int, help='initial x train position')
    parser.add_argument('--iy', default=100, type=int, help='initial y train position')
    parser.add_argument('--fx', default=4300, type=int, help='final x train position')
    parser.add_argument('--fy', default=100, type=int, help='final y train position')

    namespace = parser.parse_args()

    if namespace.routes == None:
        namespace.routes = ['S1', 'S2', 'S3', 'S4', 'S6', 'S7', 'S8', 'S20']

    return namespace


parse()