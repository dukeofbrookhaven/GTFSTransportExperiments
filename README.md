# Project Title

Transportation Examples

## Getting Started

These are some relatively small (and mostly harmless) programs to exercise various programming concepts and systems.

ShortestPath is a program written in C++ to compute the optimal transportation route from point A to point B using a particular transportation system.  This reads data that complies with the General Transit Feed Specification (GTFS) and uses some STL algorithms to compute the shortest route, using parallelization or even vectorization (depending upon your taste for overkill).

This started as a weekend bench project.  All ridiculous shortcuts are the sole responsibility of this author.

To build this program, run the following command:

...
gcc parse_gtfs.C optimize_route.C main.C -o gtfs_route.exe

To run this program,

...
gtfs_route [--timebuff minutes] [--routebuff feet] <gtfsdir> <source_lat> <source_lon> <timeofday> <dest_lat> <dest_lon>

where:

...
gtfsdir is folder containing location of a transit system data (see Prerequisites)
source_lat, source_lon describes the starting point (in decimal degrees latitude and longitude)
timeofday is the time of day
dest_lat, dest_lon describes your destination point (in decimal degrees latitude and longitude)

...
timebuff provides a buffer in case a given route is behind schedule.  The default is 15.0 minutes, so as long as one cycle is not running later, you should reach your destination within the predicted time.

The route buffer identifies the maximum tolerable distance from your starting location to the transport stop.  The default is 1000 feet

### Prerequisites

You need to download (and unpack) a transit system map in GTFS format, as described in https://www.transitwiki.org/TransitWiki/index.php/Publicly-accessible_public_transportation_data

This program was tested on the Atlanta (MARTA) transport data, and a couple of other systems, but ought to work on anything that complies with the GFTS standard.

## Built With

* [GCC](https://gcc.gnu.org/) - The GNU Compiler Collection

## Authors

* **Miles Duke** - *Initial work* - [TransportRamblings](https://github.com/TransportRamblings)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is copyright 2017.  Please contact the author for redistribution.
