// The Gauge header file: define Gauge class and headers

#ifndef DISPLAYABLEOBJECT_H
#define DISPLAYABLEOBJECT_H


#include "EGL/egl.h"

/******************************************************************************************************************************
									GAUGE CLASS
******************************************************************************************************************************/
class DisplayableObject{
	
private: //This is an access_specifier.  
	
	
	
	/****************************************************************
		
	****************************************************************/

	


	
	// DISPLAY
	EGLDisplay 		display;						//	EGL display connection (eglGetCurrentDisplay)
	EGLContext 		renderingContext;		//	Rendering context (eglGetCurrentContext)
	EGLSurface 		displaySurface;			// Display surface (eglGetCurrentSurface)
	
	
	EGLint 				num_config;				//	Number of EGLConfigs returned (eglChooseConfig)
	EGLBoolean 		result; 						//	Boolean used to store result of EGL calls
	EGLConfig 		config;						//	Frame Buffer configuration (eglChooseConfig)
	
	int centerX; 										//The displayable object center position: A private attribute, and as such cannot be collected by derived classes.
	int centerY;										//The displayable object center position: A private attribute, and as such cannot be collected by derived classes.


public: //This is an access_specifier. 
	
	DisplayableObject(void);					//	DisplayableObject CONSTRUCTOR	
	
	//	Initialize pixel buffer surface of given pixel size (width & height) using provided VGImage as the color buffer
	EGLSurface createBufferSurface( int , int , VGImage* );
	
	void switchToBufferSurface(EGLSurface);
	void switchToDisplaySurface(void);
	
	
	void drawBuffer(int, int, int, int, float, VGImage); //
	
	int getDOPosX(void); //Return the X position of the current Displayable Object:  This is to collect the position of derivede classes
	int getDOPosY(void); // return the  position
	
private:

protected:

void setDOPos(int , int); //THe derived class object (gauge, graph, etc) calls this base class method in constructor to set the display object position equal to position it was instaitated at
	
};

#endif