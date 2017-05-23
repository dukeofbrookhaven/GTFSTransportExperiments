# The benefit of this example is that there's much less data, so test cycle is faster
#
#   California Highway Patrol: 5264 Highway 49 North, Mariposa, CA (37.496, -119.98)
#   Above All Automotive: 4995 Gold Leaf Dr, Mariposa, CA (37.506, -120.009)
#   John C Fremont Healthcare: 5189 Hospital Rd, Mariposa, CA 95338 (37.5, -119.978)
#
#
# Test case 1:  Hospital to CHP (need 4000 walking buffer to generate points)
#
time ./gtfs_route.exe --routebuff 4000 -- ../GTFS_Data/yosemite-ca-us 37.5 -119.978 11:00 37.496 -119.98  ; echo Application terminated with status $?
#
# Test case 2:  CHP to body shop (no path, even expanding searching distance to 9000 feet)
#
#   This test case also exposes a recorded stop time with empty values for arrival time
#
time ./gtfs_route.exe --routebuff 9000 -- ../GTFS_Data/yosemite-ca-us 37.496 -119.98 11:00 37.506 -120.009 ; echo Application terminated with status $?
#
# These test cases might suggest a narrative, but I've never been to this place :-)
#
