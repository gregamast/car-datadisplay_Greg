CXX=distcc arm-linux-gnueabihf-g++ -std=gnu++0x $(INCLUDEFLAGS)

INCLUDEFLAGS=-I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg/client/config4cpp/include -I..


LIBFLAGS=-L/opt/vc/lib -L/opt/vc/lib -lGLESv2 -lEGL -lbcm_host -lpthread  -ljpeg -L/home/pi/openvg/client/config4cpp/lib

objects = serial.o touchScreen.o Gauge.o TouchableObject.o DataStream.o Button.o Project.o

all: $(objects) Project

Project.o: Project.cpp

Button.o: Button.cpp

DataStream.o: DataStream.cpp

Gauge.o: Gauge.cpp

serial.o: serial.cpp

TouchableObject.o: TouchableObject.cpp

touchScreen.o: touchScreen.cpp

Project: Project $(objects) 
	g++ -Wall -std=gnu++0x $(LIBFLAGS) -o Project $(objects) ../libshapes.o ../oglinit.o -lbcm2835 -lconfig4cpp
	rm *.o 
	rm /tmp/distcc*.*
	
	 
