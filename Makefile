CXX=distcc arm-linux-gnueabihf-g++ -std=gnu++0x $(INCLUDEFLAGS)

INCLUDEFLAGS=-I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg/client/config4cpp/include -I..


LIBFLAGS=-L/opt/vc/lib -L/opt/vc/lib -lGLESv2 -lEGL -lbcm_host -lpthread  -ljpeg -L/home/pi/openvg/client/config4cpp/lib

objects = Serial.o touchScreen.o Gauge.o TouchableObject.o PID.o Button.o TextView.o Menu.o parsingUtils.o Project.o

all: $(objects) Project

Project.o: Project.cpp

Menu.o: Menu.cpp

TextView.o: TextView.cpp

Button.o: Button.cpp

PID.o: PID.cpp

Gauge.o: Gauge.cpp

Serial.o: Serial.cpp

PID.o: PID.cpp

TouchableObject.o: TouchableObject.cpp

touchScreen.o: touchScreen.cpp

parsingUtils.o:	parsingUtils.cpp

Project: Project $(objects) 
	g++ -Wall -std=gnu++0x $(LIBFLAGS) -o Project $(objects) ../libshapes.o ../oglinit.o -lbcm2835 -lconfig4cpp
	rm *.o 
	rm /tmp/distcc*.*
	
	 
