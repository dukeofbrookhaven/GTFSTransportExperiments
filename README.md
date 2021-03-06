# Project Title

GTFS Transportation Experimental Code

## Getting Started

These are some relatively small (and mostly harmless) programs to exercise various programming concepts and systems.

ShortestPath is a program written in bare bones C++ (G++ 11, no database, no boost) to compute the optimal transportation route from point A to point B using a particular transportation system.  This reads data that complies with the General Transit Feed Specification (GTFS) and uses some STL algorithms and other conventions provided by C++ 11.

This started as a weekend bench project, and may reflect some serious shortcuts in the name of expediency, e.g. no Makefiles or serious build management.

To build this program, run the following command:

```
g++ -std=c++11 main.c++ parse_gtfs.c++ -static-libgcc -static-libstdc++ -o gtfs_route.exe
```

or

```
compile.bash
```

To run this program,

```
gtfs_route [--timebuff minutes] [--routebuff feet] [--verbose] <gtfsdir> <source_lat> <source_lon> <timeofday> <dest_lat> <dest_lon>
```

where:

* *gtfsdir* is folder containing transit system data (see Prerequisites)
* *source_lat*, *source_lon* describes the starting point (in decimal degrees latitude and longitude)
* *timeofday* is the time of day in hh:mm format (24-hour day)
* *dest_lat*, *dest_lon* describes your destination point (in decimal degrees latitude and longitude)
* *timebuff* provides a buffer in case a given route is behind schedule.  The default is 15.0 minutes, so as long as one cycle is not running later, you should reach your destination within the predicted time.
* *routebuff* identifies the greatest distanct you would consider walking to reach a given transport stop.  The default is 1000 feet
* *verbose* provides some verbose information (e.g. content of certain system tables) for tracing

There are 2 bash scripts with data points that can exercise this program

* *mariposaCounty.bash* - Uses the 'yosemite-ca-us' transit system data
* *varsityToCentennial.bash* - Uses the MARTA transit system data

### Prerequisites

You need to download (and unpack) a transit system map in GTFS format, as described in https://www.transitwiki.org/TransitWiki/index.php/Publicly-accessible_public_transportation_data

This program was tested on the Atlanta (MARTA) transport data, and a couple of other systems, but is intended to work on anything that complies with the GFTS standard.

## Built With

* [GCC](https://gcc.gnu.org/) - The GNU Compiler Collection gcc.exe (rubenvb-4.7.2-release) on Windows 10

## Authors

* **Miles Duke** - *Initial work* - [GTFSTransportExperiments](https://github.com/dukeofbrookhaven/GTFSTransportExperiments)

## License

This experimental project is copyright 2017, with no real intention to distribute.  If you see an actual useful application, I might be willing to turn IP rights over to an appropriate community.
