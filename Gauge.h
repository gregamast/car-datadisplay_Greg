// The Gauge header file: define Gauge class and headers

#ifndef GAUGE_H
#define GAUGE_H

#include "TouchableObject.h"
#include "Button.h"
#include "DisplayableObject.h"
#include "EGL/egl.h"


/******************************************************************************************************************************
									GAUGE CLASS
******************************************************************************************************************************/
class Gauge: public TouchableObject, DisplayableObject{
	
private: //This is an access_specifier.  
	
	int centerX	;  
	int centerY	; 
	int radius;
	float gaugeRadius;
	float dynamicContentRadius; // This is the radius up to which the content is dynamically rewritten and thus needs to be updated in the while loop
	float borderColor[4];
	float backgroundColor[4];
	float needleColor[4];
	bool borderDesired;
	
	string identifier; //This is to identify the name or type of gauge object, as set in the configuration file
	string group;// THis is to identify if the particular gauge belongs to a greater gruop  This is private because we are calling it with a getter 
	
	/****************************************************************
		DISPLAY RANGE Member Properties
	****************************************************************/
	int numRanges;
	string* EngUnits;
	
	float* startVal;
	float* stopVal;
	float* startAng;
	float* stopAng;
	float* majorInt;
	float* minorInt; 
	float** majorTickColor;
	float** minorTickColor;

	
	/****************************************************************
		DISPLAY LABEL Member Properties
	****************************************************************/
	
	float* labelStartVal;
	float* labelStopVal;
	float* labelIncrement;
	int* labelDecPlaces;
	float* labelStartAng;
	float* labelStopAng;
	float** labelColor;

	Fontinfo* labelFont;
	
	VGImage gaugeBuffer; //THis is now going to be the buffer which feeeds the offscreen surface
	EGLint num_config;
	EGLBoolean result; //result denotes success of choosing a configuration
	EGLConfig config;
	EGLDisplay realDisplay; 
	EGLSurface realSurface;
	EGLContext realContext;
	EGLSurface mySurface;


public: //This is an access_specifier. 
	
	Gauge(int, int, int , string);				//Gauge CONSTRUCTOR	
	
	string getIdentifier(void); //This is to be called in the main project logic to determine the id associated with a particular object at an index of the object vector
	string getGroup(void);//This is to be called in the main project logic to determine the main group the button fits into
	
	
	void setNumRanges( int ); // this is actually private since we are only calling it in configure method, so eventually should make this private
	void setEngUnits(string , int);
	
	// Draw function setters (tick marks & border)
	void setBorderColor(float*);				// Set border color 			(rgba)
	void setBackgroundColor(float*);			// Set background color 		(rgba)
	void setDataRange(float,float, int);		// Set display data range 		(min, max, range #)
	void setDataAngleRange(float, float, int);	// Set display angular range 	(min, max, range #)
	void setMajorInterval(float, int);			// Set major tick interval 		(interval, range #)
	void setMinorInterval(float, int);			// Set minor tick interval 		(interval, range #)
	void setMajorTickColor(float*, int);		// Set major tick color 		(rgba, range #)
	void setMinorTickColor(float*, int);		// Set major tick color 		(rgba, range #)

	// Draw function
	// (Call after setting all of the above!)
	void draw(void);							// Draw gauge (initialize)
	
	// Update function setters (labels, needle, etc)
	void setNeedleColor(float*);				// Set needle color 			(rgba)
	void setLabelColor(float*, int);			// Set label label color 		(rgba, range #)
	void setLabelRange(float, float, int);		// Set label display range 		(min, max, range #)
	void setLabelAngleRange(float, float, int);	// Set label angle range 		(min, max, range #)
	void setLabelIncrement(float, int);			// Set label increment 			(increment, range #)
	void setLabelDecPlaces(int, int);			// Set label # of dec places 	(# of decimal places, range #)
	void setLabelFont(Fontinfo, int);				// Set label font				(font)

	void setReadoutColor(float*);				// Set readout color 			(rgba)
	void setReadoutDigits(int);					// Set readout # digits 		(# digits)
	void setReadoutDecPlaces(int);				// Set readout # dec places 	(# of decimal places)
	void setReadoutAngle(float);				// Set readout angle 			(readout center angle)
	void setReadoutRadius(float);				// Set readout radius 			(readout center radius)
	void setReadoutFont(Fontinfo);				// Set label font 				(font)
	
	
	// Update function
	// (Call after setting all of the above!)
	void update(float, string);							// Update gauge with value		(value)

	// Other gauge draw functions
	void moveResize(float, float, float);		// Move and or resize gauge 	(centerX, centerY, radius)
	void setVisible(bool);						// Set gauge visibility			(visibility)
							
	
	
	void configure(string); // Takes in string "configType" which is the title of the section in the config file to which set the configuration parameters (for example, configureation type could be "boostGauge" or "tempGauge")
	

	
private:

	
	void processConfigLine(string , int );
	// Internally accessed methods (called within draw / update functions)
	void drawTickSet(float, float, float, float, float*, bool);			// Draw tick set(s): 
	//		(start angle
	//		 stop angle
	//		 angle interval
	//		 angle ratio
	//		 tick color rgba
	//		 isMajor)

	void drawLabelSet(float, float, float, int, float, float, float*, Fontinfo);	// Draw label set
	//		(start label,
	//		 stop label,
	//		 label increment,
	//		 labeldecimal places, 
	//		 label start angle,
	//		 label stopAngle,
	//		 labelColor rgba,
	//		 labelFont)
	
	void drawNeedle(float angle);

	
	float degToRad(float);						// Degrees to radians			(degrees)
};

#endif