// This is the main project file

/***********************************************************************************************************************************
							INCLUDES
***********************************************************************************************************************************/

using namespace std;
#include <iostream>
#include <stdio.h>

#include <string>

#include "VG/openvg.h"		// OpenVG
#include "VG/vgu.h"			// OpenVG
#include "fontinfo.h"		// OpenVG	
#include "shapes.h"			// OpenVG
#include <stdlib.h>			// OpenVG

#include "math.h"

#include "Gauge.h" // Access to our Gauge class and member functions!
#include "serial.h"
#include "DataStream.h"

#include<bcm2835.h>

// *** Label Fonts & other includes (for sprintf) ***
#include "avengeance.inc"
#include "digits.inc"


// This is for the touchsceen stuff and NEEDS to occur before include touchScreen.h
#include <linux/input.h>
#include <fcntl.h>
#include <pthread.h>

// Access to our touchscreen functions
#include "touchScreen.h"

#include "Project.h"
#include "Button.h"

/***********************************************************************************************************************************
							VARIABLE DECLARATIONS
***********************************************************************************************************************************/

// Label and readout fonts
Fontinfo avengeance;
Fontinfo digits;






touch_t touch;
int quitState = 0;



/***********************************************************************************************************************************
							PROTOTYPES
***********************************************************************************************************************************/
void setupGraphics(int*, int*);


/***********************************************************************************************************************************
							MAIN
***********************************************************************************************************************************/

int main(){
	
	

	
	if(!bcm2835_init())
	return 1;
	
	int width, height;
	//float gaugeColor[] = {0 , 1, 0, 1};
	float gaugeColor[] = {1 , 0, 127.0/255.0, 1};
	//float majorTickColor[] = {0, 1, 0, 1};
	float majorTickColor[] = {1 , 0, 127.0/255.0, 1};
	float minorTickColor[] = {1, 1, 1, 1};
	
	float labelColor[] = {102.0/255.0 , 178.0/255.0, 1, 1};
	//float majorTickColor2[] = {0, 1, 0, 1};
	float majorTickColor2[] = {1 , 0, 127.0/255.0, 1};
	float minorTickColor2[] = {1, 1, 1, 1};
	
	
	float black[] = {0, 0, 0, 1};
	float red[] = {1,0,0,1};
	float green[] = {0,1,0,1};
	float blue[]=	{0,0,1,1};
	float white[]=	{1,1,1,1};
	
	/****************************************************************
		Initialize Screen Graphics
	****************************************************************/
	setupGraphics(&width,&height);	
	
	/****************************************************************
		Serial Communication
	****************************************************************/
	int uart0_filestream = openSerial(); // opens the serial communication
	
	
	/****************************************************************
		touch Thread Creation: THis is for reading touch data from touchscreen actions
	****************************************************************/
	if (touchinit(width, height) != 0) {
		fprintf(stderr, "Unable to initialize the touch\n");
		exit(1);
	}
	
	/****************************************************************
		Load Fonts
	****************************************************************/
	avengeance = loadfont(avengeance_glyphPoints, 
	avengeance_glyphPointIndices, 
	avengeance_glyphInstructions,                
	avengeance_glyphInstructionIndices, 
	avengeance_glyphInstructionCounts, 
	avengeance_glyphAdvances,
	avengeance_characterMap, 
	avengeance_glyphCount);
	digits = loadfont(digits_glyphPoints, 
	digits_glyphPointIndices, 
	digits_glyphInstructions,                
	digits_glyphInstructionIndices, 
	digits_glyphInstructionCounts, 
	digits_glyphAdvances,
	digits_characterMap, 
	digits_glyphCount);
	
	
	/****************************************************************
		Create DATA STREAM Object
	****************************************************************/
	DataStream BoostDataStream(2);
	
	BoostDataStream.setCharTags('A' , 'B');
	BoostDataStream.setStreamScaling(100.0);
	
	BoostDataStream.setRangeScaling(1.0,1); //for the PSI positive data values. data comes in from arduino for boost gauge as PSI*streamscaling factor
	
	
	BoostDataStream.setRangeLimits(0,30,1);
	string eu1="PSI";
	BoostDataStream.setEngUnits(eu1,1);
	BoostDataStream.setWeightedMALag(3,1);
	float coefficients[]={0.25,0.25,0.5};
	BoostDataStream.setWeightedMACoeffs(coefficients,1);
	BoostDataStream.setSimpleMALag(3,1);
	
	// Configure the second range
	
	BoostDataStream.setRangeScaling(-2.036,2); //for the PSI positive data values. data comes in from arduino for boost gauge as PSI*streamscaling factor
	//cout<<"hi"<<endl;
	BoostDataStream.setRangeLimits(-14.734,0,2); // this is PSI equivlanet of 30 inHG, we are rejecting larger magnitude values
	//cout<<"hi"<<endl;
	string eu2="inHG";
	//cout<<"hi"<<endl;
	BoostDataStream.setEngUnits(eu2,2);
	//cout<<"hi"<<endl;
	BoostDataStream.setWeightedMALag(3,2);
	
	BoostDataStream.setWeightedMACoeffs(coefficients,2);
	BoostDataStream.setSimpleMALag(3,2);
	

	
	/****************************************************************
		Create BOOST GAUGE Object
	****************************************************************/
	Gauge BoostGauge(width/6, height/2, width/6 , 2); // used the constructor of class Gauge to instantiate object BoostGauge
	
	
	BoostGauge.setEngUnits(eu1, 1);
	BoostGauge.setEngUnits(eu2, 2);

	BoostGauge.setBorderColor(gaugeColor);
	BoostGauge.setBackgroundColor(black);
	BoostGauge.setNeedleColor(gaugeColor);
	
	
	
	//Set up Boost Gauge Object Range 1
	
	
	BoostGauge.setDataRange(0 , 30, 1); // Always provide in stern order: min value first, then max
	BoostGauge.setDataAngleRange(0,-180, 1);
	BoostGauge.setMajorInterval(5,1);
	BoostGauge.setMinorInterval(1,1);
	BoostGauge.setMajorTickColor(majorTickColor, 1);
	BoostGauge.setMinorTickColor(minorTickColor, 1);
	
	
	BoostGauge.setLabelFont(avengeance, 1);
	BoostGauge.setLabelColor(labelColor, 1);			// Set label label color 		(rgba, range #)
	BoostGauge.setLabelRange(0, 30, 1);		// Set label display range 		(min, max, range #)
	BoostGauge.setLabelAngleRange(0, 180, 1);	// Set label angle range 		(min, max, range #)
	BoostGauge.setLabelIncrement(5, 1);			// Set label increment 			(increment, range #)
	BoostGauge.setLabelDecPlaces(0, 1);			// Set label # of dec places 	(# of decimal places, range #)
	BoostGauge.setLabelFont( avengeance,1);				// Set label font				(font)
	
	
	//Set up Boost Gauge Object Range 2

	
	BoostGauge.setDataRange(0 , 30, 2); // Always provide in stern order: min value first, then max
	BoostGauge.setDataAngleRange(0,90, 2); 
	BoostGauge.setMajorInterval(10,2);
	BoostGauge.setMinorInterval(2, 2);
	BoostGauge.setMajorTickColor(majorTickColor2, 2);
	BoostGauge.setMinorTickColor(minorTickColor2, 2);
	
	
	BoostGauge.setLabelFont(avengeance, 2);
	BoostGauge.setLabelColor(labelColor, 2);			// Set label label color 		(rgba, range #)
	BoostGauge.setLabelRange(10, 30, 2);		// Set label display range 		(min, max, range #)
	BoostGauge.setLabelAngleRange(-30, -90, 2);	// Set label angle range 		(min, max, range #)
	BoostGauge.setLabelIncrement(10, 2);			// Set label increment 			(increment, range #)
	BoostGauge.setLabelDecPlaces(0, 2);			// Set label # of dec places 	(# of decimal places, range #)
	BoostGauge.setLabelFont( avengeance,2);				// Set label font				(font)
	

	BoostGauge.touchEnable(); 	//Enable the touch functionaltiy  of this touchable object gauge object BoostGauge object

	/****************************************************************
		Create BUTTON objects
	****************************************************************/
	// Screen Refresh Rate Button:
	
	Button ScreenRRButton(width/2, height/2, 100, 50);
	ScreenRRButton.setBorder(green, 2); 
	string textString = "Hz update";
	ScreenRRButton.enableText('B'); 
	ScreenRRButton.setText(textString);
	
	ScreenRRButton.enableValue('T');
	ScreenRRButton.setValueDecPlaces(2);
	ScreenRRButton.setValueRefreshRate(5);
	ScreenRRButton.setValueColor(green);
	
	ScreenRRButton.touchEnable();
	
	// Exit Button:
	
	// Button constructor: center X, center Y, width, height
	// Button ExitButton(width-50, -50/2,100,50);
	Button ExitButton(width-50, -22,100,50);
	ExitButton.setBorder(red,2);
	string ExitButtonText = "X";
	ExitButton.setTextColor(red);
	ExitButton.enableText('C');
	ExitButton.setText(ExitButtonText);
	ExitButton.touchEnable();
	
	Button ExitButton1(width-150-10, -22,100,50);
	ExitButton1.setBorder(green,2);
	string ExitButtonText1 = "X";
	ExitButton1.setTextColor(green);
	ExitButton1.enableText('C');
	ExitButton1.setText(ExitButtonText1);
	ExitButton1.touchEnable();
	
	Button ExitButton2(width-250-20, -22,100,50);
	ExitButton2.setBorder(white,2);
	string ExitButtonText2 = "X";
	ExitButton2.setTextColor(white);
	ExitButton2.enableText('C');
	ExitButton2.setText(ExitButtonText2);
	ExitButton2.touchEnable();
	
	Button ExitButton3(width-350-30, -22,100,50);
	ExitButton3.setBorder(blue,2);
	string ExitButtonText3 = "X";
	ExitButton3.setTextColor(blue);
	ExitButton3.enableText('C');
	ExitButton3.setText(ExitButtonText3);
	ExitButton3.touchEnable();
	
	Button ExitButton4(width-450-40, -22,100,50);
	ExitButton4.setBorder(red,2);
	string ExitButtonText4 = "X";
	ExitButton4.setTextColor(green);
	ExitButton4.enableText('C');
	ExitButton4.setText(ExitButtonText4);
	ExitButton4.touchEnable();
	
	Button ExitButton5(width-550-50, -22,100,50);
	ExitButton5.setBorder(blue,2);
	string ExitButtonText5 = "X";
	ExitButton5.setTextColor(white);
	ExitButton5.enableText('C');
	ExitButton5.setText(ExitButtonText5);
	ExitButton5.touchEnable();
	
	
	
	
	
	
	
	/****************************************************************
		Loop through program start sequence
	****************************************************************/
	
	
	
	// Image(VGfloat x, VGfloat y, int w, int h, char * filename)
	/* 	for(float alpha=0;alpha<=1;alpha+=0.01){
	Image(0, 0, width, height, "digitalsolutions.jpg");
	//Background(0, 0, 0,alpha);
	Fill(0,0,0,alpha);
	Rect(0,0,width,height);
	End();//This ends the picture
	
	bcm2835_delay(10);

	}
	
	Background(0, 0, 0); */
	
	
	/****************************************************************
		Draw Created Objects
	****************************************************************/

	BoostGauge.draw();
	ScreenRRButton.draw();
	ExitButton.draw();
	ExitButton1.draw();
	ExitButton2.draw();
	ExitButton3.draw();
	ExitButton4.draw();
	ExitButton5.draw();
	End();//This ends the picture
	
	
	/****************************************************************
		Create + Draw {NEXT}Object
	****************************************************************/
	
	/****************************************************************
		Create + Draw {NEXT}Object
	****************************************************************/

	
	
	/****************************************************************
		Execution Loop
	****************************************************************/
	// currentTime1 = bcm2835_st_read();
	
	
	while(1){
		// Read the current time
		uint64_t loopTime = bcm2835_st_read();
		char serialData[256];
		readSerial(uart0_filestream , serialData);
		
		/****************************************************************
			Update all Data Stream Objects with New Data 
		****************************************************************/
		
		
		BoostDataStream.update( serialData, loopTime );
		//ScreenRRButton.update(BoostDataStream.getRawUpdateRate());
		
		ScreenRRButton.setValue(BoostDataStream.getRawUpdateRate());
		ScreenRRButton.update();
		ScreenRRButton.updateTouch(touch);
		
	

		if (ScreenRRButton.isTouched())
		{
			std::cout << "ScreenRRButton was touched!!!" << endl;
			
		}


		//exitButton.move(0,1,1);

		ExitButton.update();
		ExitButton1.update();
		ExitButton2.update();
		ExitButton3.update();
		ExitButton4.update();
		ExitButton5.update();
		
		ExitButton.updateTouch(touch);
		ExitButton1.updateTouch(touch);
		ExitButton2.updateTouch(touch);
		ExitButton3.updateTouch(touch);
		ExitButton4.updateTouch(touch);
		ExitButton5.updateTouch(touch);
		
		ExitButton.updateVisuals();
		ExitButton1.updateVisuals();
		ExitButton2.updateVisuals();
		ExitButton3.updateVisuals();
		ExitButton4.updateVisuals();
		ExitButton5.updateVisuals();
		
	
		
		
		if (ScreenRRButton.isTouched())
		{
			std::cout << "screen RR Button was touched!!!" << endl;
			// void TouchableObject::move(int finalX, int finalY, int transTime, string motionType )
			
			if(ExitButton.getCurrentPosY() <0){
			ExitButton.move(width-50, 25,250,"emptyForNow");
			ExitButton1.move(width-150-10, 25, 250+50,"emptyForNow");
			ExitButton2.move(width-250-20,25, 250+100, "emptyForNow");
			ExitButton3.move(width-350-30,25,250+150,"emptyForNow");
			ExitButton4.move(width-450-40,25,250+200,"emptyForNow");
			ExitButton5.move(width-550-50,25,300+250,"emptyForNow");
			}
			
			if(ExitButton.getCurrentPosY() >0){
			ExitButton.move(width-50, -20,250,"emptyForNow");
			ExitButton1.move(width-150-10, -22, 250+50,"emptyForNow");
			ExitButton2.move(width-250-20,-22, 250+100, "emptyForNow");
			ExitButton3.move(width-350-30,-22,250+150,"emptyForNow");
			ExitButton4.move(width-450-40,-22,250+200,"emptyForNow");
			ExitButton5.move(width-550-50,-22,300+250,"emptyForNow");
			}
			
			
		}
		
		
		
		
		
		/****************************************************************
			Update all Gauge Objects with New Data 
		****************************************************************/
		BoostGauge.update(BoostDataStream.getWeightedMADatum(), BoostDataStream.getEngUnits()); //Temp comment: 0 for a junk input for now, just draws a nonupdating needle
		End();//This ends the picture
		
		/****************************************************************
			Update all Touchable Objects in touchable sense with New Data 
		****************************************************************/
		
		BoostGauge.updateTouch(touch);
		
		if(BoostGauge.isTouched()){
			std::cout<<"The Boost Gauge was Touched!!!!"<<'\n';
		}
		
		
		
	}
	
	

	
}


/***********************************************************************************************************************************
							DEFINE SETUPGRAPHICS FUNCTION
***********************************************************************************************************************************/

void setupGraphics(int* widthPtr, int* heightPtr){
	init(widthPtr, heightPtr); //INIT Graphics

	Start(*widthPtr, *heightPtr);	//this should go in setup			   // Start the picture
	Background(0, 0, 0);	//this should go in setup	
	
}

