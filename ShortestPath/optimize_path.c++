#define _XOPEN_SOURCE 700
#undef __STRICT_ANSI__
#include <iostream>
#include <vector>
#include <set>
#include <math.h>
#include <ctime>
#include <algorithm>
#include <chrono>
#include "parse_gtfs.h"
#include "optimize_path.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

//
// Stop-to-stop distances computed using the following Haversine code
//    Source: https://rosettacode.org/wiki/Haversine_formula#C
//

//
// Mean earth radius, in kilometers when we're verifying
// the results of haversine, but in feet for every day use
//
#ifdef TEST_ROSETTA
#define R 6371
#else
#define R 6371 * 3280.84
#endif

#define TO_RAD (3.1415926536 / 180)
double dist_feet(double th1, double ph1, double th2, double ph2)
{
	double dx, dy, dz;
	ph1 -= ph2;
	ph1 *= TO_RAD, th1 *= TO_RAD, th2 *= TO_RAD;
 
	dz = sin(th1) - sin(th2);
	dx = cos(ph1) * cos(th1) - cos(th2);
	dy = sin(ph1) * cos(th1);
	return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * R;
}

#ifdef TEST_ROSETTA
//
// These
//
int main()
{
	double d = dist(36.12, -86.67, 33.94, -118.4);
	printf("dist: %.1f km (%.1f mi.)\n", d, d / 1.609344);

	d = dist(33.9, -84.35, 34.0, -81.07);
	printf("atl to cola: %.1f km (%.1f mi.)\n", d, d / 1.609344);

	d = dist(33.9, -84.35, 36.2, -81.67);
	printf("atl to boone: %.1f km (%.1f mi.)\n", d, d / 1.609344);

	d = dist(34.0, -81.07, 36.2, -81.67);
	printf("cola to boone: %.1f km (%.1f mi.)\n", d, d / 1.609344);
 
	return 0;
}
#endif

#ifndef STRPTIME_AVAILABLE
//
// I couldn't find strptime or any suitable alternative in MinGW
// 4.7.2, so created a limited version.  Hopefully, this will
// fail to compile if there is a real strptime() available
//
extern "C"
{
    char *strptime(const char *time_of_day, const char *format, struct tm *tm)
    {
        char *remainder;
        int hours, minutes;
        time_t now;

        hours = strtol(time_of_day, &remainder, 10);
        if ( hours < 0 || hours > 24 || remainder == (char *) NULL || *remainder != ':' )
            return NULL;

        remainder++; // Skip over ':'
        minutes = strtol(remainder, &remainder, 10);
        if ( minutes < 0 || minutes > 60 )
            return NULL;
        
        if ( tm != (struct tm *) NULL ) {
            time(&now);
            localtime_s(tm, &now);
            tm->tm_hour = hours;
            tm->tm_min = minutes;
        }

        return remainder;
    }
}
#endif    

long difftime_in_minutes(string a, string b)
{
    struct tm atm, btm;
    time_t atime, btime;
    double diff_in_secs;

    if ( strptime(a.c_str(), "%r", &atm) == NULL ) {
        throw invalid_time_stamp();
    }
    if ( strptime(b.c_str(), "%r", &btm) == NULL ) {
        throw invalid_time_stamp();
    }
    
    atime = mktime(&atm);
    btime = mktime(&btm);
    diff_in_secs = difftime(btime, atime);
    
    return ceil(diff_in_secs / 60.0);
}


//
// PATH OPTIMIZATION
// 
// Search for paths from source to dest, where each path is a set of "legs":
// 
//   -- a leg is a pair of stop_times on the same trip
//   -- between legs in the path:
//        - there is no more than distance-threshold walking
//        - there is enough time to cover walking from leg to leg, plus the delay-threshhold
//   -- a path with "no legs" is a special case of walking directly to destination;
//   -- paths are ranked based upon total path time, including an estimated
//      time for walking to, from, and between the legs
//   -- the zero-leg path might be only (or best) alternative available
//
//
// BRUTE FORCE APPROACH
//        
// A "general" algorithm would just start with the stops, and
// generate every possible path from source to dest.  This is
// appealing for systems with very complicated topographies, but it
// would be very resource-intensive.
//
// CONSIDERED APPROACH
// 
// We're going to focus on reducing the combinations by assuming the
// system is a set of interconnected "primary" trips, where each stop
// on the primary trips is a hub with 1 or more secondary trips
// radiating outward.  Viable paths involve visiting no more than 2 of
// these hubs (and therefore no more than 2 secondary trips).
//
// This also simplifies the complexity of the paths proposed, even if
// "brute force" somehow found an odd mesh of secondary trips that had
// less travel time.
// 
// STRATEGY
//
//   1. Find the trips that stop within distance threshold of source or dest
//   2. From these trips, find other trips which intersect with them
//      a. This search can terminate once you find a "hub".
//   3. Generate viable paths covering source trips that intersect with dest trips
//   4. Rank the paths by "time consumed"
//
// OTHER SIMPLIFYING ASSUMPTIONS, in decreasing order of perceived risk
//
//   (1) Routes are aggregates of trips defined by system, and are not
//       needed to compute efficient paths
//       
//   (2) Consider only paths where final leg is reached before midnight,
//       so we don't have to handle "wrapping around" to tomorrows
//       schedule, and the odd timestamp maths that would entail.
//       
//   (3) All published trips happen every day -- therefore
//       day-of-week has no influence on path selection
//
//   (4) Using time_of_day to constrain searches.  We don't need to
//       consider stops that have already completed by the time our
//       hypothetical traveler reaches a certain stop
//

//
// Find stops within distance of a target point
//
int add_stops_within_distance(unordered_map<string, Stop> &stops, double lat, double lon, double distance,
                               /* in/out */ set<string> &stops_within_distance)
{
    int count_before = stops_within_distance.size();

    //
    // Accumulate a list of unique stops within a certain distance of
    // a point.
    //
    // Comparing the various incantations of for_each, and the
    // exercise of capturing the correct type of references for the
    // lambda, I have to say the code with range-based for is much
    // more readable.  The payoff might be there for other algorithms,
    // or for parallelized algorithms in C++ 17.
    //
#if 0
    for_each(stops.begin(), stops.end(), [&stops_within_distance,lat,lon,distance](const pair <const string, Stop> &stop_key_value) {
    //for_each(stops.begin(), stops.end(), [&stops_within_distance,lat,lon,distance](const std::unordered_map<string,Stop>::value_type &stop_key_value) {
            if ( dist_feet(lat, lon, stop_key_value.second.lat, stop_key_value.second.lon) < distance ) {
                stops_within_distance.insert(stop_key_value.second.id);
            }
        });
#else
    // for ( const pair <const string, Stop> stop_key_value : stops ) {
    for (const auto stop_key_value: stops) {
        if ( dist_feet(lat, lon, stop_key_value.second.lat, stop_key_value.second.lon) < distance ) {
            stops_within_distance.insert(stop_key_value.second.id);
        }
    }
#endif
            

    return stops_within_distance.size() - count_before;
}

int add_trips_containing_stop(vector<Trip> &trips, vector<Stop_Time> &stop_times,
                              const string &stop_id, const string *after_time_of_day, long time_box,
                              /* in/out */ set<string> &trips_containing_stop)
{
    int count_before = trips_containing_stop.size();
    struct tm tm;
    
    int trips_later_today = 0;
    cout << "Searching for trips that hit stop " << stop_id << ":" << endl;
    for ( auto stop_time: stop_times ) {
        if ( stop_id == stop_time.stop ) {
            //
            // Validate arrival time.  Must be valid time stamp, and within
            // appropriate range of optional time-of-day constraint
            //
            try {
                //
                // Skip trips that have already happened today, and
                // those arriving outside the specified time box
                //
                if ( after_time_of_day ) {
                    int minutes_away = difftime_in_minutes(*after_time_of_day, stop_time.arrive);
                    if ( minutes_away < 0 || minutes_away > time_box ) {
                        continue;
                    }
                }
                else {
                    if ( strptime(stop_time.arrive.c_str(), "%r", &tm) == NULL ) {
                        throw invalid_time_stamp();
                    }
                }
            }
            
            catch ( invalid_time_stamp(its) ) {
                //
                // Some stops do not contain actual arrival or departure times
                // Perhaps there is something that could be done with them,
                // but without understanding the context, we must ignore these
                //
                cout << "  TRIP " << stop_time.trip << " covers this stop, but the published arrival time <" << stop_time.arrive << "> is unintelligible." << endl;
                continue;
            }
        
            //
            // Maintain list of qualifying trips as a set of trip ids
            // (strings) This eliminates duplicates, and avoids
            // copying entire trip structure.  We might have to search
            // for the trip later, but if that becomes important to
            // performance, we could convert the trips vector into an
            // unordered_map
            //
            cout << "  Adding trip: " << stop_time.trip << " for stop departing at " << stop_time.depart << endl;
            trips_containing_stop.insert(stop_time.trip);
        }
    }
    
    return trips_containing_stop.size() - count_before;
}

int optimize_paths(vector<Trip> &trips, unordered_map<string, Stop> &stops,
                   vector<Stop_Time> &stop_times, vector<Route> &routes,
                   double start_lat, double start_lon, double dest_lat, double dest_lon,
                   string start_time_of_day, double route_buffer, double time_buffer,
                   int longest_initial_wait, int longest_acceptable_time)
{
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> process_time;

    start = chrono::system_clock::now();


    set<string> source_stops;
    int num_source = add_stops_within_distance(stops, start_lat, start_lon, route_buffer, source_stops);
    cout << "Found " << num_source << " stops within " << route_buffer << " feet of starting point." << endl;

    //
    // Generate list of unique trips that stop near the source
    // position within the timebox.  For the lambda, specify the
    // capture explicitly to ensure the large tables are references
    // while operational parameters are captured by value.  G++
    // probably does "the right thing", but compilers tend to help
    // those who help themselves.
    // 
    set<string> source_trips;
    for_each(source_stops.begin(), source_stops.end(),
             [&trips, &stop_times, start_time_of_day, longest_initial_wait, &source_trips](const string &stop_id) {
                 add_trips_containing_stop(trips, stop_times, stop_id, &start_time_of_day, longest_initial_wait, source_trips);
             } );
    cout << "Found " << source_trips.size() << " unique trips that stop within " << route_buffer << " feet of destination." << endl;

    set<string> dest_stops;
    int num_dest = add_stops_within_distance(stops, dest_lat, dest_lon, route_buffer, dest_stops);
    cout << "Found " << num_dest << " stops within " << route_buffer << " feet of destination." << endl << endl;

    //
    // Generate list of unique trips that stop near the destination
    // position.  Use a wider timebox, since we don't know how long
    // various connections might take to get here.
    // 
    set<string> dest_trips;
    for_each(dest_stops.begin(), dest_stops.end(),
             [&trips, &stop_times, start_time_of_day, longest_acceptable_time, &dest_trips](const string &stop_id) {
                 add_trips_containing_stop(trips, stop_times, stop_id, &start_time_of_day, longest_acceptable_time, dest_trips);
             } );
    cout << "Found " << dest_trips.size() << " unique trips that stop within " << route_buffer << " feet of destination." << endl;

    end = chrono::system_clock::now();
    process_time = end - start;
    cout << "Path computation took " << process_time.count() << " seconds." << endl;

    return dest_trips.size();  // Not accurate, yet
}
