// This is start of the header guard.  ADD_H can be any unique name.  By convention, we use the name of the header file.
#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

// This is the content of the .h file, which is where the declarations go


// Mouse state structure: We want this available to Project.cpp file but also available elsewhere (touchScreen.cpp), therefore put in header file to share
typedef struct { 
	int fd;
	
	struct input_event ev;
	
	int mt_tracking_id;
	bool btn_touch; 
	int abs_x, abs_y;
	
} touch_t;

//extern: everyone that includes this header file to be aware of the variables and structure, but they are not defined in this header file
extern touch_t threadTouch; // create structure of structure type mouse_t and call it mouse


extern int quitState; // This is for use in the eventThread "function"

// Need to prototype mouseinit
int touchinit(int , int );

// Need to prototype 
void *eventThread(void *);





// This is the end of the header guard
#endif
