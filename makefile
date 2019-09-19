
CXX = g++
CXXFLAGS = -Wall

make: Sally.o driver2.cpp
        $(CXX) $(CXXFLAGS) Sally.o driver2.cpp -o output

Sally.o: Sally.cpp Sally.h
        $(CXX) $(CXXFLAGS) Sally.cpp Sally.h -c

clean:
        rm *.o output


