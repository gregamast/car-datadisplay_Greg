
using namespace std;
#include <stdio.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"
#include <stdlib.h>
#include <math.h>
#include <iostream> 

#include <cmath>
#include "TouchableObject.h"
   
  #include "Gauge.h"       
   

/****************************************************************
	CONSTRUCTOR
****************************************************************/
Gauge::Gauge(int x, int y, int r, int ranges){
	centerX = x;
	centerY = y;
	radius = r;
	numRanges = ranges;
	
	borderDesired=false;
	

	/****************************************************************
		Caling relevant setters from inherited Class Touchable Object
	****************************************************************/
	setCircular();				// Called by derived class to set touch area as circular
	setCircleCenter(x, y);		// Called by derived class to set circular touch area center
	setCircleRadius(r);			// Called by derived class to set circular touch area radius
	
	
	/****************************************************************
		Instatiating DISPLAY RANGE Member Properties
	****************************************************************/
	EngUnits 			= new string[numRanges];
	
	startVal 			= new float[numRanges];
	stopVal				= new float[numRanges];
	startAng			= new float[numRanges];
	stopAng				= new float[numRanges];
	majorInt			= new float[numRanges];
	minorInt			= new float[numRanges];
	majorTickColorRed	= new float[numRanges];
	majorTickColorGreen	= new float[numRanges];
	majorTickColorBlue	= new float[numRanges];
	majorTickColorAlpha	= new float[numRanges];
	minorTickColorRed	= new float[numRanges];
	minorTickColorGreen	= new float[numRanges];
	minorTickColorBlue	= new float[numRanges];
	minorTickColorAlpha	= new float[numRanges];
	
	/****************************************************************
		Instatiating DISPLAY LABEL Member Properties
	****************************************************************/
	
	labelStartVal		= new float[numRanges];
	labelStopVal		= new float[numRanges];
	labelIncrement		= new float[numRanges];
	labelDecPlaces		= new int[numRanges];
	labelStartAng		= new float[numRanges];
	labelStopAng		= new float[numRanges];
	labelColorRed		= new float[numRanges];
	labelColorGreen		= new float[numRanges];
	labelColorBlue		= new float[numRanges];
	labelColorAlpha		= new float[numRanges];
	labelFont			= new Fontinfo[numRanges];

	
}



/****************************************************************
	Define SET METHODS
****************************************************************/
void Gauge::setEngUnits(string EU, int range){
	EngUnits[range-1]=EU;
	//cout<<"this is the unit "<<EngUnits[range-1]<<endl;
}

void Gauge::setBorderColor(float color[4]){
	borderColor[0] = color[0];
	borderColor[1] = color[1];
	borderColor[2] = color[2];
	borderColor[3] = color[3];
}

void Gauge::setBackgroundColor(float color[4]){
	backgroundColor[0] = color[0];
	backgroundColor[1] = color[1];
	backgroundColor[2] = color[2];
	backgroundColor[3] = color[3];	
}

void Gauge::setNeedleColor(float color[4]){
	needleColor[0] = color[0];
	needleColor[1] = color[1];
	needleColor[2] = color[2];
	needleColor[3] = color[3];	
}

void Gauge::setMajorTickColor(float color[4], int range){
	majorTickColorRed[range-1]		= color[0];
	majorTickColorGreen[range-1]	= color[1];
	majorTickColorBlue[range-1]		= color[2];
	majorTickColorAlpha[range-1]	= color[3];	
}

void Gauge::setMinorTickColor(float color[4], int range){
	minorTickColorRed[range-1]		= color[0];
	minorTickColorGreen[range-1]	= color[1];
	minorTickColorBlue[range-1]		= color[2];
	minorTickColorAlpha[range-1]	= color[3];
}

void Gauge::setDataRange(float start, float stop, int range){
	startVal[range-1]	= start;
	stopVal[range-1]	= stop;
}

void Gauge::setDataAngleRange(float start, float stop, int range){
	startAng[range-1]	= start;
	stopAng[range-1]	= stop;
}

void Gauge::setMajorInterval(float interval, int range){
	majorInt[range-1] = interval;
}

void Gauge::setMinorInterval(float interval, int range){
	minorInt[range-1] = interval;
}

void Gauge::setLabelRange(float start, float stop, int range){
	labelStartVal[range-1]	= start;
	labelStopVal[range-1]	= stop;
}

void Gauge::setLabelAngleRange(float start, float stop, int range){
	labelStartAng[range-1]	= start;
	labelStopAng[range-1]	= stop;
}

void Gauge::setLabelIncrement(float increment, int range){
	labelIncrement[range-1] = increment;
}

void Gauge::setLabelDecPlaces(int places, int range){
	labelDecPlaces[range-1] = places;
}

void Gauge::setLabelColor(float color[4], int range){
	labelColorRed[range-1]			= color[0];
	labelColorGreen[range-1]		= color[1];
	labelColorBlue[range-1]			= color[2];
	labelColorAlpha[range-1]		= color[3];
}

void Gauge::setLabelFont(Fontinfo font, int range){
	labelFont[range-1] = font;
	
}

float Gauge::degToRad(float degrees){
	float radians = (3.14159*degrees)/180.;
	return radians;
}

void Gauge::draw(void){
	
	/****************************************************************
		Draw the gauge background
	****************************************************************/
	
	float borderWidth = 0.03*radius;
	gaugeRadius = radius - borderWidth/2;
	dynamicContentRadius = 0.85*gaugeRadius; // this can be removed now
	
	
	StrokeWidth(0);
	setfill(backgroundColor);
	Circle(centerX,centerY,gaugeRadius*2);	
	
	
	
	/****************************************************************
		Draw the Major and Minor Ticks: Range1 && Range 2 Optional
	****************************************************************/
	int range = 0;
	
	for(;range<numRanges;range++)
	{
		float valRange 		= stopVal[range] - startVal[range];
		float angRange 		= stopAng[range] - startAng[range];
		float angPerVal 	= angRange/std::abs(valRange);
		float angMajorInt 	= majorInt[range] * angPerVal;
		float angMinorInt 	= minorInt[range] * angPerVal;
		float angRatio 		= angMajorInt / angMinorInt;
		
		float majorTickColor[] =	{majorTickColorRed[range],
			majorTickColorGreen[range],
			majorTickColorBlue[range],
			majorTickColorAlpha[range]};
		float minorTickColor[] =	{minorTickColorRed[range],
			minorTickColorGreen[range],
			minorTickColorBlue[range],
			minorTickColorAlpha[range]};
		
		
		
		drawTickSet(startAng[range], stopAng[range], angMajorInt, angRatio, majorTickColor, true);		// Draw major ticks
		drawTickSet(startAng[range], stopAng[range], angMinorInt, angRatio, minorTickColor, false);		// Draw minor ticks
	}
	
	
	range = 0;
		
		for(;range<numRanges;range++)
		{

			float labelColor[] =		{labelColorRed[range],
				labelColorGreen[range],
				labelColorBlue[range],
				labelColorAlpha[range]};
			
			drawLabelSet(labelStartVal[range], labelStopVal[range], labelIncrement[range], labelDecPlaces[range], labelStartAng[range], labelStopAng[range], labelColor, labelFont[range] ); // draw label set
		}
		
	if(borderDesired){
	StrokeWidth(borderWidth);
	Fill(0,0,0,0);
	setstroke(borderColor);
	Circle(centerX,centerY,gaugeRadius*2);	// Draw gauge border (on top of ticks)
	}
	
	gaugeBuffer = vgCreateImage(VG_sABGR_8888, 2*radius, 2*radius, VG_IMAGE_QUALITY_BETTER);
	
	vgGetPixels(gaugeBuffer, 0 , 0 , centerX-radius, centerY-radius, 2*radius , 2*radius);
	
	
}

void Gauge::update(float needleValue, string units){
	

	
	
	/****************************************************************
		Convert input data to angle needed for DrawNeedle method
	****************************************************************/
	bool unitsFound = false;
	int range = 0;
	int dataRange = 0;
	
	while(!unitsFound){
		if(range==numRanges){
			cout<<"units not found inside Gauge Update method."<<endl;
			break;
		}
		if(EngUnits[range].compare(units)==0){
			unitsFound = true;
			dataRange = range;
			
		}
		range++;
	}
	
	
	if(unitsFound){
		
	/****************************************************************
		Redrawing all static components of gauge from buffer
	****************************************************************/
		vgSetPixels(centerX-radius, centerY-radius, gaugeBuffer, 0 , 0 , 2*radius , 2*radius);
		

	/****************************************************************
		Draw the Needle
	****************************************************************/
		
		float ang;
		ang = needleValue*(stopAng[dataRange]-startAng[dataRange])/abs(stopVal[dataRange]-startVal[dataRange]);
		
		drawNeedle(-ang); 
		
		
	}
	

}

void Gauge::drawLabelSet(float startLabel, float stopLabel, float labelIncrement, int decPlaces, float startAngle, float stopAngle, float* labelColor, Fontinfo font){

	float labelAngle = startAngle;
	if(labelIncrement == 0) labelIncrement = 1;
	int numLabels = (stopLabel - startLabel) / labelIncrement;
	int fontSize = 0.15*gaugeRadius;
	float labelRadius = 0.74*gaugeRadius;
	float angIncrement = (stopAngle-startAngle) / numLabels;
	setfill(labelColor);
	char labelText[10];
	char decPlacesText[10];
	
	sprintf(decPlacesText, "%d", decPlaces);
	// use append to concatenate different string values together to create format specifier
	string formatSpecifier = "%.";
	formatSpecifier.append(decPlacesText);
	formatSpecifier.append("f");

	
	char* formatSpecifierChar = (char*)formatSpecifier.c_str();
	
	float labelValue = startLabel;
	int labelCount = 0;
	for(;labelCount<=numLabels;labelCount++)
	{
		sprintf(labelText, formatSpecifierChar, labelValue);
		labelAngle = startAngle + labelCount*angIncrement;
		int labelX = centerX - labelRadius*cos(degToRad(labelAngle))-.195*fontSize;
		int labelY = centerY + labelRadius*sin(degToRad(labelAngle)) - fontSize/2;
		TextMid(labelX, labelY, labelText, font, fontSize);
		labelValue+=labelIncrement;
	}
}

void Gauge::drawTickSet(float startAng, float stopAng, float angInt,float angRatio, float* tickColor, bool isMajor){
	
	float tickWidth, tickLength, glowWidth;
	
	if(isMajor){
		tickLength = gaugeRadius - dynamicContentRadius; 
		tickWidth = 0.2*tickLength;
		glowWidth = 0.3*tickLength;
		
		
	}
	else{
		tickLength = (gaugeRadius - dynamicContentRadius)*0.65; 
		tickWidth = 0.2*tickLength;
		glowWidth = 0.3*tickLength;
	}

	float angTot = startAng;
	
	float tickGlowStops[] = {
		0.00,	tickColor[0] 		, tickColor[1] 		, tickColor[2], 		tickColor[3]*float(0.8),
		1.00,	backgroundColor[0] 	, backgroundColor[1] 	, backgroundColor[2], 	backgroundColor[3]*float(0.0)
	};
	
	
	Translate(centerX,centerY);
	Rotate(startAng);
	while(std::abs(angTot-startAng)<=std::abs(stopAng - startAng)){
		if((remainder( angTot, angInt*angRatio)==0) && !isMajor){
			Rotate(angInt);
			angTot+=angInt;	
			
		}
		else{
			// Draw the Upper Glow Rectangle
			FillLinearGradient( -gaugeRadius, 0.5*tickWidth, -gaugeRadius, 0.5*tickWidth+glowWidth, tickGlowStops, 2 );
			StrokeWidth(0);//we dont want our glow rect to have any stroke
			Rect(-gaugeRadius, 0.5*tickWidth, tickLength, 0.95*glowWidth );
			
			// Draw the Lower Glow Rectangle
			FillLinearGradient( -gaugeRadius, -0.5*tickWidth, -gaugeRadius, -0.5*tickWidth-glowWidth, tickGlowStops, 2 );
			StrokeWidth(0);//we dont want our glow rect to have any stroke
			Rect(-gaugeRadius, -0.5*tickWidth-0.95*glowWidth, tickLength, glowWidth );
			
			// Draw the Major Ticks
			StrokeWidth(1.1*tickWidth); // x1.1 factor is to wash in the glow better
			setstroke(tickColor);
			Line(-gaugeRadius,0,-gaugeRadius+tickLength,0);
			Rotate(angInt);
			angTot+=angInt;	
			
		}
		
	}
	Rotate(-(angTot));
	Translate(-centerX,-centerY);
	
	
}

void Gauge::drawNeedle(float angle){
	// Center Glow
	StrokeWidth(0);
	int glowRadius = gaugeRadius*0.1875;
	VGfloat glowStops[] = {	0.000,	needleColor[0],needleColor[1],needleColor[2],needleColor[3],
		1.000,	0,0,0,1};
	
	
	FillRadialGradient(centerX, centerY, centerX, centerY, glowRadius+5, glowStops, 2);
	Circle(centerX, centerY, glowRadius*2);

	//Gauge Center
	float centerRadius = gaugeRadius*0.125;
	float scaling = gaugeRadius*0.041;
	StrokeWidth(0);
	Fill(0,0,0,1);
	Circle(centerX, centerY, centerRadius);
	int focalX = centerX - 15;
	int focalY = centerY + 0;
	VGfloat stops[] = {	0.000,	5,5,5,.05,
		1.000,	0,0,0,1};
	FillRadialGradient(centerX, centerY, focalX, focalY, centerRadius*scaling, stops,0);
	Circle(centerX, centerY, centerRadius*2);
	Fill(0,0,0,.7);
	Circle(centerX, centerY, centerRadius);
	Fill(0,0,0,1);
	Circle(centerX, centerY, centerRadius*2*.75);

	// Gauge Needle
	centerRadius = gaugeRadius*0.05;


	float needleLength = 0.97*dynamicContentRadius;

	float xVertices[5];
	float yVertices[5];
	xVertices[0] = centerX - centerRadius*cos(degToRad(angle+90));
	yVertices[0] = centerY + centerRadius*sin(degToRad(angle+90));
	xVertices[1] = centerX - 0.92*needleLength*cos(degToRad(angle)) - (0.5*centerRadius)*cos(degToRad(angle+90));
	yVertices[1] = centerY + 0.92*needleLength*sin(degToRad(angle)) + (0.5*centerRadius)*sin(degToRad(angle+90));
	xVertices[2] = centerX - needleLength*cos(degToRad(angle));
	yVertices[2] = centerY + needleLength*sin(degToRad(angle));
	xVertices[3] = centerX - 0.92*needleLength*cos(degToRad(angle)) - (0.5*centerRadius)*cos(degToRad(angle-90));
	yVertices[3] = centerY + 0.92*needleLength*sin(degToRad(angle)) + (0.5*centerRadius)*sin(degToRad(angle-90));
	xVertices[4] = centerX - centerRadius*cos(degToRad(angle-90));
	yVertices[4] = centerY + centerRadius*sin(degToRad(angle-90));
	VGfloat needleStops[] = {	0.000,	255,255,255,0.1,
		0.250, 	needleColor[0],needleColor[1],needleColor[2],.8,
		1.000, 	needleColor[0],needleColor[1],needleColor[2],.8};
	FillRadialGradient(centerX, centerY, centerX, centerY, needleLength, needleStops, 3);
	Polygon(xVertices, yVertices, 5);

	// Gauge Needle Cap
	Stroke(0,0,0,1);
	StrokeWidth(2*centerRadius);
	float needleCapLength = centerRadius*3.33;
	float  needleCapX = centerX - needleCapLength*cos(degToRad(angle+180));
	float  needleCapY = centerY + needleCapLength*sin(degToRad(angle+180));

	Line(centerX, centerY, needleCapX, needleCapY);

}








