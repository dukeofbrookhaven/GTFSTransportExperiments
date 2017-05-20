//
// PODS
//

#include <vector>
#include <string>

typedef struct _agency 
{
    std::string id;
    std::string name;
    std::string email;
} Agency;

struct missing_input_exception : public std::exception {
   const char * what () const throw () {
      return "Unable to locate necessary input file.";
   }
};

extern bool load_gtfs_system_data(std::string gtfs_data_folder, Agency &agency); // Throws missing_input exception and unterminated_quote_exception

