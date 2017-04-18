#!/bin/bash
rm -rf obj/Debug/*.o
g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c procfscollector.cpp -o obj/Debug/procfscollector.o
g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c src/PrettyUnits.cc -o obj/Debug/PrettyUnits.o
g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c src/GUTimer.cpp -o obj/Debug/GUTimer.o
g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c src/LOFARTimer.cc -o obj/Debug/LOFARTimer.o

g++ -o bin/Debug/procfs-sampler obj/Debug/procfscollector.o obj/Debug/PrettyUnits.o obj/Debug/GUTimer.o obj/Debug/LOFARTimer.o -lrt


g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c stracecollector.cpp -o obj/Debug/stracecollector.o
g++ -o bin/Debug/strace-sampler obj/Debug/stracecollector.o obj/Debug/PrettyUnits.o obj/Debug/GUTimer.o obj/Debug/LOFARTimer.o -lrt

