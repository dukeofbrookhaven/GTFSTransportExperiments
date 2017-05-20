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

#include <iostream>
#include <fstream>
#include <errno.h>
#include <string.h>
#include "parse_csv.h"
using namespace std;

enum quote_modes
{
    unstarted,
    started,
    ended
};

vector<string> parse_quoted_csv_line(string input_line)
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

        case ' ':
            //
            // Ignore leading spaces in fields, found in Australian data
            //
            if ( quote_mode != unstarted || underConstruction.length() > 0 ) {
                underConstruction.push_back(ch);
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

//
// Goal 1: Use Maps to make it easy to see how fields in the CSV are exploited
//
// Goal 2 (time permitting): define a custom iterator that loops through CSV lines,
//   thus localizing more of the line-by-line nature of the data
//
const bool parse_quoted_csv_file(string folder, string filename, vector<unordered_map<string, string>> &result)
{
    string fullpathfilename(folder);
    fullpathfilename.append("/").append(filename);
    ifstream csv_file(fullpathfilename);
    int linenum = 0;
    string line;
    if ( !csv_file.is_open() ) {
        cerr << "Error opening file <" << fullpathfilename << ">: " << strerror(errno) << endl;
        throw missing_file_exception();
    }

    try {
        //
        // Read CSV header line
        //
        if ( getline(csv_file, line) ) {
            linenum++;
            auto field_names = parse_quoted_csv_line(line);
            while ( getline(csv_file, line) ) {
                linenum++;
                unordered_map<string, string> record;
                auto split_line = parse_quoted_csv_line(line);
                auto field_name = field_names.begin();
                for ( auto item : split_line ) {
                    record[*field_name++] = item;
                }
                result.push_back(record);
            }
        }
    }
    catch ( const unterminated_quote_exception &uqe ) {
        cerr << "Error in file <" << fullpathfilename << ">, line " << linenum << ", unterminated quote: " << line << endl;
        throw uqe;
    }

    return true;
}
