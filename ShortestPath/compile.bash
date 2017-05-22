#/cygdrive/c/Miles/RailsInstaller/DevKit/mingw/bin/g++ -std=c++11 main.C -static-libgcc -static-libstdc++ -o gtfs_route.exe
cmd='g++ -std=c++11 main.c++ parse_gtfs.c++ parse_csv.c++ optimize_route.c++ -static-libgcc -static-libstdc++ -o gtfs_route.exe'
echo $cmd
if $cmd; then
    echo Compile succeeded.
else
    echo Compile failed with status $?
fi

