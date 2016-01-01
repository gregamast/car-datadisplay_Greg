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
#include "serial2.h"
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
#include "Menu.h"
#include "TextView.h"



/***********************************************************************************************************************************
							VARIABLE DECLARATIONS
***********************************************************************************************************************************/

// Label and readout fonts

Fontinfo avengeance;
Fontinfo digits; 

touch_t touch;
int quitState = 0;

VGImage backgroundImage;

// vector<Gauge> GObjects;
// vector<Button> BObjects;

vector<TouchableObject> TObjects;

uint64_t loopTime;

float sendcolor[] = {1.0, 0.4, 0.4, 1.0};

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
	
	
	
	// Label and readout fonts
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
	BoostDataStream.setRangeLimits(-14.734,0,2); // this is PSI equivlanet of 30 inHG, we are rejecting larger magnitude values
	string eu2="inHG";
	BoostDataStream.setEngUnits(eu2,2);
	BoostDataStream.setWeightedMALag(3,2);
	BoostDataStream.setWeightedMACoeffs(coefficients,2);
	BoostDataStream.setSimpleMALag(3,2);
	

	// Color definitions (float r, float g, float b, float alpha)
	float black[] = {0,0,0,1};
	float green[] = {0,1,0,1};
	float red[] = {1,0,0,1};
	float blue[] = {0,0,1,1};
	float white[] = {1,1,1,1};
	float translucentBlack[] = {0,0,0,0.5};
	float gray[] = {0.43,0.43,0.43,1};

	TextView textView1(width/6, height/2, width/3-15, width/3-15, "TextView1");
	textView1.update();

	bool menuActive = false;

	Menu Mode1Menu(width/2, height-40, width-20, 60, "Mode1Menu");
	Mode1Menu.touchEnable();
	//Mode1Menu.selectButton("m1");

	Menu TestCommandMenu(width-width/4, height/2-40, width/2-20, height-80, "TestCommandMenu");
	TestCommandMenu.touchEnable();

	Serial ELMSerial("/dev/ELM327", 5);
	ELMSerial.serialWrite("ATZ");

	TextView SerialViewer(width/4, height/2-40, width/2-20, height-80, "SerialViewer");

	/****************************************************************
		Create GAUGE OBJECTS
	****************************************************************/
	
	// GObjects.push_back( Gauge(width/6, height/2, width/6 , "BoostGauge") );
	// GObjects.push_back( Gauge( (width/6)*3, height/2, width/6 , "TempGauge" ) );
	// GObjects.push_back( Gauge( (width/6)*5, height/2, width/6 , "BoostGauge2" ) );
	

	/****************************************************************
		Create BUTTON objects
	****************************************************************/
	// BObjects.push_back( Button(width/2, height/2, width, height ,"BackgroundTouchButton") );
	// BObjects.push_back( Button( width-300, height/2, 100, 50 , "FramerateButton" ));
	// BObjects.push_back( Button( width-50, -22,100,50 , "MenuButton7" ));
	// BObjects.push_back( Button( width-150-10, -22,100,50 , "MenuButton6" ));
	// BObjects.push_back( Button( width-250-20, -22,100,50 , "MenuButton5" ));
	// BObjects.push_back( Button( width-350-30, -22,100,50, "MenuButton4" ));
	// BObjects.push_back( Button( width-450-40, -22,100,50, "MenuButton3" ));
	// BObjects.push_back( Button( width-550-50, -22,100,50, "MenuButton2" ));
	// BObjects.push_back( Button( width-650-60, -22,100,50, "MenuButton1" ));

	// BObjects.push_back( Button( width-650-60, -22,100,50 ,"MenuButton_DD1") );
	// BObjects.push_back( Button( width-550-50, -22,100,50 ,"MenuButton_DD2") );
	// BObjects.push_back( Button( width-450-40, -22,100,50 ,"MenuButton_DD3") );

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
	
	
	// for(int i=0; i<GObjects.size() ; i++){
		// GObjects[i].touchEnable();
	// }
	
	// for(int i=0; i<BObjects.size() ; i++){
		// BObjects[i].touchEnable();
	// }
	
	
	/****************************************************************
		Fill display buffer (currently no draw for button, need to remove the whole "draw " idea in general. It is not draw but filling buffer space. End draws the buffer on the scren)
	****************************************************************/
	// // std::cin.ignore();
	// for(int i=0; i<GObjects.size() ; i++){
		// GObjects[i].draw();
	// }


	/****************************************************************
		DRAW all created objects
	****************************************************************/
	End();//This ends the picture
	
	while(1) {
		loopTime = bcm2835_st_read();
		char serialData[256];
		readSerial(uart0_filestream, serialData);			// Capture serial data
		BoostDataStream.update(serialData, loopTime);		// Update datastream with serial data

		// Grab touch data at the begining of each loop and 
		//loopTouch = touch;
		// Draw background image
		vgSetPixels(0, 0, backgroundImage, 0, 0, 800, 480);

		Mode1Menu.update(touch);
		TestCommandMenu.update(touch);

		string testCommand = TestCommandMenu.getPressedButtonName();
		if(!testCommand.empty()){
			SerialViewer.addNewLine(testCommand, sendcolor);
			ELMSerial.serialWrite(testCommand);
			TestCommandMenu.selectButton(testCommand);
		}


		SerialViewer.update();
		

		string data = ELMSerial.serialRead();

		float receivecolor[] = {0.4, 0.69, 1.0, 1.0};
		if(!data.empty()) {
			cout << "Data: " << endl << data << endl;
			cout << "Data characters: " << endl;
			for(int idx=0; idx<data.size(); idx++)
				cout << (int)data[idx] << " ";
			cout << endl;
			SerialViewer.addNewLine(data, receivecolor);
		}

		if(Mode1Menu.isButtonPressed("m1")) {
			Mode1Menu.selectButton("m1");
			SerialViewer.addNewLine("ATZ", sendcolor);
			ELMSerial.serialWrite("ATZ");

		}
		if(Mode1Menu.isButtonPressed("m2")) {
			Mode1Menu.selectButton("m2");
			SerialViewer.addNewLine("ATE0", sendcolor);
			ELMSerial.serialWrite("ATE0");

		}
		if(Mode1Menu.isButtonPressed("m3")) {
			Mode1Menu.selectButton("m3");
			SerialViewer.addNewLine("", sendcolor);
			ELMSerial.serialWrite("\n");

		}
		if(Mode1Menu.isButtonPressed("m4")) {
			Mode1Menu.selectButton("m4");
			SerialViewer.clear();
		}
		if(Mode1Menu.isButtonPressed("m5")) Mode1Menu.selectButton("m5");
		if(Mode1Menu.isButtonPressed("m6")) Mode1Menu.selectButton("m6");

		if(!Mode1Menu.isHidden() && Mode1Menu.isPressedOutside())
			{
				Mode1Menu.hide();
				cout << "trying to hide menu" << endl;
			}
		else if(Mode1Menu.isHidden() && Mode1Menu.isPressedOutside())
			{
				Mode1Menu.unhide();
				cout << "trying to hide menu" << endl;
			}



		End();
	}
}

// setupGraphics()
void setupGraphics(int* widthPtr, int* heightPtr) {	
	init(widthPtr,heightPtr);
	Start(*widthPtr, *heightPtr);		//Set up graphics, start picture
	Background(0,0,0);
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

