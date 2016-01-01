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
#include "Serial.h"
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

// Fontinfo avengeance;
// Fontinfo digits; 

touch_t touch;
int quitState = 0;

VGImage backgroundImage;

// vector<Gauge> GObjects;
// vector<Button> BObjects;

vector<TouchableObject> TObjects;

int findGObject_identifier(string ident) {
	int idx = 0;
	for(int i = 0; i<GObjects.size(); i++) {
		if(ident.compare(GObjects[i].getIdentifier()) == 0)
		idx =i;
	}
	return idx;
}

int findBObject_identifier(string ident) {
	int idx = 0;
	for(int i = 0; i<BObjects.size(); i++) {
		if(ident.compare(BObjects[i].getIdentifier()) == 0)
		idx =i;
	}
	return idx;
}

int findGObject_group(string ident) {
	int idx = 0;
	for(int i = 0; i<GObjects.size(); i++) {
		if(ident.compare(GObjects[i].getGroup()) == 0)
		idx =i;
	}
	return idx;
}

int findBObject_group(string ident) {
	int idx = 0;
	for(int i = 0; i<BObjects.size(); i++) {
		if(ident.compare(BObjects[i].getGroup()) == 0)
		idx =i;
	}
	return idx;
}

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
	BoostDataStream.setRangeLimits(-14.734,0,2); // this is PSI equivlanet of 30 inHG, we are rejecting larger magnitude values
	string eu2="inHG";
	BoostDataStream.setEngUnits(eu2,2);
	BoostDataStream.setWeightedMALag(3,2);
	BoostDataStream.setWeightedMACoeffs(coefficients,2);
	BoostDataStream.setSimpleMALag(3,2);
	

	
	/****************************************************************
		Create GAUGE OBJECTS
	****************************************************************/
	
	GObjects.push_back( Gauge(width/6, height/2, width/6 , "BoostGauge") );
	GObjects.push_back( Gauge( (width/6)*3, height/2, width/6 , "TempGauge" ) );
	GObjects.push_back( Gauge( (width/6)*5, height/2, width/6 , "BoostGauge2" ) );
	

	/****************************************************************
		Create BUTTON objects
	****************************************************************/
	// BObjects.push_back( Button(width/2, height/2, width, height ,"BackgroundTouchButton") );
	BObjects.push_back( Button( width-300, height/2, 100, 50 , "FramerateButton" ));
	// BObjects.push_back( Button( width-50, -22,100,50 , "MenuButton7" ));
	// BObjects.push_back( Button( width-150-10, -22,100,50 , "MenuButton6" ));
	// BObjects.push_back( Button( width-250-20, -22,100,50 , "MenuButton5" ));
	// BObjects.push_back( Button( width-350-30, -22,100,50, "MenuButton4" ));
	// BObjects.push_back( Button( width-450-40, -22,100,50, "MenuButton3" ));
	// BObjects.push_back( Button( width-550-50, -22,100,50, "MenuButton2" ));
	// BObjects.push_back( Button( width-650-60, -22,100,50, "MenuButton1" ));

	BObjects.push_back( Button( width-650-60, -22,100,50 ,"MenuButton_DD1") );
	BObjects.push_back( Button( width-550-50, -22,100,50 ,"MenuButton_DD2") );
	BObjects.push_back( Button( width-450-40, -22,100,50 ,"MenuButton_DD3") );

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
		set all touchable objects to be touchable
	****************************************************************/
	
	
	for(int i=0; i<GObjects.size() ; i++){
		GObjects[i].touchEnable();
	}
	
	for(int i=0; i<BObjects.size() ; i++){
		BObjects[i].touchEnable();
	}
	
	
	/****************************************************************
		Fill display buffer (currently no draw for button, need to remove the whole "draw " idea in general. It is not draw but filling buffer space. End draws the buffer on the scren)
	****************************************************************/
	// // std::cin.ignore();
	for(int i=0; i<GObjects.size() ; i++){
		GObjects[i].draw();
	}


	/****************************************************************
		DRAW all created objects
	****************************************************************/
	End();//This ends the picture
	
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

		for(int i=0; i<GObjects.size() ; i++){
			GObjects[i].updateTouch(touch);
		}
		
		for(int i=0; i<BObjects.size() ; i++){
			BObjects[i].updateTouch(touch);
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
		

		BObjects[findBObject_identifier("FramerateButton")].setValue(BoostDataStream.getRawUpdateRate());

		
		for(int i=0; i<GObjects.size() ; i++){
			GObjects[i].update(BoostDataStream.getWeightedMADatum(), BoostDataStream.getEngUnits());
		}
		

		for(int i=0; i<BObjects.size() ; i++){
			BObjects[i].update();
		}

		
		
		/****************************************************************
			MAIN MENU Logic to process touch
		****************************************************************/
		

		
		/****************************************************************
			Data Display MAIN MENU Logic to process touch
		****************************************************************/
		
		
	
			

		

		/****************************************************************
			DRAW all updated objects/visuals 
		****************************************************************/
		// Call the End at the end of each while loop
		End();//This ends the picture
		
	} // End inf while loop
	
	

	
} //End of MAIN function


/***********************************************************************************************************************************
							DEFINE SETUPGRAPHICS FUNCTION
***********************************************************************************************************************************/

void setupGraphics(int* widthPtr, int* heightPtr){
	init(widthPtr, heightPtr); //INIT Graphics

	Start(*widthPtr, *heightPtr);	//this should go in setup			   // Start the picture
	Background(0, 0, 0);	//this should go in setup	
	
}






// if ( BoostGauge.isTouched() ){
//std::cout << "screen RR Button was touched!!!" << endl;
//void TouchableObject::move(int finalX, int finalY, int transTime, string motionType )



// if(MenuButton1.getCurrentPosY() <0){

// for(int i=0;i<=2;i++){
// gaugeArray[i]->update(BoostDataStream.getWeightedMADatum(), BoostDataStream.getEngUnits());
// }

// BoostGauge.fade(75 ,250+60*6,"emptyForNow");
// TempGauge.fade(75 ,250+60*6,"emptyForNow");
// BoostGauge2.fade(75 ,250+60*6,"emptyForNow");
// ScreenRRButton.fade(75, 250+60*6,"emptyForNow" );

// MenuButton7.move(width-50, 25,250,"emptyForNow");
// MenuButton7.fade( 0 ,250,"emptyForNow");

// MenuButton6.move(width-150-10, 25, 250+60*1,"emptyForNow");
// MenuButton6.fade( 0,250+60*1,"emptyForNow");

// MenuButton5.move(width-250-20,25, 250+60*2, "emptyForNow");
// MenuButton5.fade( 0,250+60*2,"emptyForNow");

// MenuButton4.move(width-350-30,25,250+60*3,"emptyForNow");
// MenuButton4.fade( 0,250+60*3,"emptyForNow");

// MenuButton3.move(width-450-40,25,250+60*4,"emptyForNow");
// MenuButton3.fade( 0,250+60*4,"emptyForNow");

// MenuButton2.move(width-550-50,25,250+60*5,"emptyForNow");
// MenuButton2.fade( 0,250+60*5,"emptyForNow");

// MenuButton1.move(width-650-60,25,250+60*6,"emptyForNow");
// MenuButton1.fade( 0,250+60*6,"emptyForNow");

// }

// if(MenuButton1.getCurrentPosY() >0){

// BoostGauge.fade(0 ,250+60*6,"emptyForNow");
// TempGauge.fade(0 ,250+60*6,"emptyForNow");
// BoostGauge2.fade(0 ,250+60*6,"emptyForNow");
// ScreenRRButton.fade(0, 250+60*6,"emptyForNow" );

// MenuButton7.move(width-50, -20,250,"emptyForNow");
// MenuButton7.fade( 75,250,"emptyForNow");

// MenuButton6.move(width-150-10, -22, 250+60*1,"emptyForNow");
// MenuButton6.fade( 75,250+60*1,"emptyForNow");

// MenuButton5.move(width-250-20,-22, 250+60*2, "emptyForNow");
// MenuButton5.fade( 75,250+60*2,"emptyForNow");

// MenuButton4.move(width-350-30,-22,250+60*3,"emptyForNow");
// MenuButton4.fade( 75,250+60*3,"emptyForNow");

// MenuButton3.move(width-450-40,-22,250+60*4,"emptyForNow");
// MenuButton3.fade( 75,250+60*4,"emptyForNow");

// MenuButton2.move(width-550-50,-22,250+60*5,"emptyForNow");
// MenuButton2.fade( 75,250+60*5,"emptyForNow");

// MenuButton1.move(width-650-60,-22,250+60*6,"emptyForNow");
// MenuButton1.fade( 75,250+60*6,"emptyForNow");

// }


// }

