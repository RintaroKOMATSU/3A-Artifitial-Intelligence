CXX = g++
CXXFLAGS = -O2


SRCS = $(wildcard *.cpp)
TARGETS = $(SRCS:.cpp=)

all: $(TARGETS)

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: clean tmpclean
clean: tmpclean
	rm -f $(TARGETS)
tmpclean:
	rm -f *~
