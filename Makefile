CXX = g++
CXXFLAGS = -Wall -g -MD -O3

RMEXEC = diff
RMFILES = *.d *.o

.PHONY: all clean

all: diff

diff: main.o differentiator.o expression.o
	$(CXX) -o $@ $^

%.o: %.c
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -rf $(RMEXEC) $(RMFILES)

-include *.d