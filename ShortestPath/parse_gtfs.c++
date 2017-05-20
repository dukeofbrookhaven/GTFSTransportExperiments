//
// Main file to parse route information in GTFS
//
// This program might be "way cooler" if it parsed the live feed, but this involves:
//   * Many more failure modes (e.g. working TLS-ready connection to the feed)
//   * The possibility that no buses/trains/speedbuggies are in operation
//   * Requires API keys which might vary per transport system
//
// For now, this can be used for static route planning.  Time permitting, we
// can add dynamic checking to see the status of actual vehicles in service.
//

#include <iostream>
#include <fstream>
#include <exception>
#include <map>
#include <unordered_map>
#include <errno.h>
#include <string.h>
// #include <filesystem>   File System is still experimental, at least in G++ 4.7.2
#include "parse_gtfs.h"
#include "parse_csv_line.h"
using namespace std;


//
// Goal 1: Use Maps to make it easy to see how fields in the CSV are exploited
//
// Goal 2 (time permitting): define a custom iterator that loops through CSV lines,
//   thus localizing more of the line-by-line nature of the data
//
const bool load_csv_file(string folder, string filename, unordered_map<string, string> &result)
{
//    string std:filesystem::path agency_file(gtfs_data_folder);
//    agency_file /= "agency.txt";
    string fullpathfilename(folder);
    fullpathfilename.append("/").append(filename);
    ifstream csv_file(fullpathfilename);
    int linenum = 0;
    string line;
    if ( !csv_file.is_open() ) {
        cerr << "Error opening file <" << fullpathfilename << ">: " << strerror(errno) << endl;
        throw missing_input_exception();
    }

    try {
        //
        // Read CSV header line
        //
        if ( getline(csv_file, line) ) {
            linenum++;
            auto field_names = parse_quoted_csv(line);
            while ( getline(csv_file, line) ) {
                linenum++;
                auto split_line = parse_quoted_csv(line);
                auto field_name = field_names.begin();
                for ( auto item : split_line ) {
                    result[*field_name++] = item;
                }
            }
        }
    }
    catch ( const unterminated_quote_exception &uqe ) {
        cerr << "Error in file <" << fullpathfilename << ">, line " << linenum << ", unterminated quote: " << line << endl;
        throw uqe;
    }

    return true;
}

bool load_gtfs_system_data(string gtfs_data_folder, Agency &agency)
{
    unordered_map<string, string> agency_values;
    bool rc = load_csv_file(gtfs_data_folder, "agency.txt", agency_values);

#if 0
    cout << "Map values: " << endl;
    for ( auto mypair: agency_values ) {
        cout << mypair.first << ": " << mypair.second << endl;
    }
#endif

#define find_with_default(hmap, key, defvalue) ( hmap.find(key) != hmap.end() ? hmap[key] : defvalue )
            
    agency.id = find_with_default(agency_values, "agency_id", "MISSING");
    agency.name = find_with_default(agency_values, "agency_name", "MISSING");
    agency.email = find_with_default(agency_values, "agency_email", "Unspecified");

    return rc;
        
//, Map<Route> routes, Map <Stop> stops, Map<Stop_Time> Stops)
}
