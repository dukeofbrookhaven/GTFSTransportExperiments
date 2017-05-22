#include <vector>
#include <exception>
#include "parse_gtfs.h"

extern double dist_feet(double th1, double ph1, double th2, double ph2);

extern int find_trips(std::vector<Stop> &stops, std::vector<Stop_Time> &stop_times,
                      std::vector<Trip> &trips, double lat, double lon,
                      double distance, std::string time_of_day);  // Throws invalid_time_stamp

struct invalid_time_stamp : public std::exception {
   const char * what () const throw () {
      return "Unable to recognize arrival or departure time.";
   }
};
