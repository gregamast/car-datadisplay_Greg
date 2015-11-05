INCLUDEFLAGS=-I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads -I..
LIBFLAGS=-L/opt/vc/lib -lGLESv2 -lEGL -lbcm_host -lpthread  -ljpeg 

all: shapedemo hellovg mouse-hellovg particles clip Project

shapedemo:	shapedemo.c ../libshapes.o ../oglinit.o
	gcc -Wall $(INCLUDEFLAGS) $(LIBFLAGS) -o shapedemo shapedemo.c ../libshapes.o ../oglinit.o

test:	shapedemo
	./shapedemo demo 5

hellovg:	hellovg.c ../libshapes.o ../oglinit.o
	gcc -Wall $(INCLUDEFLAGS) $(LIBFLAGS) -o  hellovg hellovg.c ../libshapes.o ../oglinit.o

mouse-hellovg:	mouse-hellovg.c ../libshapes.o ../oglinit.o
	gcc -Wall $(INCLUDEFLAGS) $(LIBFLAGS) -o  mouse-hellovg mouse-hellovg.c ../libshapes.o ../oglinit.o

particles:	particles.c ../libshapes.o ../oglinit.o
	gcc -Wall $(INCLUDEFLAGS) $(LIBFLAGS) -o  particles particles.c ../libshapes.o ../oglinit.o

clip:	clip.c ../libshapes.o ../oglinit.o
	gcc -Wall $(INCLUDEFLAGS) $(LIBFLAGS) -o  clip clip.c ../libshapes.o ../oglinit.o

indent:
	indent -linux -c 60 -brf -l 132 shapedemo.c hellovg.c mouse-hellovg.c particles.c clip.c

Project: Project.cpp Gauge.cpp serial.cpp DataStream.cpp touchScreen.cpp TouchableObject.cpp Readout.cpp  ../libshapes.o ../oglinit.o
	g++ -std=gnu++0x -Wall $(INCLUDEFLAGS) $(LIBFLAGS) -o Project Project.cpp Gauge.cpp serial.cpp DataStream.cpp touchScreen.cpp TouchableObject.cpp Readout.cpp  ../libshapes.o ../oglinit.o -lbcm2835



