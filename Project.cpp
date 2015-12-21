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

// Fontinfo avengeance;
// Fontinfo digits; 

touch_t touch;
int quitState = 0;

VGImage backgroundImage;

vector<Gauge> GObjects;
vector<Button> BObjects;

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
		Load Fonts
	****************************************************************/
	// avengeance = loadfont(avengeance_glyphPoints, 
	// avengeance_glyphPointIndices, 
	// avengeance_glyphInstructions,                
	// avengeance_glyphInstructionIndices, 
	// avengeance_glyphInstructionCounts, 
	// avengeance_glyphAdvances,
	// avengeance_characterMap, 
	// avengeance_glyphCount);
	// digits = loadfont(digits_glyphPoints, 
	// digits_glyphPointIndices, 
	// digits_glyphInstructions,                
	// digits_glyphInstructionIndices, 
	// digits_glyphInstructionCounts, 
	// digits_glyphAdvances,
	// digits_characterMap, 
	// digits_glyphCount);
	
	
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
	
	GObjects.push_back( Gauge(width/6, height/2, width/6 , "BoostGauge") );

	//BoostGauge.touchEnable(); 	//Enable the touch functionaltiy  of this touchable object gauge object BoostGauge object
	//BoostGauge.setLabelFont(avengeance, 1);
	//BoostGauge.setLabelFont(avengeance, 2);

	/****************************************************************
		Create TEMPERATURE GAUGE Object
	****************************************************************/
	GObjects.push_back( Gauge( (width/6)*3, height/2, width/6 , "TempGauge" ) );
	//Gauge TempGauge((width/6)*3, height/2, width/6 ); // used the constructor of class Gauge to instantiate object BoostGauge
	
	//configType = "TempGauge";
	//TempGauge.configure(configType);
	//TempGauge.touchEnable(); 	//Enable the touch functionaltiy  of this touchable object gauge object TempGauge object
	//TempGauge.setLabelFont(avengeance, 1);
	//TempGauge.setLabelFont(avengeance, 2);
	
	/****************************************************************
		Create BOOST GAUGE Object
	****************************************************************/
	// Gauge(int x, int y, int r, int ranges)
	//Gauge BoostGauge2((width/6)*5, height/2, width/6 ); // used the constructor of class Gauge to instantiate object BoostGauge2
	
	GObjects.push_back( Gauge( (width/6)*5, height/2, width/6 , "BoostGauge2" ) );
	
	//configType = "BoostGauge2";
	//BoostGauge2.configure(configType);

	//BoostGauge2.touchEnable(); 	//Enable the touch functionaltiy  of this touchable object gauge object BoostGauge2 object
	//BoostGauge2.setLabelFont( avengeance,1);				// Set label font				(font)
	//BoostGauge2.setLabelFont( avengeance,2);				// Set label font				(font)
	/****************************************************************
		Create BUTTON objects
	****************************************************************/
	BObjects.push_back( Button(width/2, height/2, width, height ,"BackgroundTouchButton") );

	BObjects.push_back( Button( width-300, height/2, 100, 50 , "FramerateButton" ));
	//Button ScreenRRButton(width-300, height/2, 100, 50);
	//configType = "FramerateButton";
	//ScreenRRButton.configure(configType);
	//ScreenRRButton.touchEnable();


	BObjects.push_back( Button( width-50, -22,100,50 , "MenuButton7" ));
	
	// Button MenuButton7(width-50, -22,100,50);
	// configType = "MenuButton7";
	// MenuButton7.configure(configType);
	//MenuButton7.touchEnable();

	BObjects.push_back( Button( width-150-10, -22,100,50 , "MenuButton6" ));
	
	// Button MenuButton6(width-150-10, -22,100,50);
	// configType = "MenuButton6";
	// MenuButton6.configure(configType);
	//MenuButton6.touchEnable();
	
	BObjects.push_back( Button( width-250-20, -22,100,50 , "MenuButton5" ));

	// Button MenuButton5(width-250-20, -22,100,50);
	// configType = "MenuButton5";
	// MenuButton5.configure(configType);
	//MenuButton5.touchEnable();
	
	BObjects.push_back( Button( width-350-30, -22,100,50, "MenuButton4" ));

	// Button MenuButton4(width-350-30, -22,100,50);
	// configType = "MenuButton4";
	// MenuButton4.configure(configType);
	//MenuButton4.touchEnable();

	BObjects.push_back( Button( width-450-40, -22,100,50, "MenuButton3" ));
	
	// Button MenuButton3(width-450-40, -22,100,50);
	// configType = "MenuButton3";
	// MenuButton3.configure(configType);
	//MenuButton3.touchEnable();
	
	BObjects.push_back( Button( width-550-50, -22,100,50, "MenuButton2" ));

	// Button MenuButton2(width-550-50, -22,100,50);
	// configType = "MenuButton2";
	// MenuButton2.configure(configType);
	//MenuButton2.touchEnable();

	BObjects.push_back( Button( width-650-60, -22,100,50, "MenuButton1" ));
	
	// Button MenuButton1(width-650-60, -22,100,50);
	// configType = "MenuButton1";
	// MenuButton1.configure(configType);
	//MenuButton1.touchEnable();


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
		Draw Created Objects (currently no draw for button, need to remove the whole "draw " idea in general. It is not draw but filling buffer space. End draws the buffer on the scren)
	****************************************************************/
	// // std::cin.ignore();
	for(int i=0; i<GObjects.size() ; i++){
		GObjects[i].draw();
	}
	// BoostGauge.draw();
	// TempGauge.draw();
	// BoostGauge2.draw();
	
	// Gauge *gaugeArray[]={&BoostGauge , &TempGauge, &BoostGauge2};
	// Button *buttonArray[]={&ScreenRRButton, &MenuButton7, &MenuButton1 , &MenuButton2, &MenuButton3, &MenuButton4, &MenuButton5, &MenuButton6 };


	


	/****************************************************************
		DRAW all created objects
	****************************************************************/
	End();//This ends the picture
	
	
	
	//vector<Gauge> GObjects;
	
	
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
		
		
		// for(int i=0;i<=2;i++){
		// gaugeArray[i]->updateTouch(touch);
		// }
		// for(int i=0;i<=7;i++){
		// buttonArray[i]->updateTouch(touch);
		// }
		
		/****************************************************************
			Update current while loop TYPE vectors: type 1, 2, ... and is touched or not
		****************************************************************/
		
		
		
		
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
		
		
		/****************************************************************
			TEMPORARY COMMENT OUT, NEED TO INTEGRATE THIS
			|
			|
			v
		****************************************************************/
		//ScreenRRButton.setValue(BoostDataStream.getRawUpdateRate());
		/****************************************************************
			^
			|
			|
			TEMPORARY COMMENT OUT, NEED TO INTEGRATE THIS
		****************************************************************/
		
		
		for(int i=0; i<GObjects.size() ; i++){
			GObjects[i].update(BoostDataStream.getWeightedMADatum(), BoostDataStream.getEngUnits());
		}
		
		// for(int i=0;i<=2;i++){
		// gaugeArray[i]->update(BoostDataStream.getWeightedMADatum(), BoostDataStream.getEngUnits());
		// }
		
		for(int i=0; i<BObjects.size() ; i++){
			BObjects[i].update();
		}
		// for(int i=0;i<=7;i++){
		// buttonArray[i]->update();
		// }
		
		
		
		/****************************************************************
			MAIN MENU Logic to process touch
		****************************************************************/
		
		for(int idx=0; idx<BObjects.size();idx++){
			bool OtherBObjTouched = false;
			bool GObjTouched = false;
			
			//std::cout<<"Does Idenifier match that of bg button:"<<BObjects[idx].getIdentifier().compare("BackgroundTouchButton")==0<<std::endl;
			std::cout<<"Is that button touched:"<<BObjects[idx].isTouched()<<std::endl;
			
			if( BObjects[idx].getIdentifier().compare("BackgroundTouchButton")==0 && BObjects[idx].isTouched() ){
				//cout<<"Background Touch Button is touched............"<<endl;
				for(int idx2=0; idx2<GObjects.size();idx2++){
					if(GObjects[idx2].isTouched()){
						GObjTouched = true;
						break;
					}
					
				}
				for(int idx3=0; idx3<BObjects.size(); idx3++){
					if(BObjects[idx3].isTouched() && BObjects[idx3].getIdentifier().compare("BackgroundTouchButton")!=0 ){
						OtherBObjTouched = true;
						break;
					}
					
				}
				
				// Then find the current state of the home menu, and move it accordingly
				if( GObjTouched || OtherBObjTouched){
					if(BObjects[3].getCurrentPosY() <0){
						
						for(int idx4=0; idx4<GObjects.size(); idx4++){
							GObjects[idx4].fade(75 ,250+60*6,"emptyForNow");
						}
						for(int idx5=0; idx5<BObjects.size(); idx5++){
							if(BObjects[idx5].getIdentifier().compare("BackgroundTouchButton")!=0){
							BObjects[idx5].move(0,45,250,"emptyForNow");
							BObjects[idx5].fade( 0,250,"emptyForNow");	
							}
						}
					}
					else{
						for(int idx6=0; idx6<GObjects.size(); idx6++){
							GObjects[idx6].fade(0 ,250+60*6,"emptyForNow");
						}
						
						for(int idx7=0; idx7<BObjects.size(); idx7++){
							if(BObjects[idx7].getIdentifier().compare("BackgroundTouchButton")!=0){
							BObjects[idx7].move(0, -45,250,"emptyForNow");
							BObjects[idx7].fade( 75,250,"emptyForNow");
							}
						}
						
					}
					
				}
				
			}//END IF Identifier match
			
		}//End FOR Index Through BOBJECTS
		
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

