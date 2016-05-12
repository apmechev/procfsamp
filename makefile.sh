#!/bin/bash
rm -rf obj/Debug/*.o
g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c samplefile.cpp -o obj/Debug/samplefile.o
g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c PrettyUnits.cc -o obj/Debug/PrettyUnits.o
g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c src/GUTimer.cpp -o obj/Debug/GUTimer.o
g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c src/LOFARTimer.cc -o obj/Debug/LOFARTimer.o

g++ -o bin/Debug/procfs-sampler obj/Debug/samplefile.o obj/Debug/PrettyUnits.o obj/Debug/GUTimer.o obj/Debug/LOFARTimer.o -lrt

