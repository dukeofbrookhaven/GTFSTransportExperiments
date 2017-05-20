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
#include <exception>
#include "parse_gtfs.h"
using namespace std;

//
// Parsing CSV
//
// Need to handle use-cases like:
//
//     7694,110,"Peachtree St./""The Peach""",,3,,819FF7,
//
// Hate to reinvent the wheel, here, but there seem to be 15 different types of
// wheel on StackOverflow, and I want one that works for GTFS's use case,
// and I want to "do my own work".
//

enum quote_modes
{
    unstarted,
    started,
    ended
};

struct unterminated_quote_exception : public exception {
   const char * what () const throw () {
      return "Invalid Line encountered in CSV: Unterminated quotes";
   }
};
 
vector<string> parse_quoted_csv(string input_line)
{
    vector<string> result;
    string underConstruction;
    enum quote_modes quote_mode { unstarted };
    for ( auto ch : input_line ) {
        switch ( ch ) {
                
        case '"':
            //
            // Begin and end quotes are consumed without
            // being processed.
            // 
            if ( quote_mode == unstarted ) {
                quote_mode = started;
            }
            else if ( quote_mode == started ) {
                quote_mode = ended;
            }
            //
            // In the case of a repeated double quote, insert one literal quote 
            else if ( quote_mode == ended ) {
                underConstruction.push_back(ch);
                quote_mode = started;
            }
            break;

        case ',':
            if ( quote_mode == started ) {
                underConstruction.push_back(ch);
            }
            else {
                //
                // Terminate current string.
                //
                result.push_back(underConstruction);
                underConstruction.clear();
                quote_mode = unstarted;
            }
            break;
            
        default:
            underConstruction.push_back(ch);
            break;
        }
    }
    
    //
    // Handle erroneous input line with unterminated quotes
    // More than likely, we have encountered a new dialect of CSV
    //
    if ( quote_mode == started ) {
        throw unterminated_quote_exception();
    }
    
    //
    // Handle the last (possibly-empty) field, still under construction
    //
    result.push_back(underConstruction);
    return result;
}


int load_gtfs_system_data(string gtfs_data_folder, Agency &agency)
{
    try {
        auto split_line = parse_quoted_csv(gtfs_data_folder);
        for ( auto item : split_line ) {
            cout << item << endl;
        }
    }
    catch ( const unterminated_quote_exception &uqe ) {
        cerr << "Error in line 1, unterminated quote: " << gtfs_data_folder << endl;
        throw uqe;
    }
            
//, Map<Route> routes, Map <Stop> stops, Map<Stop_Time> Stops)
}
