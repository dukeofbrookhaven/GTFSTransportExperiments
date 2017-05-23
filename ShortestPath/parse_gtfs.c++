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
#include <chrono>
#include <stdlib.h>
// #include <filesystem>   File System is still experimental, at least in G++ 4.7.2
#include "parse_gtfs.h"
#include "parse_csv.h"
using namespace std;


string find_required(unordered_map<string, string> &record, string key)
{
    auto pair = record.find(key);
    if ( pair != record.end() ) {
        return pair->second;
    }
    
    cerr << "Unable to find required key " << key << " among the following values: " << endl;
    for ( auto mypair: record ) {
        cerr << "   " << mypair.first << ": " << mypair.second << endl;
    }
    throw missing_value_exception();
}

#define find_with_default(hmap, key, defvalue) ( hmap.find(key) != hmap.end() ? hmap[key] : defvalue )
    

bool load_gtfs_system_data(string gtfs_data_folder,
                           vector<Agency> &agencies,
                           vector<Route> &routes,
                           unordered_map<string, Stop> &stops,
                           vector<Stop_Time> &stop_times,
                           vector<Trip> &trips)
{
    bool rc;
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> process_time;

    //
    // If there were more generic work to be done here, these next 5
    // stanzas would be a good case for object-ification.
    // Unfortunately, the minimum overhead for defining a class would
    // overwhelm the likely savings.

    //
    // Read list of agencies (Perth WA has multiple)
    //
    start = chrono::system_clock::now();
    vector<unordered_map<string, string>> agency_table;
    rc = parse_quoted_csv_file(gtfs_data_folder, "agency.txt", agency_table);
    for ( auto agency_fields : agency_table ) {
        Agency agency;
        agency.id = find_required(agency_fields, "agency_id");
        agency.name = find_with_default(agency_fields, "agency_name", "MISSING");
        agency.email = find_with_default(agency_fields, "agency_email", "Unspecified");
        agencies.push_back(agency);
    }
    end = chrono::system_clock::now();
    process_time = end - start;
    cout << "Read agency data in : " << process_time.count() << " seconds." << endl;
    start = end;
    

    //
    // Read list of routes
    //
    vector<unordered_map<string, string>> route_table;
    rc = parse_quoted_csv_file(gtfs_data_folder, "routes.txt", route_table);
    for ( auto route_fields : route_table ) {
        Route route;
        route.id = find_required(route_fields, "route_id");
        route.short_name = find_with_default(route_fields, "route_short_name", "Unspecified");
        route.long_name = find_with_default(route_fields, "route_long_name", "Unspecified");
        route.desc = find_with_default(route_fields, "route_desc", "Unspecified");
        routes.push_back(route);
    }
    end = chrono::system_clock::now();
    process_time = end - start;
    cout << "Read route data in :  " << process_time.count() << " seconds." << endl;
    start = end;

    //
    // Read list of stops
    //
    vector<unordered_map<string, string>> stop_table;
    rc = parse_quoted_csv_file(gtfs_data_folder, "stops.txt", stop_table);
    for ( auto stop_fields : stop_table ) {
        Stop stop;
        stop.id = find_required(stop_fields, "stop_id");
        stop.code = find_with_default(stop_fields, "stop_code", "Unspecified");
        stop.name = find_with_default(stop_fields, "stop_name", "Unspecified");
        stop.lat = strtod(find_required(stop_fields, "stop_lat").c_str(), nullptr);
        stop.lon = strtod(find_required(stop_fields, "stop_lon").c_str(), nullptr);
        stops[stop.id] = stop;
    }
    end = chrono::system_clock::now();
    process_time = end - start;
    cout << "Read stop data in :   " << process_time.count() << " seconds." << endl;
    start = end;


    //
    // Read list of stop_times
    //
    vector<unordered_map<string, string>> stop_time_table;
    rc = parse_quoted_csv_file(gtfs_data_folder, "stop_times.txt", stop_time_table);
    for ( auto stop_time_fields : stop_time_table ) {
        Stop_Time stop_time;
        stop_time.trip = find_required(stop_time_fields, "trip_id");
        stop_time.arrive = find_with_default(stop_time_fields, "arrival_time", "Unspecified");
        stop_time.depart = find_with_default(stop_time_fields, "departure_time", "Unspecified");
        stop_time.stop = find_required(stop_time_fields, "stop_id");
        stop_time.sequence = strtol(find_with_default(stop_time_fields, "stop_sequence", "0").c_str(), nullptr, 10);
        stop_times.push_back(stop_time);
    }
    end = chrono::system_clock::now();
    process_time = end - start;
    cout << "Read stop times in :  " << process_time.count() << " seconds." << endl;
    start = end;


    //
    // Read list of trips
    //
    vector<unordered_map<string, string>> trip_table;
    rc = parse_quoted_csv_file(gtfs_data_folder, "trips.txt", trip_table);
    for ( auto trip_fields : trip_table ) {
        Trip trip;
        trip.route = find_required(trip_fields, "route_id");
        trip.id = find_required(trip_fields, "trip_id");
        trip.headsign = find_with_default(trip_fields, "trip_headsign", "Unspecified");
        trips.push_back(trip);
    }
    end = chrono::system_clock::now();
    process_time = end - start;
    cout << "Read trip data in :   " << process_time.count() << " seconds." << endl;

    return rc;
}
