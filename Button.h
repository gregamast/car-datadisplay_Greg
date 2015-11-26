
/********************************/
/*		Button Class			*/
/********************************/

// This is start of the header guard.  ADD_H can be any unique name.  By convention, we use the name of the header file.
#ifndef BUTTON_H
#define BUTTON_H

using namespace std;
#include "TouchableObject.h"

class Button: public TouchableObject{

private:

	// Size & location related properties
	int readoutWidth, readoutHeight;		// Size of entire readout, including border stroke
	int rectWidth, rectHeight;				// Size of rectangle to be drawn
	int centerX, centerY;					// Location of readout center
	int bottomLeftX, bottomLeftY;			// Location of rectangle bottom left corner
	float ALPHA;
	
	
	// Timing related properties
	int desiredRefreshRate; 
	uint64_t lastUpdateTime;

	// Graphics related properties
	
	float* 	backgroundColor;
	float* 	borderColor;
	int 	borderWidth;
	
	string 	text;
	string 	lastText;
	int 	textFontSize;
	float* 	textColor;
	char 	textVertAlign;
	bool 	containsText;
	
	float 	value;
	int 	valueFontSize;
	int 	valueDecPlaces;
	float* 	valueColor; 
	char 	valueVertAlign;
	string formatSpecifierString;
	bool 	containsValue;
	
	

	
	
	



public:
	Button(int, int, int, int);		// Readout constructor: center X, center Y, width, height
	void setBackgroundColor(float*);
	void setBorder(float*, int);			// Set border color, border width
	
	
	void enableValue(char);			// Two characters: Horizontal alighment (L, R, C), and vertical alignment (T, B, C)
	void setValueDecPlaces(int);					// Set number of digits after decimal
	void setValue(float);
	void setValueRefreshRate(int);				// Set desired refresh frequency (Hz) 
	void setValueColor(float*);
	
	void enableText(char);			// Two characters: Horizontal alighment (L, R, C), and vertical alignment (T, B, C)
	void setText(string);
	void setTextColor(float*);

	
	
	
	////////////////////////////////////////////////////////////////


	


	void update();
	
};

// This is the end of the header guard
#endif