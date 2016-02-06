#include <locale.h>	//For the parsing library
#include </home/pi/openvg/client/config4cpp/include/config4cpp/Configuration.h> //For the parsing library
using namespace config4cpp; //For the parsing library

using namespace std;
#include <stdio.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"
#include <stdlib.h>
#include <math.h>
#include <iostream> 

#include <cmath>


#include "parsingUtils.h" // utilize parsing utilities for configuration file processing

#include "EGL/egl.h"

#include "DisplayableObject.h"        

#include <fstream> //This is for reading the config file in
#include <algorithm>







/****************************************************************
	CONSTRUCTOR
****************************************************************/
DisplayableObject::DisplayableObject(void){
	
	display = eglGetCurrentDisplay();
	if(display == EGL_NO_DISPLAY) cout << "Failed to get current display" << endl;
	
	static const EGLint attribute_list[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};
	
	
	

	
	result = eglChooseConfig(display, attribute_list, &config, 1, &num_config); //result denotes success of choosing a configuration
	if(result == EGL_FALSE) cout << "Failed to choose config" << endl;
	
	//Get the current drawing surface: this is so that when we are done working with the pbuffer surfcae, we can swithc back to the current one
	displaySurface = eglGetCurrentSurface(EGL_DRAW);
	if(displaySurface == EGL_NO_SURFACE) cout << "Failed to get current surface" << endl;
	
	//Get the current context: this is so we know what context to use
	renderingContext = eglGetCurrentContext();
	if(renderingContext == EGL_NO_CONTEXT) cout << "Failed to get current context" << endl;
	
}

//	Initialize pixel buffer surface of given pixel size (width & height) using provided VGImage as the color buffer
EGLSurface DisplayableObject::createBufferSurface( int surfaceWidth, int surfaceHeight , VGImage*  bufferPtr){
	
	*bufferPtr = vgCreateImage(VG_sABGR_8888, 2*surfaceWidth	, 2*surfaceHeight, VG_IMAGE_QUALITY_BETTER);
	
	static const EGLint surfAttr[] = {
		EGL_HEIGHT, surfaceHeight*2,
		EGL_WIDTH, surfaceWidth*2,
		EGL_NONE
	};
	
	EGLSurface bufferSurface = eglCreatePbufferFromClientBuffer (display, EGL_OPENVG_IMAGE, (EGLClientBuffer) *bufferPtr, config, surfAttr); //the image needs to be cast to EGLClientBuffer
	if(bufferSurface == EGL_NO_SURFACE)  cout << "Failed to create pbuffer surface" << endl;
	
	
	// switch to the surface to clear the surface..
	switchToBufferSurface(bufferSurface);
	
	float surfaceBackgroundColor[4];
	RGBA(0, 0, 0, 0.1, surfaceBackgroundColor); //dumps these attribures into the array we created
	vgSetfv(VG_CLEAR_COLOR, 4, surfaceBackgroundColor); //setv sets a value, seti sets an attribute
	vgClear(0, 0, surfaceWidth*2, 2*surfaceHeight);
	
	switchToDisplaySurface();
	
	return bufferSurface;
}





void DisplayableObject::switchToBufferSurface(EGLSurface bufferSurface){
	
	// https://www.khronos.org/registry/egl/sdk/docs/man/html/eglMakeCurrent.xhtml
	result = eglMakeCurrent(display, bufferSurface, bufferSurface, renderingContext);
	if(result == EGL_FALSE) cout << "Failed to make new display current" << endl;
	
	
}
void DisplayableObject::switchToDisplaySurface(void){
	
	//switch back to original surface
	eglMakeCurrent(display, displaySurface, displaySurface, renderingContext);
	if(result == EGL_FALSE) cout << "Failed to make original display current" << endl;
	
	
}





