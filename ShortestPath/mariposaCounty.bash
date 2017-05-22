# The benefit of this example is that there's much less data, so test cycle is faster
#
#   California Highway Patrol: 5264 Highway 49 North, Mariposa, CA (37.496, -119.98)
#   Above All Automotive: 4995 Gold Leaf Dr, Mariposa, CA (37.506, -120.009)
#
# This test case also exposes a recorded stop time with empty values for arrival time
#
time ./gtfs_route.exe --routebuff 9000 -- ../GTFS_Data/yosemite-ca-us 37.496 -119.98 11:00 37.506 -120.009 ; echo Application terminated with status $?
