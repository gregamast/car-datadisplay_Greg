
using namespace std;
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "VG/openvg.h"		// OpenVG
#include "VG/vgu.h"			// OpenVG
#include "fontinfo.h"		// OpenVG	
#include "shapes.h"			// OpenVG
#include <stdlib.h>			// OpenVG

#include <linux/input.h>
#include <fcntl.h>
#include <pthread.h>
#include "Project.h"
#include "touchScreen.h"


// touchinit starts the touch event thread
int touchinit(int w, int h) {
	pthread_t inputThread;

	return pthread_create(&inputThread, NULL, &eventThread, NULL);
}



// evenThread reads from the touch input file
void *eventThread(void *arg) {

	// Open touch driver
	if ((touch.fd = open("/dev/input/event0", O_RDONLY)) < 0) {
		fprintf(stderr, "Error opening touch!\n");
		quitState = 1;
		return &quitState;
	}

	int mt_tracking_id = 0;
		bool btn_touch = false;
		int abs_x = 0; 
		int abs_y = 0;
 
	while (1) {
		// This read function only returns 1 set of structure property values at a time!
		/* 		struct input_event {
				struct timeval time;
				__u16 type;
				__u16 code;
				__s32 value;
				}; */
		// We read, we sort the value (if its x type store it in touch.x, if y..., then start loop again and read next value
		read(touch.fd, &touch.ev, sizeof(struct input_event));

		
		// Parsing Touch Logic
		
		
		
		//std::cout<<"event type is: "<<touch.ev.type<<endl;
		//std::cout<<"event code is: "<<touch.ev.code<<endl;
		if(touch.ev.type==EV_SYN){
			
			// std::cout<<"event type is: "<<touch.ev.type<<endl;
			if(touch.ev.code == SYN_REPORT){
				
				// std::cout<<"event code enum is: "<<touch.ev.code<<" which is: SYN_REPORT "<<endl;
				// std::cout<<"The attributes of struct touch are being populated... ... "<<endl;
				touch.mt_tracking_id = mt_tracking_id;
				touch.btn_touch = btn_touch;
				touch.abs_x  = abs_x;
				touch.abs_y = abs_y;
			}
		}
		else if(touch.ev.type==EV_ABS){
			// std::cout<<"event type is: "<<touch.ev.type<<endl;
			if(touch.ev.code == ABS_X){
				abs_x = touch.ev.value;
				// std::cout<<"absX: "<<abs_x<<endl;
			}
			if(touch.ev.code == ABS_Y){
				abs_y =  480 - touch.ev.value; //the touch 0,0 origin is defined on the upper left (the way we are using the screen) so need to flip the y value so that we convert the origin to bottom left
				// std::cout<<"absY: "<<abs_y<<endl;
			}
			if(touch.ev.code==ABS_MT_TRACKING_ID){
				mt_tracking_id = touch.ev.value;
				// std::cout<<"mt_tracking_id: "<<mt_tracking_id<<endl;
			}
		}
		
		else if(touch.ev.type==EV_KEY){
			//std::cout<<"event type is: "<<touch.ev.type<<endl;
			if(touch.ev.code == BTN_TOUCH){
				btn_touch = touch.ev.value;
				//std::cout<<"btn_touch: "<<btn_touch<<endl;
			}
			
		}
		
		
		
	}
	
}
