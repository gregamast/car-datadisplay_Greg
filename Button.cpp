/********************************/
/*		Button Class			*/
/********************************/
#include <iostream> 
#include "VG/openvg.h"		//
#include "VG/vgu.h"			//
#include "fontinfo.h"		// OpenVG
#include "shapes.h"			//
#include <cmath>			// Math (float remainder operation)
#include "Button.h"
#include <stdio.h>
#include <bcm2835.h>

#include "parsingUtils.h" // utilize parsing utilities for configuration file processing
 
using namespace std;		// ??

// *** Label Fonts 
#include "avengeance.inc"
#include "digits.inc"

// Label and readout fonts
Fontinfo avengeance;
Fontinfo digits; 


/* Button Constructor */
Button::Button(int cx, int cy, int w, int h , string configType){
	lastUpdateTime = 0;
	centerX = cx;
	centerY = cy;
	readoutWidth = w;
	readoutHeight = h;
	lastText = "";
	
	containsValue = false;
	containsText = false;


	valueColor = new float[4]{0,1,0,1};
	valueColorAlpha = valueColor[3];
	
	textColor = new float[4]{0,1,0,1};
	textColorAlpha = textColor[3];
	
	backgroundColor = new float[4]{0,0,0,1};
	backgroundColorAlpha = backgroundColor[3];
	
	borderColor = new float[4]{0,1,0,1};
	borderColorAlpha = borderColor[3];

	setRectangular();
	

	setRectWidthHeight(readoutWidth, readoutHeight);
	setRectCenter(centerX, centerY);
	
	configure(configType);


	
}

void Button::configure(string configType) {
	
	
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
	
	
	
	setlocale(LC_ALL, "");
	Configuration * cfg = Configuration::create();
	try {
		cfg->parse("ButtonConf");
		string buttonName = configType;
		// string scope = configType;
		
		identifier = parseString(cfg, buttonName, "identifier");
		radiusWidth = parseInt(cfg, buttonName, "radiusWidth");
		radiusHeight = parseInt(cfg, buttonName, "radiusHeight");
		
		borderWidth = parseInt(cfg, buttonName, "borderWidth");
		rectHeight = readoutHeight-borderWidth;
		rectWidth = readoutWidth - borderWidth;
		bottomLeftX = centerX - (rectWidth+borderWidth/2) / 2;
		bottomLeftY = centerY - (rectHeight+borderWidth/2) / 2;
		parseColor(cfg, buttonName, borderColor, "borderColor");
		borderColorAlpha = borderColor[3];
		parseColor(cfg, buttonName, backgroundColor, "backgroundColor");
		backgroundColorAlpha = backgroundColor[3];
		containsText = parseBool(cfg, buttonName, "enableText");
		if(containsText) {
			string textAlign = parseString(cfg, buttonName, "textAlign");
			textVertAlign = textAlign.at(0);
			parseColor(cfg, buttonName, textColor, "textColor");
			text = parseString(cfg, buttonName, "defaultText");			
		}
		containsValue = parseBool(cfg, buttonName, "enableValue");
		if(containsValue) {
			string valueAlign = parseString(cfg, buttonName, "valueAlign");
			valueVertAlign = valueAlign.at(0);
			parseColor(cfg, buttonName, valueColor, "valueColor");
			desiredRefreshRate = parseInt(cfg, buttonName, "valueRefreshRate");
			setValueDecPlaces(parseInt(cfg, buttonName, "valueDecPlaces"));
		}
		
		
		//need to implement this debounce!
		//setPressDebounce(parseInt(cfg, buttonName, "pressDebounce"));
	}catch(const ConfigurationException & ex) {
		cout << ex.c_str() << endl;
	}
	cfg->destroy();
}


string Button::getIdentifier(void){
	return identifier;
	
}

void Button::setValueDecPlaces(int dec){						// Set number of digits before & after decimal
	
	valueDecPlaces = dec;
	char decPlacesText[10];
	sprintf(decPlacesText, "%d", valueDecPlaces);
	formatSpecifierString = "%.";
	formatSpecifierString.append(decPlacesText);
	formatSpecifierString.append("f");
	
}


void Button::update(void)
{
	
	
	
	updateVisuals();
	
	// Handle movement

	if(centerX != getDesiredPosX() || centerY != getDesiredPosY()){
		// Redraw the background buffer before moving: THis is taking the pixes determined in getPixels and setting them on the screen
		

		// Reset
		centerX = getDesiredPosX();
		centerY = getDesiredPosY();

		bottomLeftX = centerX - (rectWidth+borderWidth) / 2;
		bottomLeftY = centerY - (rectHeight+borderWidth) / 2;
		
	
		lastText = "";

	}
	
	// Handle fade
	
	if(getDesiredFadePercentage()!=0){//fadePerc 0 means we are using the full alpha orignally set, 100% means it is fully transparent (not visible)

			float alphaScalar = (100.0-getDesiredFadePercentage())/100.0; // NEEDS TO BE ".0" !!!!!
			
			borderColor[3] = borderColorAlpha*alphaScalar;
			backgroundColor[3]=backgroundColorAlpha*alphaScalar;
			
			textColor[3] = textColorAlpha*alphaScalar;
			valueColor[3] = valueColorAlpha*alphaScalar;
		
	}
	
	setfill(backgroundColor);
	StrokeWidth(borderWidth);
	setstroke(borderColor);
	//void Roundrect(VGfloat x, VGfloat y, VGfloat w, VGfloat h, VGfloat rw, VGfloat rh)
	Roundrect(bottomLeftX, bottomLeftY, rectWidth, rectHeight , radiusWidth , radiusHeight);
	


	if(containsText){
		if(text.compare(lastText)!=0){
			
			lastText.assign(text);
			setfill(textColor);
			StrokeWidth(0);
			textFontSize = (rectHeight-borderWidth)/2;
			int textWidth = TextWidth((char*)text.c_str(), SansTypeface, textFontSize);

			//loop to determine largest text width we can achieve
			while(textWidth > 0.9*rectWidth){
				textFontSize--;
				textWidth = TextWidth((char*)text.c_str(), SansTypeface, textFontSize);
			}

		}
		StrokeWidth(0);
		
					if(textVertAlign == 'T'){
				
				setfill(textColor);
				TextMid(centerX, bottomLeftY+rectHeight-textFontSize, (char*)text.c_str(), SansTypeface, textFontSize-2);
			}
			if(textVertAlign == 'C'){
				
				setfill(textColor);
				TextMid(centerX, centerY-textFontSize/2, (char*)text.c_str(), SansTypeface, textFontSize-2);
			}
			if(textVertAlign == 'B'){	
				
				setfill(textColor);
				TextMid(centerX, bottomLeftY+borderWidth, (char*)text.c_str(), SansTypeface, textFontSize-2);
			}
	}



	if(containsValue )
	{
		
		setfill(valueColor);
		// need to imploement logic to accept no value color, then set it black with alpha 0
		StrokeWidth(0);
		valueFontSize = (rectHeight-borderWidth)/2;
		char valueText[10];
		sprintf(valueText, (char*)formatSpecifierString.c_str(), value);
		string  valueString(valueText);
		int valueWidth = TextWidth((char*)valueString.c_str(), SansTypeface, valueFontSize);

		while(valueWidth > 0.9*rectWidth)
		{
			valueFontSize--;
			valueWidth = TextWidth((char*)valueString.c_str(), SansTypeface, valueFontSize);
		}
		if(valueVertAlign == 'T')
		{
		
			setfill(valueColor);
			TextMid(centerX, bottomLeftY+rectHeight-valueFontSize, (char*)valueString.c_str(), SansTypeface, valueFontSize-2);
		}
		if(valueVertAlign == 'C')
		{
			setfill(valueColor);
			TextMid(centerX, centerY-valueFontSize/2, (char*)valueString.c_str(), SansTypeface, valueFontSize-2);
		}
		if(valueVertAlign == 'B')
		{	
			
			setfill(valueColor);
			TextMid(centerX, bottomLeftY+borderWidth, (char*)valueString.c_str(), SansTypeface, valueFontSize-2);
		}




	}
}

void Button::setValue(float val){
	
	uint64_t currentTime = bcm2835_st_read();
	if(desiredRefreshRate == 0)	desiredRefreshRate = 5;
	uint64_t nextTime = lastUpdateTime + (1000000/desiredRefreshRate);
	
	if(currentTime>=nextTime){
		value = val;
	
	lastUpdateTime = currentTime;	
	}
	
}




/* Button setters */
void Button::setBackgroundColor(float color[4])
{
	backgroundColor[0] = color[0];
	backgroundColor[1] = color[1];
	backgroundColor[2] = color[2];
	backgroundColor[3] = color[3];
	backgroundColorAlpha = color[3];
}

void Button::setValueColor(float color[4])
{
	valueColor[0] = color[0];
	valueColor[1] = color[1];
	valueColor[2] = color[2];
	valueColor[3] = color[3];
	valueColorAlpha = color[3];
}

void Button::setTextColor(float color[4])
{
	textColor[0] = color[0];
	textColor[1] = color[1];
	textColor[2] = color[2];
	textColor[3] = color[3];
	textColorAlpha = color[3];
	
}

void Button::setBorder(float color[4], int width)		// Set border color, border width
{
	borderWidth = width;
	borderColor[0] = color[0];
	borderColor[1] = color[1];
	borderColor[2] = color[2];
	borderColor[3] = color[3];
	borderColorAlpha = color[3]; 
	
	rectHeight = readoutHeight-borderWidth;
	rectWidth = readoutWidth - borderWidth;

	bottomLeftX = centerX - (rectWidth+borderWidth) / 2;
	bottomLeftY = centerY - (rectHeight+borderWidth) / 2;
}

void Button::setValueRefreshRate(int rate)		// Set desired refresh frequency (Hz)
{
	desiredRefreshRate = rate;
}
void Button::enableValue( char VAlign)					// 'L', 'R', 'C' for left, right, center value alignment
{
	valueVertAlign = VAlign;
	containsValue = true;
}
void Button::enableText( char VAlign)					// 'L', 'R', 'C' for left, right, center label alignment
{
	textVertAlign = VAlign;
	containsText = true;
}
void Button::setText(string lbl)						// Set Button label
{
	text.assign(lbl); // assign is a string class function
}





