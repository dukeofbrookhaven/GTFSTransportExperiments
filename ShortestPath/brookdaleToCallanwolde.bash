#
# Brookdale Park, East Point:  33.688N, 84.42W
# Callanwolde Fine Arts Center, 980 Briarcliff Rd: 33.782N, 84.345W
# 
# This case took a while to run, and didn't produce any common stops.  It
# appeared that the Lindbergh interchange got snarled up into one of
# the networks.
#
time ./gtfs_route.exe --routebuff 1000 -- ../GTFS_Data/MARTA 33.688 -84.42 12:55 33.782 -84.345 ; echo Application Exited with $?
