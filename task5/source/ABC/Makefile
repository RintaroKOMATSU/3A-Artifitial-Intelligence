CXX = g++


CXXFLAGS = -Wall -O2 -Iinclude -fopenmp
LDLIBS =  -lglut -lGL -lGLU 

SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:%.cpp=%.o)


TARGET = bin/main

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS) 

.PHONY: tmpclean clean

tmpclean:
	rm -f $(SRCDIR)/*~ *~
clean: tmpclean
	rm -f $(TARGET) $(OBJS)