// This is the main project file

/***********************************************************************************************************************************
							INCLUDES
***********************************************************************************************************************************/


using namespace std;
#include <iostream>
#include <stdio.h>
#include <array>

#include <vector>

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

VGImage backgroundImage;




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
	float blackLowA[] = {0, 0, 0, 0.75};
	float red[] = {1,0,0,1};
	float green[] = {0,1,0,1};
	float blue[]=	{0,0,1,1};
	float white[]=	{1,1,1,1};
	
	float blackLowA1[] = {0 , 0 , 0 , 0.6};
	
	
	
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
	

	Gauge BoostGauge(width/6, height/2, width/6);
	
	string configType;
	
	
	
	configType = "BoostGauge";
	BoostGauge.configure(configType);


	// BoostGauge.setBorderColor(blue);
	
	// BoostGauge.setBackgroundColor(blackLowA1);
	// BoostGauge.setNeedleColor(white);
	
	BoostGauge.touchEnable(); 	//Enable the touch functionaltiy  of this touchable object gauge object BoostGauge object
	
	//Set up Boost Gauge Object Range 1
	
	
	// BoostGauge.setDataRange(0 , 30, 1); // Always provide in stern order: min value first, then max
	// BoostGauge.setDataAngleRange(0,-180, 1);
	// BoostGauge.setMajorInterval(5,1);
	// BoostGauge.setMinorInterval(1,1);
	//BoostGauge.setMajorTickColor(white, 1);
	//BoostGauge.setMinorTickColor(white, 1);
	
	
	BoostGauge.setLabelFont(avengeance, 1);
	// BoostGauge.setLabelColor(white, 1);			// Set label label color 		(rgba, range #)
	// BoostGauge.setLabelRange(0, 30, 1);		// Set label display range 		(min, max, range #)
	// BoostGauge.setLabelAngleRange(0, 180, 1);	// Set label angle range 		(min, max, range #)
	// BoostGauge.setLabelIncrement(5, 1);			// Set label increment 			(increment, range #)
	// BoostGauge.setLabelDecPlaces(0, 1);			// Set label # of dec places 	(# of decimal places, range #)
	// BoostGauge.setLabelFont( avengeance,1);				// Set label font				(font)
	
	//Set up Boost Gauge Object Range 2

	// BoostGauge.setDataRange(0 , 30, 2); // Always provide in stern order: min value first, then max
	// BoostGauge.setDataAngleRange(0,90, 2); 
	// BoostGauge.setMajorInterval(10,2);
	// BoostGauge.setMinorInterval(2, 2);
	// BoostGauge.setMajorTickColor(red, 2);
	// BoostGauge.setMinorTickColor(red, 2);
	
	BoostGauge.setLabelFont(avengeance, 2);
	//BoostGauge.setLabelColor(white, 2);			// Set label label color 		(rgba, range #)
	// BoostGauge.setLabelRange(10, 30, 2);		// Set label display range 		(min, max, range #)
	// BoostGauge.setLabelAngleRange(-30, -90, 2);	// Set label angle range 		(min, max, range #)
	// BoostGauge.setLabelIncrement(10, 2);			// Set label increment 			(increment, range #)
	// BoostGauge.setLabelDecPlaces(0, 2);			// Set label # of dec places 	(# of decimal places, range #)
	// BoostGauge.setLabelFont( avengeance,2);				// Set label font				(font)
	
	
	
	
	
	
	// Gauge *myGauge;
	// myGauge = &BoostGauge;
	// myGauge->setLabelDecPlaces(0,2);
	/****************************************************************
		Create TEMPERATURE GAUGE Object
	****************************************************************/
	// Gauge(int x, int y, int r, int ranges)
	Gauge TempGauge((width/6)*3, height/2, width/6 ); // used the constructor of class Gauge to instantiate object BoostGauge
	
	configType = "TempGauge";
	TempGauge.configure(configType);
	
	// TempGauge.setNumRanges(2);
	// TempGauge.setEngUnits(eu1, 1);
	// TempGauge.setEngUnits(eu2, 2);

	TempGauge.setBorderColor(blue);
	
	TempGauge.setBackgroundColor(blackLowA1);
	TempGauge.setNeedleColor(red);
	
	TempGauge.touchEnable(); 	//Enable the touch functionaltiy  of this touchable object gauge object TempGauge object
	
	//Set up Boost Gauge Object Range 1
	
	
	TempGauge.setDataRange(0 , 10, 1); // Always provide in stern order: min value first, then max
	TempGauge.setDataAngleRange(0,-90-45, 1);
	TempGauge.setMajorInterval(5,1);
	TempGauge.setMinorInterval(1,1);
	//TempGauge.setMajorTickColor(red, 1);
	//TempGauge.setMinorTickColor(red, 1);
	
	
	TempGauge.setLabelFont(avengeance, 1);
	//TempGauge.setLabelColor(white, 1);			// Set label label color 		(rgba, range #)
	TempGauge.setLabelRange(0, 10, 1);		// Set label display range 		(min, max, range #)
	TempGauge.setLabelAngleRange(0, 90+45, 1);	// Set label angle range 		(min, max, range #)
	TempGauge.setLabelIncrement(5, 1);			// Set label increment 			(increment, range #)
	TempGauge.setLabelDecPlaces(0, 1);			// Set label # of dec places 	(# of decimal places, range #)
	TempGauge.setLabelFont( avengeance,1);				// Set label font				(font)
	
	//Set up Boost Gauge Object Range 2

	TempGauge.setDataRange(0 , 25, 2); // Always provide in stern order: min value first, then max
	TempGauge.setDataAngleRange(0,90, 2); 
	TempGauge.setMajorInterval(10,2);
	TempGauge.setMinorInterval(2, 2);
	//TempGauge.setMajorTickColor(blue, 2);
	//TempGauge.setMinorTickColor(blue, 2);
	
	TempGauge.setLabelFont(avengeance, 2);
	//TempGauge.setLabelColor(white, 2);			// Set label label color 		(rgba, range #)
	TempGauge.setLabelRange(10, 25, 2);		// Set label display range 		(min, max, range #)
	TempGauge.setLabelAngleRange(-30, -90-45, 2);	// Set label angle range 		(min, max, range #)
	TempGauge.setLabelIncrement(10, 2);			// Set label increment 			(increment, range #)
	TempGauge.setLabelDecPlaces(0, 2);			// Set label # of dec places 	(# of decimal places, range #)
	TempGauge.setLabelFont( avengeance,2);				// Set label font				(font)
	
	/****************************************************************
		Create BOOST GAUGE Object
	****************************************************************/
	// Gauge(int x, int y, int r, int ranges)
	Gauge BoostGauge2((width/6)*5, height/2, width/6 ); // used the constructor of class Gauge to instantiate object BoostGauge2
	
	configType = "BoostGauge2";
	BoostGauge2.configure(configType);
	
	// BoostGauge2.setNumRanges(2);
	// BoostGauge2.setEngUnits(eu1, 1);
	// BoostGauge2.setEngUnits(eu2, 2);

	BoostGauge2.setBorderColor(blue);
	
	BoostGauge2.setBackgroundColor(blackLowA1);
	BoostGauge2.setNeedleColor(white);
	
	BoostGauge2.touchEnable(); 	//Enable the touch functionaltiy  of this touchable object gauge object BoostGauge2 object
	
	//Set up Boost Gauge Object Range 1
	
	
	BoostGauge2.setDataRange(0 , 30, 1); // Always provide in stern order: min value first, then max
	BoostGauge2.setDataAngleRange(0,-180, 1);
	BoostGauge2.setMajorInterval(5,1);
	BoostGauge2.setMinorInterval(1,1);
	// BoostGauge2.setMajorTickColor(white, 1);
	// BoostGauge2.setMinorTickColor(white, 1);
	
	
	BoostGauge2.setLabelFont(avengeance, 1);
	//BoostGauge2.setLabelColor(white, 1);			// Set label label color 		(rgba, range #)
	BoostGauge2.setLabelRange(0, 30, 1);		// Set label display range 		(min, max, range #)
	BoostGauge2.setLabelAngleRange(0, 180, 1);	// Set label angle range 		(min, max, range #)
	BoostGauge2.setLabelIncrement(5, 1);			// Set label increment 			(increment, range #)
	BoostGauge2.setLabelDecPlaces(0, 1);			// Set label # of dec places 	(# of decimal places, range #)
	BoostGauge2.setLabelFont( avengeance,1);				// Set label font				(font)
	
	//Set up Boost Gauge Object Range 2

	BoostGauge2.setDataRange(0 , 30, 2); // Always provide in stern order: min value first, then max
	BoostGauge2.setDataAngleRange(0,90, 2); 
	BoostGauge2.setMajorInterval(10,2);
	BoostGauge2.setMinorInterval(2, 2);
	// BoostGauge2.setMajorTickColor(red, 2);
	// BoostGauge2.setMinorTickColor(red, 2);
	
	BoostGauge2.setLabelFont(avengeance, 2);
	//BoostGauge2.setLabelColor(white, 2);			// Set label label color 		(rgba, range #)
	BoostGauge2.setLabelRange(10, 30, 2);		// Set label display range 		(min, max, range #)
	BoostGauge2.setLabelAngleRange(-30, -90, 2);	// Set label angle range 		(min, max, range #)
	BoostGauge2.setLabelIncrement(10, 2);			// Set label increment 			(increment, range #)
	BoostGauge2.setLabelDecPlaces(0, 2);			// Set label # of dec places 	(# of decimal places, range #)
	BoostGauge2.setLabelFont( avengeance,2);				// Set label font				(font)
	/****************************************************************
		Create BUTTON objects
	****************************************************************/
	
	// // std::cin.ignore();
	// Screen Refresh Rate Button:
	
	Button ScreenRRButton(width-300, height/2, 100, 50);
	ScreenRRButton.setBorder(white, 2); 
	string textString = "Hz update";
	ScreenRRButton.enableText('B'); 
	ScreenRRButton.setText(textString);
	float blueLowA[]={0,0,1,0.75};
	ScreenRRButton.setBackgroundColor(blackLowA);
	
	ScreenRRButton.enableValue('T');
	ScreenRRButton.setValueDecPlaces(2);
	ScreenRRButton.setValueRefreshRate(5);
	ScreenRRButton.setValueColor(white);
	
	ScreenRRButton.touchEnable();
	
	// Exit Button:
	
	// Button constructor: center X, center Y, width, height
	// Button ExitButton(width-50, -50/2,100,50);
	Button ExitButton(width-50, -22,100,50);
	ExitButton.setBackgroundColor(blackLowA);
	ExitButton.setBorder(red,2);
	string ExitButtonText = "1";
	ExitButton.setTextColor(red);
	ExitButton.enableText('C');
	ExitButton.setText(ExitButtonText);
	ExitButton.touchEnable();
	
	Button ExitButton1(width-150-10, -22,100,50);
	ExitButton1.setBackgroundColor(blackLowA);
	ExitButton1.setBorder(white,2);
	string ExitButtonText1 = "2";
	ExitButton1.setTextColor(white);
	ExitButton1.enableText('C');
	ExitButton1.setText(ExitButtonText1);
	ExitButton1.touchEnable();
	
	Button ExitButton2(width-250-20, -22,100,50);
	ExitButton2.setBackgroundColor(blackLowA);
	ExitButton2.setBorder(white,2);
	string ExitButtonText2 = "3";
	ExitButton2.setTextColor(white);
	ExitButton2.enableText('C');
	ExitButton2.setText(ExitButtonText2);
	ExitButton2.touchEnable();
	
	Button ExitButton3(width-350-30, -22,100,50);
	ExitButton3.setBackgroundColor(blackLowA);
	ExitButton3.setBorder(white,2);
	string ExitButtonText3 = "4";
	ExitButton3.setTextColor(white);
	ExitButton3.enableText('C');
	ExitButton3.setText(ExitButtonText3);
	ExitButton3.touchEnable();
	
	Button ExitButton4(width-450-40, -22,100,50);
	ExitButton4.setBackgroundColor(blackLowA);
	ExitButton4.setBorder(white,2);
	string ExitButtonText4 = "5";
	ExitButton4.setTextColor(white);
	ExitButton4.enableText('C');
	ExitButton4.setText(ExitButtonText4);
	ExitButton4.touchEnable();
	
	Button ExitButton5(width-550-50, -22,100,50);
	ExitButton5.setBackgroundColor(blackLowA);
	ExitButton5.setBorder(white,2);
	string ExitButtonText5 = "6";
	ExitButton5.setTextColor(white);
	ExitButton5.enableText('C');
	ExitButton5.setText(ExitButtonText5);
	ExitButton5.touchEnable();
	
	Button ExitButton6(width-650-60, -22,100,50);
	ExitButton6.setBackgroundColor(blackLowA);
	ExitButton6.setBorder(white,2);
	string ExitButtonText6 = "Home :)";
	ExitButton6.setTextColor(white);
	ExitButton6.enableText('C');
	ExitButton6.setText(ExitButtonText6);
	ExitButton6.touchEnable();
	
	

	
	
	/****************************************************************
		Loop through program start sequence
	****************************************************************/
	// // std::cin.ignore();
	
	
	// Image(VGfloat x, VGfloat y, int w, int h, char * filename)
	
	/* 		for(float alpha=1;alpha>=0;alpha-=0.01){
			Image(-300, -100, width+400, height+400, "mickey.jpg");
	
	//Background(0, 0, 0,alpha);
	Fill(0,0,0,alpha);
	Rect(0,0,width,height);
	End();//This ends the picture
	
	bcm2835_delay(1);
	

	}
	
// // std::cin.ignore();
	
	Background(0, 0, 0); */
	
	//Image(0, 0, width, height, "bgImage2.jpg");
	Image(0, 0, width, height, "carbon.jpg");
	backgroundImage = vgCreateImage(VG_sABGR_8888, width, height, VG_IMAGE_QUALITY_BETTER);
	vgGetPixels(backgroundImage , 0 , 0 , 0 , 0 , width , height);
	
	/****************************************************************
		Draw Created Objects (currently no draw for button, need to remove the whole "draw " idea in general. It is not draw but filling buffer space. End draws the buffer on the scren)
	****************************************************************/
	// // std::cin.ignore();
	
	BoostGauge.draw();
	TempGauge.draw();
	BoostGauge2.draw();
	
	Gauge *gaugeArray[]={&BoostGauge , &TempGauge, &BoostGauge2};
	Button *buttonArray[]={&ScreenRRButton, &ExitButton, &ExitButton1 , &ExitButton2, &ExitButton3, &ExitButton4, &ExitButton5, &ExitButton6 };


	


	/****************************************************************
		DRAW all created objects
	****************************************************************/
	End();//This ends the picture
	
	
	//// // std::cin.ignore();
	
	/****************************************************************
		Execution Loop
	****************************************************************/
	// currentTime1 = bcm2835_st_read();
	
	
	while(1){
		
		/****************************************************************
			Set background image into buffer - for every loop 
		****************************************************************/
		vgSetPixels(0, 0, backgroundImage, 0 , 0 , width , height);
		

		
		/****************************************************************
			Time
		****************************************************************/
		// Read the current time
		uint64_t loopTime = bcm2835_st_read();
		
		/****************************************************************
			Update all Touchable Objects with new touch Data 
		****************************************************************/

		
		for(int i=0;i<=2;i++){
			gaugeArray[i]->updateTouch(touch);
		}
		for(int i=0;i<=7;i++){
			buttonArray[i]->updateTouch(touch);
		}
		
		/****************************************************************
			READ serial line input data 
		****************************************************************/
		char serialData[256];
		readSerial(uart0_filestream , serialData);
		
		/****************************************************************
			Update all Data Stream Objects with New Data 
		****************************************************************/
		
		BoostDataStream.update( serialData, loopTime );
		
		/****************************************************************
			Update all Gauge Objects with New Data 
					- runs updateVisuals
		****************************************************************/
		
		ScreenRRButton.setValue(BoostDataStream.getRawUpdateRate());
		
		for(int i=0;i<=2;i++){
			gaugeArray[i]->update(BoostDataStream.getWeightedMADatum(), BoostDataStream.getEngUnits());
		}
		for(int i=0;i<=7;i++){
			buttonArray[i]->update();
		}
		
		
		
		/****************************************************************
			MAIN MENU Logic to process touch
		****************************************************************/

		
		if (BoostGauge.isTouched())
		{
			//std::cout << "screen RR Button was touched!!!" << endl;
			// void TouchableObject::move(int finalX, int finalY, int transTime, string motionType )
			
			
			
			if(ExitButton.getCurrentPosY() <0){
				
				for(int i=0;i<=2;i++){
					gaugeArray[i]->update(BoostDataStream.getWeightedMADatum(), BoostDataStream.getEngUnits());
				}
				
				BoostGauge.fade(75 ,250+60*6,"emptyForNow");
				TempGauge.fade(75 ,250+60*6,"emptyForNow");
				BoostGauge2.fade(75 ,250+60*6,"emptyForNow");
				ScreenRRButton.fade(75, 250+60*6,"emptyForNow" );
				
				ExitButton.move(width-50, 25,250,"emptyForNow");
				ExitButton.fade( 0 ,250,"emptyForNow");
				
				ExitButton1.move(width-150-10, 25, 250+60*1,"emptyForNow");
				ExitButton1.fade( 0,250+60*1,"emptyForNow");
				
				ExitButton2.move(width-250-20,25, 250+60*2, "emptyForNow");
				ExitButton2.fade( 0,250+60*2,"emptyForNow");
				
				ExitButton3.move(width-350-30,25,250+60*3,"emptyForNow");
				ExitButton3.fade( 0,250+60*3,"emptyForNow");
				
				ExitButton4.move(width-450-40,25,250+60*4,"emptyForNow");
				ExitButton4.fade( 0,250+60*4,"emptyForNow");
				
				ExitButton5.move(width-550-50,25,250+60*5,"emptyForNow");
				ExitButton5.fade( 0,250+60*5,"emptyForNow");
				
				ExitButton6.move(width-650-60,25,250+60*6,"emptyForNow");
				ExitButton6.fade( 0,250+60*6,"emptyForNow");
				
			}
			
			if(ExitButton.getCurrentPosY() >0){
				
				BoostGauge.fade(0 ,250+60*6,"emptyForNow");
				TempGauge.fade(0 ,250+60*6,"emptyForNow");
				BoostGauge2.fade(0 ,250+60*6,"emptyForNow");
				ScreenRRButton.fade(0, 250+60*6,"emptyForNow" );
				
				ExitButton.move(width-50, -20,250,"emptyForNow");
				ExitButton.fade( 75,250,"emptyForNow");
				
				ExitButton1.move(width-150-10, -22, 250+60*1,"emptyForNow");
				ExitButton1.fade( 75,250+60*1,"emptyForNow");
				
				ExitButton2.move(width-250-20,-22, 250+60*2, "emptyForNow");
				ExitButton2.fade( 75,250+60*2,"emptyForNow");
				
				ExitButton3.move(width-350-30,-22,250+60*3,"emptyForNow");
				ExitButton3.fade( 75,250+60*3,"emptyForNow");
				
				ExitButton4.move(width-450-40,-22,250+60*4,"emptyForNow");
				ExitButton4.fade( 75,250+60*4,"emptyForNow");
				
				ExitButton5.move(width-550-50,-22,250+60*5,"emptyForNow");
				ExitButton5.fade( 75,250+60*5,"emptyForNow");
				
				ExitButton6.move(width-650-60,-22,250+60*6,"emptyForNow");
				ExitButton6.fade( 75,250+60*6,"emptyForNow");
				
			}
			
			
		}
		

		/****************************************************************
			DRAW all updated objects/visuals 
		****************************************************************/
		// Call the End at the end of each while loop
		End();//This ends the picture
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

