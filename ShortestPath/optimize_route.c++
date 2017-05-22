#define _XOPEN_SOURCE 700
#undef __STRICT_ANSI__
#include <iostream>
#include <vector>
#include <math.h>
#include <ctime>
#include <algorithm>
#include "parse_gtfs.h"
#include "optimize_route.h"

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
// Alas, Execution_Policy appears to be C++ 17, and MinGW does not readily do C++ 14 yet
// For the time being, we are sequential
//

int find_trips(vector<Stop> &stops, vector<Stop_Time> &stop_times, vector<Trip> &trips, double lat, double lon, double distance, string time_of_day)
{
    int stops_found = 0;

    vector<Stop> stops_within_distance;
    //
    // My first C++ lambda
    //
    copy_if(stops.begin(), stops.end(), back_inserter(stops_within_distance),
            [=](Stop &s) { return dist_feet(lat,lon,s.lat,s.lon) < distance; });
      //      [=](Stop &s) { return 1; });

    // multimap<double, pair<string,Stop>> preferred_trips;
    for ( auto dest : stops_within_distance ) {
        if ( stops_found == 0 ) {
            cout << endl << "Found the following stops within distance of (" << lat << ", " << lon << "):" << endl;
        }
        
        double d = dist_feet(lat, lon, dest.lat, dest.lon);
        cout << d << " feet away to " << dest.name <<"<" << dest.id << "/" << dest.code << ">" << endl;

        int trips_later_today = 0;
        for ( auto stop_time: stop_times ) {
            if ( dest.id == stop_time.stop ) {
                //
                // Some stops do not contain actual arrival or departure times
                // Perhaps there is something that could be done with them,
                // but without understanding the context, we must ignore these
                //
                try {
                    int minutes_away = difftime_in_minutes(time_of_day, stop_time.arrive);
                    if ( minutes_away < 0 || minutes_away > 180 ) {
                        continue; // Skip trips that have already happened today, and those more than 3 hours away
                    }
                }
                catch ( invalid_time_stamp(its) ) {
                    cout << "  TRIP " << stop_time.trip << " covers this stop, but the published arrival time <" << stop_time.arrive << "> is unintelligible." << endl;
                    continue;
                }
                
                // cout << "Computing difference between " << time_of_day << " and " << stop_time.arrive << "( trip " << stop_time.trip << " << departs from stop " << stop_time.stop << " at " << stop_time.depart << ")" << endl;
                int minutes_away = difftime_in_minutes(time_of_day, stop_time.arrive);
                if ( minutes_away < 0 ) {
                    continue; // Skip trips that have already happened today
                }
            
                cout << "  Trip " << stop_time.trip << " arrives in " << minutes_away << " minutes at " << stop_time.arrive << " and departs at " << stop_time.depart << endl;
                if ( ++trips_later_today > 20 ) {
                    break; // Don't overwhelm the output
                }
            }
        }
        stops_found++;
    }

    if ( stops_found == 0 ) {
        cout << endl << "No stops found within " << distance << " feet of (" << lat << ", " << lon << "):" << endl;
    }

    return stops_found;
}
