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

#include "parse_csv_line.h"
using namespace std;

enum quote_modes
{
    unstarted,
    started,
    ended
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
            //
            // Add comma to the string if we are within double-quotes
            //
            if ( quote_mode == started ) {
                underConstruction.push_back(ch);
            }
            //
            // Otherwise, terminate the current string and start the next
            //
            else {
                result.push_back(underConstruction);
                underConstruction.clear();
                quote_mode = unstarted;
            }
            break;
            
        default:
            //
            // Pass regular characters on to the target string
            //
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
