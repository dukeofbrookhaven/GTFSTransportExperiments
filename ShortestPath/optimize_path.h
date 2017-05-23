#include <vector>
#include <set>
#include <exception>
#include <string>
#include "parse_gtfs.h"

using namespace std;

struct invalid_time_stamp : public exception {
   const char * what () const throw () {
      return "Unable to recognize arrival or departure time.";
   }
};

extern double dist_feet(double th1, double ph1, double th2, double ph2);

extern long difftime_in_minutes(string a, string b); // Throws invalid_time_stamp

extern int add_stops_within_distance(vector<Stop> &stops, double lat, double lon, double distance,
                                     /* in/out */ vector<Stop> &stops_within_distance);

extern int add_trips_containing_stop(const vector<Trip> &trips, const vector<Stop_Time> &stop_times,
                                     const Stop &stop, const string *after_time_of_day,
                                     /* in/out */ set<string> &trips_containing_stop);

extern int optimize_paths(vector<Trip> &trips, vector<Stop> &stops,
                          vector<Stop_Time> &stop_times, vector<Route> &routes,
                          double start_lat, double start_lon, double dest_lat, double dest_lon,
                          string start_time_of_day, double route_buffer, double time_buffer);




