
/********************************/
/*		MENU Class			*/
/********************************/

// This is start of the header guard.  ADD_H can be any unique name.  By convention, we use the name of the header file.
#ifndef MENU_H
#define MENU_H

using namespace std;
#include "TouchableObject.h"
#include "Button.h"
#include <vector>



class Menu: public TouchableObject{
	
/****************************************************************************************
							ATTRIBUTES
*****************************************************************************************/

private:
	// Size & location related properties
	int width, height;		// Size of entire readout, including border stroke
	int rectWidth, rectHeight;				// Size of rectangle to be drawn
	int centerX, centerY;					// Location of readout center
	int bottomLeftX, bottomLeftY;			// Location of rectangle bottom left corner
	
	string identifier; // This is the idnetifier for the menu to pull from the MenuConfiguration file
	
	//int radiusWidth; //Width of rounded rectangle corner radius in pixelsssh
	//int radiusHeight; //Height of rounded rectangle corner radius in pixels
	int cornerRadius;
	
	int buttonPadding; //Blank space seperating buttons of a same menu
	int numButtons;
	
	// Graphics related properties
	
	float 	backgroundColor[4];
	float 	backgroundColorAlpha; // This captures the starting/intial alpha (max magnitude alpha can achieve for this component)
	
	float 	borderColor[4];
	float 	borderColorAlpha; // This captures the starting/intial alpha (max magnitude alpha can achieve for this component)
	
	int 	borderWidth;
	
	bool 	isHorizontal;
	
	//calculated from menu attributes and passed to constructor for buttons that build the particular menu
	int buttonWidth;
	int buttonHeight;
	
	
	int pressDebounce;
	
	bool hideable;
	int hideDeltaX;
	int hideDeltaY;
	int hideFade;
	int hideDuration;

	bool hidden;
	bool configureButtons;
	
	// Button configuration properties to be used in button creatio
	float buttonBackgroundColor[4];
	float buttonSelectedBackgroundColor[4];
	float buttonBorderColor[4];
	int buttonBorderWidth;
	float buttonSelectedBorderColor[4];
	int buttonSelectedBorderWidth;
	float buttonTextColor[4];
	float buttonSelectedTextColor[4];
	char buttonTextVertAlign;
	int buttonCornerRadius;

	string* buttonNames;
	string* buttonCfgText;
	bool* buttonSelectStates;
	string menuSelectMode;
	int timedSelectDuration;
	uint64_t timedSelectionStart;
	uint64_t timedSelectionEnd;
	
public:
	
	vector<Button> menuButtons;	
	
	
/****************************************************************************************
							METHODS
*****************************************************************************************/
private:

	void configure(string);
	int getVectorIndex(string);

public:
	
	Menu(int, int, int, int , string);		// Readout constructor: center X, center Y, width, height, identifier
	
	void update(touch_t);
	bool isButtonPressed(string);
	bool isButtonSelected(string);
	void selectButton(string);
	void deselectButton(string);
	string getPressedButtonName(void);
	
	void hide(void);
	void unhide(void);
	bool isHidden(void);
	
	
};

// This is the end of the header guard
#endif