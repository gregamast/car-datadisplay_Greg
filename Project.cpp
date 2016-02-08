// This is the main project file

/***********************************************************************************************************************************
							INCLUDES
***********************************************************************************************************************************/


using namespace std;
#include <iostream> 
#include <stdio.h>
#include <array>

#include <vector>

#include <iterator> //for std::distance
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

#include <algorithm>



/***********************************************************************************************************************************
VARIABLE DECLARATIONS
***********************************************************************************************************************************/

// Color definitions (float r, float g, float b, float alpha)
float black[] 						= { 0 , 0 , 0 , 1 };
float green[] 						= { 0 , 1 , 0 , 1 };
float red[] 						= { 1 , 0 , 0 , 1 };
float blue[] 						= { 0 , 0 , 1 , 1 };
float white[] 						= { 1 , 1 , 1 , 1 };
float translucentBlack[] 		= { 0 , 0 , 0 , 0.5 };
float gray[] 						= { 0.43 , 0.43 , 0.43 , 1 };
float sendcolor[] 				= { 1.0 , 0.4 , 0.4 , 1.0 };
float receivecolor[] 			= { 0.4 , 0.69 , 1.0 , 1.0 };

touch_t threadTouch, loopTouch; //loopTouch is a copy to allow each loop iteration to operate on same data.  touch is constantly updated in a seperate thread (consider renaming this to threadTouch)
int quitState = 0;

VGImage backgroundImage;
int width, height;

uint64_t loopTime;

// Set up container vectors per mode: ex DASHBOARD is a mode
vector<Button> 	DASHBOARD_HotButtons;
vector<PID> 		DASHBOARD_PIDs;
vector<Gauge>	 	DASHBOARD_GaugeObjects;
vector<Menu> 		DASHBOARD_Menus;



/***********************************************************************************************************************************
PROTOTYPES
***********************************************************************************************************************************/
void setupGraphics(int*, int*);
void DisplayObjectManager( std::vector<Gauge>& , std::vector<PID>& , std::vector<Menu>& , std::vector<Button>& );


/***********************************************************************************************************************************
MAIN
***********************************************************************************************************************************/

int main(){
	
	
	if(!bcm2835_init()) return 1;
	
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
	ELMSerial.setReadTimeout(5000);
	ELMSerial.serialWrite("ATZ");
	ELMSerial.setEndChar('>');
	
	/****************************************************************
		touch Thread Creation: THis is for reading touch data from touchscreen actions
	****************************************************************/
	if (touchinit(width, height) != 0) {
		fprintf(stderr, "Unable to initialize the touch\n");
		exit(1);
	}


	/****************************************************************
		CREATE MAIN MODE MENU
	****************************************************************/
	Menu ModeMenu(width/2, height-40, width-20, 60, "ModeMenu");
	ModeMenu.touchEnable();
	
	
	

	
	
	
	
	/****************************************************************
		Draw Main Background image
	****************************************************************/

	Image(0, 0, width, height, "carbon.jpg");
	backgroundImage = vgCreateImage(VG_sABGR_8888, width, height, VG_IMAGE_QUALITY_BETTER);
	vgGetPixels(backgroundImage , 0 , 0 , 0 , 0 , width , height);
	

	/***********************************************************************************************************************************
	TOP-LEVEL INFINITE WHILE
	***********************************************************************************************************************************/
	while(1) {
		loopTime = bcm2835_st_read();
		
		// Grab touch data at the begining of each loop and 
		loopTouch = threadTouch;
		// Draw background image
		vgSetPixels(0, 0, backgroundImage, 0, 0, 800, 480);

		// Update menus
		ModeMenu.update(loopTouch);
		


		if(ModeMenu.isButtonPressed("m1")) ModeMenu.selectButton("m1");
		if(ModeMenu.isButtonPressed("m2")) ModeMenu.selectButton("m2");
		if(ModeMenu.isButtonPressed("m3")) ModeMenu.selectButton("m3");
		if(ModeMenu.isButtonPressed("m4")) ModeMenu.selectButton("m4");
		if(ModeMenu.isButtonPressed("m5")) ModeMenu.selectButton("m5");
		if(ModeMenu.isButtonPressed("m6")) ModeMenu.selectButton("m6");

		
		/**************************************************************************************************************************************************************************
		Infinite While loop for Mode 1: Display mode
		***************************************************************************************************************************************************************************/
		
		// NOTE WANT TO IMPLEMENT A SWITCH CASE HERE
		if(ModeMenu.isButtonSelected("m1")){
			
			int numPIDs = 0;
			
			// create PID iterator for requests
			std::vector<PID>::iterator p = DASHBOARD_PIDs.begin();
			bool waitingOnData;
			
			// create iterator for a vector of object type Button
			
			
			bool menuVisable = false;

			string serialData = "";

			while(1){
				

				if(DASHBOARD_PIDs.size() != numPIDs){
					numPIDs = DASHBOARD_PIDs.size();
					p = DASHBOARD_PIDs.begin();
				}
				
				loopTime = bcm2835_st_read();
				loopTouch = threadTouch;
				
				//Loop through PID vector and attempt a call and read until from SErial object to obtain data
				//If data not requested
				if( !waitingOnData && p!= DASHBOARD_PIDs.end() ){
					ELMSerial.serialWrite((p)->getCommand() );
					waitingOnData = true;
				}
				
				else if(waitingOnData && !serialData.empty() ){
					waitingOnData = false;
					(p) ->update(serialData, loopTime);
					if(p<DASHBOARD_PIDs.end() ) p++;
					if(p== DASHBOARD_PIDs.end() ) p = DASHBOARD_PIDs.begin();
				}
				
				
				
				vgSetPixels(0, 0, backgroundImage, 0, 0, 800, 480);
				ModeMenu.update(loopTouch);
				
				
				
				
				
				
				
				for( std::vector<Button>::iterator it = DASHBOARD_HotButtons.begin(); it != DASHBOARD_HotButtons.end(); it++) {
					(it)->updateTouch(loopTouch);
					(it)->update();
				}
				
				for( std::vector<Menu>::iterator it = DASHBOARD_Menus.begin(); it != DASHBOARD_Menus.end(); it++) {
					(it)->update(loopTouch);
				}
				
				for( std::vector<Gauge>::iterator it = DASHBOARD_GaugeObjects.begin(); it != DASHBOARD_GaugeObjects.end(); it++) {
					(it)->updateTouch(loopTouch);
					(it)->update(0, "PSI");
				}
				
				DisplayObjectManager( DASHBOARD_GaugeObjects , DASHBOARD_PIDs , DASHBOARD_Menus , DASHBOARD_HotButtons );
				
				
				//************************************************************************************************************************************************************
				
				//************************************************************************************************************************************************************
				
				//************************************************************************************************************************************************************
				
				//************************************************************************************************************************************************************
				
				//************************************************************************************************************************************************************
				
				/*
					if( (it)->isReleased() && !menuVisable){
						
						menuVisable=true;
						for( std::vector<Button>::iterator obj = DASHBOARD_HotButtons.begin(); obj != DASHBOARD_HotButtons.end(); obj++) (obj)->touchDisable();
					}
				}
				

				//update all object vectors appropriatelay: we want to update gauge objects before so that they show up "underneath" the menu if it is called upon
				std::vector<PID>::iterator PIter = DASHBOARD_PIDs.begin();
				for( std::vector<Gauge>::iterator GIter = DASHBOARD_GaugeObjects.begin(); GIter != DASHBOARD_GaugeObjects.end(); GIter++){
					(GIter)->updateTouch(loopTouch);
					
					// SImply select the same element that obj currently points to, but from the PID vector standpoint.  THis requires an in-parallel "update" to gauge and PID vectors such that gauge of element i always takes data from PID of element i
					(GIter)->update( (PIter)->getRawDatum() ,"PSI" );
					(GIter)->touchEnable();
					if((GIter)->isPressed() ){
						DASHBOARD_GaugeObjects.erase(GIter);
						DASHBOARD_PIDs.erase(PIter);	
					}
					PIter++;
				}
				

				
				
				
				//IF the menu is visible, we need to then update it with touch and draw it (part of update method)
				if(menuVisable){
					
					string groupNamePressed = ""; //THis tells us which of the grouyps of first menu was pressed
					
					HOTBUTTON_GroupMenu.update(loopTouch);
					groupNamePressed =HOTBUTTON_GroupMenu.getPressedButtonName();
					if(!groupNamePressed.empty()){
						HOTBUTTON_GroupMenu.selectButton(groupNamePressed);
						if(groupNamePressed.compare(groupName)!=0){ //Nonzero value means it doesnt compare, and we have selected a different gropuname and need to repopluate the vector with the second menu to ddisplay
							groupName = groupNamePressed;
							
							if(HOTBUTTON_ParameterMenus.size()>0) HOTBUTTON_ParameterMenus.erase(HOTBUTTON_ParameterMenus.end());
							
							HOTBUTTON_ParameterMenus.emplace_back(3*width/6 , 0.5*(height-30) , width/3-10 , height-100,groupName);
						}
						
						
					}
					
					// SELECT the parameter menu button: give the button the select appearance
					for(std::vector<Menu>::iterator m=HOTBUTTON_ParameterMenus.begin(); m!=HOTBUTTON_ParameterMenus.end();m++){
						
						(m)->update(loopTouch);
						string parameterPressed = "";
						parameterPressed = (m)->getPressedButtonName();
						if(!parameterPressed.empty()) (m)->selectButton(parameterPressed);
					}
					
					
					// SELECT the Display object menu button
					HOTBUTTON_DisplayObjectMenu.update(loopTouch);
					string buttonName = HOTBUTTON_DisplayObjectMenu.getPressedButtonName();
					if(!buttonName.empty()) HOTBUTTON_DisplayObjectMenu.selectButton(buttonName);
					
					
					//Find which buttons are finally selected
					string groupMenuButtonSelected = HOTBUTTON_GroupMenu.getSelectedButtonName();
					string parameterMenuButtonSelected =  HOTBUTTON_ParameterMenus[0].getSelectedButtonName();
					string displayObjectedMenuButtonSelected = HOTBUTTON_DisplayObjectMenu.getSelectedButtonName();
					
					
					// Implement Logic for If all menus have selected Button
					if(!groupMenuButtonSelected.empty() && !parameterMenuButtonSelected.empty() && !displayObjectedMenuButtonSelected.empty() ){
						
						cout<<"ALl menus selected"<<endl;
						
						
						//In parallel, populate the appropriate PID object in the PID vector
						DASHBOARD_PIDs.emplace_back(parameterMenuButtonSelected);
						p = DASHBOARD_PIDs.begin(); //THis is to reset iterator to beginning of vector: avoid bug when this is not done, causes program crash
						
						
						//Popluate appropriate object vectors with selected object
						
						cout<<parameterMenuButtonSelected<<endl;
						cout<<displayObjectedMenuButtonSelected<<endl;
						cout<<groupMenuButtonSelected<<endl;
						cout<<displayObjectedMenuButtonSelected.compare("Gauge") <<endl;
						
						// Deselect buttons (except first menu)
						cout<<parameterMenuButtonSelected<<endl;
						HOTBUTTON_ParameterMenus[0].deselectButton(parameterMenuButtonSelected);
						HOTBUTTON_DisplayObjectMenu.deselectButton(displayObjectedMenuButtonSelected);
						
						
						
						if( displayObjectedMenuButtonSelected.compare("Gauge")==0 ){
							string parmMenuButtonSelectedForAppend = parameterMenuButtonSelected;
							DASHBOARD_GaugeObjects.emplace_back(width/6, height/2, width/6 , parmMenuButtonSelectedForAppend.append("Gauge") );

							//oNLY DRAW THE GAUGE ONCE:
							DASHBOARD_GaugeObjects.back().draw();
							
						}
						
						
						
						
						
						//Re-touchenable all other buttons/hotbuttons/objects
						for( std::vector<Button>::iterator obj = DASHBOARD_HotButtons.begin(); obj != DASHBOARD_HotButtons.end(); obj++) (obj)->touchEnable();
						
						menuVisable = false; 
						
						// De-select the buttons which were selected in this round of menu display
						// HOTBUTTON_ParameterMenus[0].deselectButton(parameterMenuButtonSelected);
						// HOTBUTTON_DisplayObjectMenu.deselectButton(displayObjectedMenuButtonSelected);

						
					} // END IF ALL MENU BUTTONS ARE SELECTED

				} //END if Menu is visible
				
				
				*/
				
				//************************************************************************************************************************************************************
				
				//************************************************************************************************************************************************************
				
				//************************************************************************************************************************************************************
				
				//************************************************************************************************************************************************************
				
				//************************************************************************************************************************************************************

				
				
				// Check if any other top menu buttons have been pressed, if so, break out of this loop for the current top menu button which is pressed
				if(ModeMenu.isButtonPressed("m1")) {
					ModeMenu.selectButton("m1");
					break;
				}
				if(ModeMenu.isButtonPressed("m2")) {
					ModeMenu.selectButton("m2");
					break;
				}
				if(ModeMenu.isButtonPressed("m3")) {
					ModeMenu.selectButton("m3");
					break;
				}
				if(ModeMenu.isButtonPressed("m4")) {
					ModeMenu.selectButton("m4");
					break;
				}
				if(ModeMenu.isButtonPressed("m5")) {
					ModeMenu.selectButton("m5");
					break;
				}
				
				if(ModeMenu.isButtonPressed("m6")) {
					ModeMenu.selectButton("m6");
					break;
				}

				
				End();
			}//END MODE 61INFIINTE WHILE LOOP
			
		} // END if mode 1 pressed
		
		
		
		
		/**************************************************************************************************************************************************************************
		Infinite While loop for Mode 6: 
		***************************************************************************************************************************************************************************/
		
		if(ModeMenu.isButtonSelected("m6")) {
			
			Menu PIDMenu(width-width/4, height/2, width/2-20, 360, "PIDMenu1");
			PIDMenu.touchEnable();
			
			
			TextView SerialViewer(width/4, height/2, width/2-20, 360, "SerialViewer");
			Menu SerialViewerMenu(width/4, 30, width/2-20, 50, "SerialViewerMenu");
			SerialViewerMenu.touchEnable();
			
			
			while(1) {
				loopTime = bcm2835_st_read();
				loopTouch = threadTouch;
				
				
				vgSetPixels(0, 0, backgroundImage, 0, 0, 800, 480);
				//vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY);
				
				
				ModeMenu.update(loopTouch);
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
				if(ModeMenu.isButtonPressed("m1")) {
					ModeMenu.selectButton("m1");
					break;
				}
				if(ModeMenu.isButtonPressed("m2")) {
					ModeMenu.selectButton("m2");
					break;
				}
				if(ModeMenu.isButtonPressed("m3")) {
					ModeMenu.selectButton("m3");
					break;
				}
				if(ModeMenu.isButtonPressed("m4")) {
					ModeMenu.selectButton("m4");
					break;
				}
				if(ModeMenu.isButtonPressed("m5")) {
					ModeMenu.selectButton("m5");
					break;
				}
				End();
			} //END MODE 6 INFIINTE WHILE LOOP

		} // END if mode 6 pressed
		
		
		
		
		End(); //called after every itereation of the loop
	}
}

// setupGraphics()
void setupGraphics(int* widthPtr, int* heightPtr) {	
	init(widthPtr,heightPtr);
	Start(*widthPtr, *heightPtr);		//Set up graphics, start picture
	Background(0,0,0);
}

//

void DisplayObjectManager( std::vector<Gauge>& Gauges, std::vector<PID>& PIDs, std::vector<Menu>& Menus, std::vector<Button>& HotButtons ){



		int width = 800; 
		int height = 480;
	//If not hotbuttons are visible & you are in mode 1 (to be added later) then we want to create hot buttons
	if( HotButtons.size()==0 && Gauges.size()==0 ){
		HotButtons.emplace_back(Button(1*width/6, 0.5*(height-30), 25, 25, "hotButton1"));
		HotButtons.back().touchEnable();
		HotButtons.emplace_back(Button(3*width/6, 0.5*(height-30), 25, 25, "hotButton2"));
		HotButtons.back().touchEnable();
		HotButtons.emplace_back(Button(5*width/6, 0.5*(height-30), 25, 25, "hotButton3"));
		HotButtons.back().touchEnable();
	}

	/**************************************************************************************
	VECTOR LOOP: BUTTONS
	**************************************************************************************/

	for(std::vector<Button>::iterator BIter = HotButtons.begin(); BIter != HotButtons.end(); BIter++){
		

		
		if(BIter->isPressed()) BIter->select();
		if(BIter->isReleased()) {
			
			// Disable touch for all other objects
			for(std::vector<Button>::iterator BIter 	= HotButtons.begin(); 	BIter != HotButtons.end(); 	BIter++) BIter->touchDisable();
			for(std::vector<Menu>::iterator MIter 	= Menus.begin(); 			MIter != Menus.end(); 		MIter++) MIter->touchDisable();
			for(std::vector<Gauge>::iterator GIter 	= Gauges.begin(); 		GIter != Gauges.end(); 		GIter++) GIter->touchDisable();
			
			Menus.emplace_back(1*width/6 , 0.5*(height-30) , width/3-10 , height-120,"HOTBUTTON_GroupMenu");
			string defaultGroup  = "G1";
			Menus.back().selectButton(defaultGroup);
			Menus.emplace_back(5*width/6 , 0.5*(height-30) , width/3-10 , height-120,"HOTBUTTON_DisplayTypeMenu");
			Menus.emplace_back(3*width/6 , 0.5*(height-30) , width/3-10 , height-120,defaultGroup);
			
			
			
		}
	
	}
	
	/**************************************************************************************
	VECTOR LOOP: MENUS
	**************************************************************************************/
	
	if(Menus.size() != 0 ){
		
	string menuType = "group";
	auto HOTBUTTON_GroupMenu_it = find_if( Menus.begin() , Menus.end() , [&menuType](const Menu& obj) {return obj.type.compare(menuType) ==0; } ); 
	
	menuType = "parameter";
	auto HOTBUTTON_ParameterMenu_it = find_if( Menus.begin() , Menus.end() , [&menuType](const Menu& obj) {return obj.type.compare(menuType) ==0; } ); 
	
	menuType = "DisplayType";
	auto HOTBUTTON_DisplayMenu_it = find_if( Menus.begin() , Menus.end() , [&menuType](const Menu& obj) {return obj.type.compare(menuType) ==0; } ); 
	

	
	string pressedButtonName = HOTBUTTON_GroupMenu_it->getPressedButtonName();
	if( ! pressedButtonName.empty()){
		HOTBUTTON_GroupMenu_it->selectButton(pressedButtonName);
		
		//remove second menu and repopulate with appropriate menu
			Menus.erase(HOTBUTTON_ParameterMenu_it);
			Menus.emplace_back(3*width/6 , 0.5*(height-30) , width/3-10 , height-120,HOTBUTTON_GroupMenu_it->getSelectedButtonName() );
			//recreate iterator
			menuType = "parameter";
			HOTBUTTON_ParameterMenu_it = find_if( Menus.begin() , Menus.end() , [&menuType](const Menu& obj) {return obj.type.compare(menuType) ==0; } ); 
	}	

	
	
	pressedButtonName = HOTBUTTON_ParameterMenu_it->getPressedButtonName();
	if( ! pressedButtonName.empty()){
		HOTBUTTON_ParameterMenu_it->selectButton(pressedButtonName);
	}
	
	pressedButtonName = HOTBUTTON_DisplayMenu_it->getPressedButtonName();
	if( ! pressedButtonName.empty()){
		HOTBUTTON_DisplayMenu_it->selectButton(pressedButtonName);
		if( !HOTBUTTON_GroupMenu_it->getSelectedButtonName().empty() && !HOTBUTTON_ParameterMenu_it->getSelectedButtonName().empty()  ){
			
			
			
									cout<<"ALl menus selected"<<endl;
						
						
						//In parallel, populate the appropriate PID object in the PID vector
						PIDs.emplace_back( HOTBUTTON_ParameterMenu_it->getSelectedButtonName() );
						
						
						
						if( HOTBUTTON_DisplayMenu_it->getSelectedButtonName().compare("Gauge")==0 ){
							string parmMenuButtonSelectedForAppend = HOTBUTTON_ParameterMenu_it->getSelectedButtonName();
							Gauges.emplace_back(width/6, height/2, width/6 , parmMenuButtonSelectedForAppend.append("Gauge") );

							//oNLY DRAW THE GAUGE ONCE:
							Gauges.back().draw();
							
						}
						

			Menus.erase(HOTBUTTON_GroupMenu_it);
			Menus.erase(HOTBUTTON_ParameterMenu_it);
			Menus.erase(HOTBUTTON_DisplayMenu_it);
			
			// Since menu is closing, loop through and re-touch enable and re-diplsay all other objects
			for(std::vector<Button>::iterator BIter 	= HotButtons.begin(); 	BIter != HotButtons.end(); 	BIter++) BIter->touchEnable();
			for(std::vector<Menu>::iterator MIter 	= Menus.begin(); 			MIter != Menus.end(); 		MIter++) MIter->touchEnable();
			for(std::vector<Gauge>::iterator GIter 	= Gauges.begin(); 		GIter != Gauges.end(); 		GIter++) GIter->touchEnable();
			
		}
	}
	
	
	
	
	// if(HOTBUTTON_GroupMenu_it->getSelectedButtonName().compare
	// for(std::vector<Menu>::iterator MIter = Menus.begin(); MIter != Menus.end(); MIter++){
		
		
		//Check special case of us having selected (or reselected) a group sub-menu button: change the second sub-menu
		// if(MIter->menuIdenifier.compare("HOTBUTTON_GroupMenu")==0){
			// string pressedButtonName = MIter->getPressedButtonName();
			// if( ! pressedButtonName.empty() ) MIter->selectButton( pressedButtonName);
			
			
		// }
		
		
		// string pressedButtonName = MIter->getPressedButtonName();
		// if( ! pressedButtonName.empty() ) MIter->selectButton( pressedButtonName);
		//if(menu is type ...)
		
		
		
	// }
	
	

}
}
	
	
	/**************************************************************************************
	VECTOR LOOP: GAUGES
	**************************************************************************************/

	/**************************************************************************************
	VECTOR LOOP: GRAPHS
	**************************************************************************************/

	/**************************************************************************************
	VECTOR LOOP: TAPES
	**************************************************************************************/








