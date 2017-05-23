#
# Goldsmith Park & Ride, 5530 Central Drive, Stone Mountain:  33.8108N, 84.1829W
# South Fulton Park & Ride, Fulton County: 33.5867N, 84.5125W
# 
# This path (from two supposedly known bus points) ran the longest of all
# and still produced no common points
#
time ./gtfs_route.exe --routebuff 1000 -- ../GTFS_Data/MARTA 33.8108 -84.1829 12:55 33.5867 -84.5125 ; echo Application Exited with $?
