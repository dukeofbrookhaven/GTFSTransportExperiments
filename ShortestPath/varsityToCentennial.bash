#
# This test case might be a worst-case, since it appears to hit really
# close to a rail station, which means the network expansion is
# including several bus systems
#
time ./gtfs_route.exe --routebuff 1000 -- ../GTFS_Data/MARTA 33.8256 -84.3893 12:55 33.7603 -84.3935 ; echo Application Exited with $?
