OBJDIR = obj/Debug
OBJ=PrettyUnits.o GUTimer.o LOFARTimer.o
PROCOBJ= $(OBJ) procfscollector.o
STRAOBJ=$(OBJ) stracecollector.o
OBJECTS = $(OBJDIR)/*.o 


procfscollector.o:
	g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c procfscollector.cpp -o $(OBJDIR)/procfscollector.o
PrettyUnits.o:
	g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c src/PrettyUnits.cc -o $(OBJDIR)/PrettyUnits.o
GUTimer.o:
	g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c src/GUTimer.cpp -o $(OBJDIR)/GUTimer.o
LOFARTimer.o:
	g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c src/LOFARTimer.cc -o $(OBJDIR)/LOFARTimer.o

procfs-sampler: $(PROCOBJ)
	g++ -O2 -o bin/Debug/procfs-sampler $(OBJECTS)  -lrt

stracecollector.o:
	g++ -Wall -fexceptions -g -std=c++11 -Iinclude -c stracecollector.cpp -o $(OBJDIR)/stracecollector.o

stracecollector: $(STRAOBJ)
	g++ -O2 -o bin/Debug/strace-sampler $(OBJECTS) -lrt 


clean:
	rm -rf $(OBJDIR)/*.o

