PROGNAME = main
TESTNAME = test
LIBS = -lboost_system
INCLUDES = -I/usr/local/include
LDFLAGS = -L./
OBJECTS = main.o heuristic.o State.o Board.o
TESTOBJECTS = test.o heuristic.o State.o Board.o
GAMEFILES = build/heuristic.o build/Board.o build/State.o
CFLAGS = -Wall -Wextra -O2 -march=native -std=c++11 -pedantic
# CFLAGS = -Wall -Wextra -ggdb3 -std=c++11 -pedantic

all: $(PROGNAME)
test: $(TESTNAME)

$(PROGNAME): $(OBJECTS)
	@mkdir -p bin
	g++ -o bin/$(PROGNAME) build/$(PROGNAME).o $(GAMEFILES) $(LIBS) $(INCLUDES) $(LDFLAGS)

$(TESTNAME): $(TESTOBJECTS)
	@mkdir -p bin
	g++ -o bin/$(TESTNAME) build/$(TESTNAME).o $(GAMEFILES) $(LIBS) $(INCLUDES) $(LDFLAGS)

$(OBJECTS): Makefile
$(TESTOBJECTS): Makefile

%.o: %.cpp
	@mkdir -p build
	g++ -c $(CFLAGS) $(INCLUDES) $(LIBS) -o build/$@ $<

clean:
	rm -rf build *.o $(PROGNAME)
