#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <string>
#include <exception>
#include "parse_gtfs.h"

int main(int argc, char **argv)
{
    //
    // This initial function is not MT-safe.
    //
    static struct option gtfs_route_options[]
        {
            {"timebuff", required_argument, 0, 't'},
            {"routebuff", required_argument, 0, 'r'},
            {0, 0, 0, 0}
        };

    int first_mandatory_option = 0;
    int opt;
    double time_buffer { 15.0L };
    double route_buffer { 1000.0L };
    double start_lat, start_lon, dest_lat, dest_lon;
    std::string time_of_day;
    char *remainder;
    std::string gtfs_dir;

    while ((opt = getopt_long(argc, argv, "r:t:",
                              gtfs_route_options, &first_mandatory_option)) != -1) {
        switch(opt) {
        case 'r':
            route_buffer = strtod(optarg, &remainder);
            break;
            
        case 't':
            time_buffer = strtod(optarg, &remainder);
            break;
        }
    }

    //
    // Need Usage() message
    //
    if ( argc - optind != 6 ) {
        std::cerr << "6 Arguments required" << std::endl;
        return -1;
    }

    gtfs_dir = argv[optind++];
    start_lat = strtod(argv[optind++], &remainder);
    start_lon = strtod(argv[optind++], &remainder);
    time_of_day = argv[optind++];
    dest_lat = strtod(argv[optind++], &remainder);
    dest_lon = strtod(argv[optind++], &remainder);
    
    std::cout << "GTFS Data Directory: " << gtfs_dir << std::endl;
    std::cout << "Starting time: " << time_of_day << std::endl;
    std::cout << "Starting point: (" << start_lat << ", " << start_lon << ")." << std::endl;
    std::cout << "Destination: (" << dest_lat << ", " << dest_lon << ")." << std::endl;
    std::cout << "Maximum walking distance: " << route_buffer << " feet" << std::endl;
    std::cout << "Longest delay without risking connection: " << time_buffer << " minutes." << std::endl;

    Agency agency;
    try {
        load_gtfs_system_data(gtfs_dir, agency);
    }
    catch ( const std::exception &e ) {
        std::cerr << "Application failed with Exception: " << e.what() << std::endl;
    }
}
