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
#include "PID.h"

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

// Color definitions (float r, float g, float b, float alpha)
float black[] = {0,0,0,1};
float green[] = {0,1,0,1};
float red[] = {1,0,0,1};
float blue[] = {0,0,1,1};
float white[] = {1,1,1,1};
float translucentBlack[] = {0,0,0,0.5};
float gray[] = {0.43,0.43,0.43,1};

touch_t threadTouch, loopTouch; //loopTouch is a copy to allow each loop iteration to operate on same data.  touch is constantly updated in a seperate thread (consider renaming this to threadTouch)
int quitState = 0;

VGImage backgroundImage;

vector<TouchableObject> TObjects;

uint64_t loopTime;

float sendcolor[] = {1.0, 0.4, 0.4, 1.0};
float receivecolor[] = {0.4, 0.69, 1.0, 1.0};

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
			- This is a comment
	****************************************************************/
	setupGraphics(&width,&height);	
	
	/****************************************************************
		Serial Communication
	****************************************************************/
	//Serial ELMSerial( "/dev/cp210x" , B9600 ); // The persisnet name has been assigned to the UART to serial bridge device i plug into my pi: cp210x
	Serial ELMSerial( "/dev/ttyUSB1" , B38400 );
	/****************************************************************
		touch Thread Creation: THis is for reading touch data from touchscreen actions
	****************************************************************/
	if (touchinit(width, height) != 0) {
		fprintf(stderr, "Unable to initialize the touch\n");
		exit(1);
	}


	Menu Mode1Menu(width/2, height-40, width-20, 60, "Mode1Menu");
	Mode1Menu.touchEnable();
	
	Menu PIDMenu(width-width/4, height/2, width/2-20, 360, "PIDMenu1");
	PIDMenu.touchEnable();


	ELMSerial.serialWrite("ATZ");
	ELMSerial.setEndChar('>');
	ELMSerial.setReadTimeout(5000);

	
	PID PID1("0105");
	PID1.update( "010541 05 26" , loopTime );
	cout<<PID1.getRawDatum() <<endl;
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
	



	
	while(1) {
		loopTime = bcm2835_st_read();
		
		// Grab touch data at the begining of each loop and 
		loopTouch = threadTouch;
		// Draw background image
		vgSetPixels(0, 0, backgroundImage, 0, 0, 800, 480);

		// Update menus
		Mode1Menu.update(loopTouch);


		if(Mode1Menu.isButtonPressed("m1")) Mode1Menu.selectButton("m1");
		if(Mode1Menu.isButtonPressed("m2")) Mode1Menu.selectButton("m2");
		if(Mode1Menu.isButtonPressed("m3")) Mode1Menu.selectButton("m3");
		if(Mode1Menu.isButtonPressed("m4")) Mode1Menu.selectButton("m4");
		if(Mode1Menu.isButtonPressed("m5")) Mode1Menu.selectButton("m5");
		if(Mode1Menu.isButtonPressed("m6")) Mode1Menu.selectButton("m6");


		
		if(Mode1Menu.isButtonSelected("m6")) {
			TextView SerialViewer(width/4, height/2, width/2-20, 360, "SerialViewer");
			Menu SerialViewerMenu(width/4, 30, width/2-20, 50, "SerialViewerMenu");
			SerialViewerMenu.touchEnable();
			
		
			while(1) {
				loopTime = bcm2835_st_read();
				loopTouch = threadTouch;
				
				
				vgSetPixels(0, 0, backgroundImage, 0, 0, 800, 480);
				//vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY);
				
				
				Mode1Menu.update(loopTouch);
				SerialViewerMenu.update(loopTouch);
				PIDMenu.update(loopTouch);
				
			
				string serialViewerCommand = SerialViewerMenu.getPressedButtonName();
				if(!serialViewerCommand.empty()) {
					SerialViewerMenu.selectButton(serialViewerCommand);
					if(serialViewerCommand.compare("Reset") == 0) {
						SerialViewer.addNewLine("ATZ", sendcolor);
						ELMSerial.serialWrite("ATZ");
					}
					if(serialViewerCommand.compare("Auto") == 0) {
						SerialViewer.addNewLine("ATSP0", sendcolor);
						ELMSerial.serialWrite("ATSP0");
					}
					if(serialViewerCommand.compare("Disp") == 0) {
						SerialViewer.addNewLine("ATDP", sendcolor);
						ELMSerial.serialWrite("ATDP");
					}
					if(serialViewerCommand.compare("Clear") == 0) {
						SerialViewer.clear();
					}
				}


				
				string commandString = PIDMenu.getPressedButtonName();
				if(!commandString.empty()) {
					PIDMenu.selectButton(commandString);
					SerialViewer.addNewLine(commandString, sendcolor);
					ELMSerial.serialWrite(commandString);
				}


				

				
				SerialViewer.update();
				string data = ELMSerial.serialReadUntil();

		
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
					break;
				}
				if(Mode1Menu.isButtonPressed("m2")) {
					Mode1Menu.selectButton("m2");
					break;
				}
				if(Mode1Menu.isButtonPressed("m3")) {
					Mode1Menu.selectButton("m3");
					break;
				}
				if(Mode1Menu.isButtonPressed("m4")) {
					Mode1Menu.selectButton("m4");
					break;
				}
				if(Mode1Menu.isButtonPressed("m5")) {
					Mode1Menu.selectButton("m5");
					break;
				}
				End();
			}

		}
		End(); //called after every itereation of the loop
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





// /****************************************************************
// Create DATA STREAM Object
// ****************************************************************/
// DataStream BoostDataStream(2);

// BoostDataStream.setCharTags('A' , 'B');
// BoostDataStream.setStreamScaling(100.0);
// BoostDataStream.setRangeScaling(1.0,1); //for the PSI positive data values. data comes in from arduino for boost gauge as PSI*streamscaling factor
// BoostDataStream.setRangeLimits(0,30,1);
// string eu1="PSI";
// BoostDataStream.setEngUnits(eu1,1);
// BoostDataStream.setWeightedMALag(3,1);
// float coefficients[]={0.25,0.25,0.5};
// BoostDataStream.setWeightedMACoeffs(coefficients,1);
// BoostDataStream.setSimpleMALag(3,1);

// Configure the second range

// BoostDataStream.setRangeScaling(-2.036,2); //for the PSI positive data values. data comes in from arduino for boost gauge as PSI*streamscaling factor
// BoostDataStream.setRangeLimits(-14.734,0,2); // this is PSI equivlanet of 30 inHG, we are rejecting larger magnitude values
// string eu2="inHG";
// BoostDataStream.setEngUnits(eu2,2);
// BoostDataStream.setWeightedMALag(3,2);
// BoostDataStream.setWeightedMACoeffs(coefficients,2);
// BoostDataStream.setSimpleMALag(3,2);