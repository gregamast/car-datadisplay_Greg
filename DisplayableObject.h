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


public: //This is an access_specifier. 
	
	DisplayableObject(void);					//	DisplayableObject CONSTRUCTOR	
	
	//	Initialize pixel buffer surface of given pixel size (width & height) using provided VGImage as the color buffer
	EGLSurface createBufferSurface( int , int , VGImage* );
	
	void switchToBufferSurface(EGLSurface);
	void switchToDisplaySurface(void);
	
	
	void drawBuffer(int, int, int, int, float, VGImage); //
	
private:

	
	
};

#endif