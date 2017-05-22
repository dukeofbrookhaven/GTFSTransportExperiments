//
// PODS
//

#pragma once

#include <vector>
#include <string>

typedef struct _agency 
{
    std::string id;
    std::string name;
    std::string email;
} Agency;

typedef struct _route 
{
    std::string id;
    std::string short_name;
    std::string long_name;
    std::string desc;
} Route;

typedef struct _stop 
{
    std::string id;
    std::string code;
    std::string name;
    double lat;
    double lon;
} Stop;

typedef struct _stop_time 
{
    std::string trip;
    std::string arrive;
    std::string depart;
    std::string stop;
    long sequence;
} Stop_Time;

typedef struct _trip 
{
    std::string route;
    std::string id;
    std::string headsign;
} Trip;


struct missing_value_exception : public std::exception {
   const char * what () const throw () {
      return "Unable to locate required data value.";
   }
};

extern bool load_gtfs_system_data(std::string gtfs_data_folder,
                                  std::vector<Agency> &agencies,
                                  std::vector<Route> &routes,
                                  std::vector<Stop> &stops,
                                  std::vector<Stop_Time> &stop_times,
                                  std::vector<Trip> &trips
                                  ); // Throws missing_file_exception, missing_value_exception, and unterminated_quote_exception

