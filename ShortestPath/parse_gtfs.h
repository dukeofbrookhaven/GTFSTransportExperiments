//
// PODS
//

#include <vector>
#include <string>

typedef struct _agency 
{
    std::string agency_id;
    std::string agency_name;
    std::string agency_email;
} Agency;

extern int load_gtfs_system_data(std::string gtfs_data_folder, Agency &agency);

