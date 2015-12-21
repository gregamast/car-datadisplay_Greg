#ifndef TOUCHABLEOBJECT_H
#define TOUCHABLEOBJECT_H 

#include <linux/input.h>
#include "touchScreen.h"
using namespace std;
#include<bcm2835.h>

/**************************************************************************************************************
* TouchableObject Class
* Parent class for all display objects to handle touchscreen processing.
*
* Accepts:	Touch structure? Mouse structure? needs improvement
*
* Determines if and how (TBD) a display object has been touched by keeping track of size, shape, and position
*
**************************************************************************************************************/

class TouchableObject
{
	// private: // Touchable Object is the BASE class. Button is the CHILD.
private:
	/* Circular properties */
	int cX;				// Circle center x coordinate
	int cY;				// Circle center y coordinate
	int cRad;			// Circle radius
	/* Rectangular properties */
	int rW;				// Rectangle width current
	int rH;				// Rectangle height current
	int rX;			// current Rectangle center x coordinate  
	int rY;			// current Rectangle center y coordinate 
	
	
	//When position is updated through updatePosition method, these "current" position properties are reassigned
	
	
	//These properties capture the starting location and time when each move function is called

	uint64_t moveStartTime;
	uint64_t fadeStartTime;
	
	uint64_t loopTime;
int moveDuration; //milliseconds, this is what is called into the move function
int fadeDuration;

int fadePercentage; //This is private since it is gievn to touchable object child objects via passed through a getter

int finalFadePercentage;
int initialFadePercentage;


	
	/* Touch properties set by update function */


	/* Enable touch processing */
	bool touchEnabled;
	bool visible;
	bool lpVisible;	// Last pass boolean of visibility
	
	bool touched;
	
	
	

	
	
	bool isRectangular;	// Rectangle or circle? You decide.
	
	

	// Moving touchable object properties
	
	
	string motionType; //Denotes the type of motion for the move action: ex. "linear", "sinusoidal", etc...
	string fadeType;//Denotes the type of fade speed for the alpha change
	
	

protected:

//The final desired location for the move call.  These are in protected since we need these for image buffers
int finalPosX;		
int finalPosY;


//These are for background screen capture to redraw any interrupted visuals
VGImage BackgroundBuffer; // image buffer containing background behind object
VGImage MovementBuffer;  //Image buffer containing background behind movement path


//These properties capture the starting location and time when each move function is called
	int moveStartRX;
	int moveStartRY;
	int moveStartCX;
	int moveStartCY;
	
	

	
	
	/* Methods called by derived classes */
	bool getVisibility(void);				// Called by derived classes to set visibility
	bool getLPVisibility(void);				// Called by derived classes to determine previous visiblity
	
	int getDesiredPosX(void);
	int getDesiredPosY(void);

	
	void setCircular(void);				// Called by derived class to set touch area as circular
	void setCircleCenter(int, int);		// Called by derived class to set circular touch area center
	void setCircleRadius(int);			// Called by derived class to set circular touch area radius

	void setRectangular(void);			// Called by derived class to set touch area as rectangular
	void setRectWidthHeight(int, int);	// Called by derived class to set rectangular touch area size
	void setRectCenter(int, int);	// Called by derived class to set rectangular touch area center

	/* Constructor */
	TouchableObject(void);				// Sets up TouchableObject, sets properties to safe state
	
	int getDesiredFadePercentage(void);	//This returns the factor to scale the current alpha value for touchable objects (gauge, button, etc.). Protected since called by the child class only
	

public:
	/* Control methods */
	void touchEnable(void);				// Enables touch functionality
	void touchDisable(void);			// Disables touch functionality
	void setVisible(void);				// Sets display object visible
	void setInvisible(void);			// Sets display object invisible

	/* Update method */
	void updateTouch(touch_t);

	/* All that for this.. */
	bool isTouched(void);
	
	// take in final pos x, final pos y, the desired transition time [milliseconds],  motion type, 
	void move(int, int, int, string);
	
	void moveTo(int, int, int, string);
	
	
	void fade(int, int, string); // final desired fade percentage, transitionTime,  fadeType (sinusoidal, linear, etc...)
	
	// Get the current position of the TOUCHABLEOBJECT
	int getCurrentPosX(void);
	int getCurrentPosY(void);
	
	
	// Animation
	void moveOffRight(void);
	void moveOnRight(void);
	
	void moveOffLeft(void);
	void moveOnLeft(void);
	
	void moveOffTop(void);
	void moveOnTop(void);
	
	void moveOffBottom(void);
	void moveOnBottom(void);
	
	void updateVisuals(void);
	
	
	
	
};

#endif