OBJECT_FILES=main.o image.o tools.o
CFLAGS=--debug `pkg-config gtkmm-2.4 cairomm-1.0 --cflags --libs`

all: simpledraw


simpledraw: $(OBJECT_FILES)
	g++ $(OBJECT_FILES) $(CFLAGS) -o simpledraw

main.o: main.cpp main.h image.h tools.h gtklibs.h
	g++ -c -o main.o main.cpp $(CFLAGS)

image.o: image.cpp image.h gtklibs.h
	g++ -c -o image.o image.cpp $(CFLAGS)

tools.o: tools.cpp tools.h image.h gtklibs.h
	g++ -c -o tools.o tools.cpp $(CFLAGS)

clean:
	-rm -f $(OBJECT_FILES) simpledraw

