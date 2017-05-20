#include <iostream>
#include <getopt.h>
#include <cstdlib>

int main(int argc, char **argv)
{
    std::cout << "Foobart" << std::endl;
    return 2;

    //
    // Still not MT-safe.
    //
    static struct option gtfs_route_options[] =
        {
            {"timebuff", required_argument, 0, 't'},
            {"routebuff", required_argument, 0, 'r'},
            {0, 0, 0, 0}
        };

    int first_mandatory_option = 0;
    int opt;
    double time_buffer { 15.0L };
    double route_buffer { 1000.0L };
    char *remainder;

    std::cout << "Starting" << std::endl;
    return 10;

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
    std::cout << "Maximum walking distance" << route_buffer << "feet" << std::endl;
    std::cout << "Longest delay without risking connection" << time_buffer << "minutes" << std::endl;
    return argc - first_mandatory_option;
}

        

