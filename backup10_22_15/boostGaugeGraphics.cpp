#include <iostream>

#include <stdio.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"
#include <stdlib.h>

#include <math.h>
#include "boostGaugeGraphics.h" // this is so the function at this level can scope the other function at this level.  bad cpp coding? you decide!

void setupGraphics(int* widthPtr, int* heightPtr)
{
	init(widthPtr, heightPtr); //INIT Graphics

	Start(*widthPtr, *heightPtr);	//this should go in setup			   // Start the picture
	Background(0, 0, 0);	//this should go in setup			   // Black background



	


	/***********************************************************************************************************************************
							DRAW GAUGE BORDER
	***********************************************************************************************************************************/

	/* 	VGfloat stopsNew2[]={
		0.00,	0.0,			0.0,			0.0,			1.0,
		0.88,  	0.0, 			0.0,  			0.0,  			1.0,
		0.915,	99.0/255.0,		171.0/255.0,	247.0/255.0,	1.0,
		0.92,	44.0/255.0,		77.0/255.0,		232.0/255.0,	1.0,
		0.96,	44.0/255.0,		77.0/255.0,		232.0/255.0,	1.0,
		0.965,	99.0/255.0,		171.0/255.0,	247.0/255.0,	1.0,
		1.00,	0.0,			0.0,			0.0,			1.0};

	FillRadialGradient(*widthPtr/2, *heightPtr/2, *widthPtr/2, *heightPtr/2, *heightPtr/2  ,stopsNew2,7);
	Circle(*widthPtr / 2, *heightPtr/2,*heightPtr);	//Draw the Gauge */
	
	/***********************************************************************************************************************************
							DRAW MAJOR AND MINOR TICK SETS
	***********************************************************************************************************************************/

	float centerX = *widthPtr/2;
	float centerY = *heightPtr/2;
	float angle;

	// VGfloat stopsNew3[]={
		// 0.00,	99.0/255.0,		171.0/255.0,	247.0/255.0,	1.0,
		// 1.00,	0.0,			0.0,			0.0,			0.0};
		
			VGfloat stopsNew3[]={
		0.00,	44.0/255.0,		77.0/255.0,	232.0/255.0,	1.0,
		1.00,	0.0,			0.0,			0.0,			0.0};
		
/* 			VGfloat stopsNew3[]={
		0.00,	255.0/255.0,		0.0,		0.0,		0.8,
		1.00,	0.0,			0.0,			0.0,			0.0}; */
	
		int numStops=2;
	
	angle = 0;
	Translate(*widthPtr/2,*heightPtr/2);
	


	int tickRadius = 150;
	int tickLength = 0.16*tickRadius;//30
	int tickWidth = 0.05*tickRadius;// EVEN NUMBERS ONLY!@!!!
	int glowWidth = .06*tickRadius*0.8;
	int gradGlowWidth = glowWidth+2;
	int overlapAngle = 0;
	

	
	

	for(angle;angle<=270;angle+=30)
	{
	Fill(44,77,232,1.0);
	StrokeWidth(0);
	Rect(tickRadius,-tickWidth/2-1,tickLength,tickWidth+2);
	
	

	FillLinearGradient(tickRadius,0.5*tickWidth,tickRadius,0.5*tickWidth+gradGlowWidth,stopsNew3, numStops );
	//void FillLinearGradient(VGfloat x1, VGfloat y1, VGfloat x2, VGfloat y2, VGfloat *stops, int n)
	Rect(tickRadius,0.5*tickWidth,tickLength,glowWidth);
	
	FillLinearGradient(tickRadius,-0.5*tickWidth,tickRadius,-0.5*tickWidth-gradGlowWidth,stopsNew3, numStops);
	Rect(tickRadius,-0.5*tickWidth-glowWidth,tickLength,glowWidth);
	
	FillLinearGradient(tickRadius,0,tickRadius-gradGlowWidth,0,stopsNew3, numStops );
	Rect(tickRadius-glowWidth,-0.5*tickWidth,glowWidth,tickWidth);
	
	FillLinearGradient(tickRadius+tickLength,0,tickRadius+tickLength+gradGlowWidth,0,stopsNew3, numStops );
	Rect(tickRadius+tickLength,-0.5*tickWidth,glowWidth,tickWidth);
	
	
	FillRadialGradient(tickRadius,0.5*tickWidth,tickRadius,0.5*tickWidth,gradGlowWidth,stopsNew3,numStops);
	// void Polygon(VGfloat *x, VGfloat *y, VGint n)
	VGfloat x1[] = {tickRadius,tickRadius-glowWidth,tickRadius};
	VGfloat y1[]= {0.5*tickWidth,0.5*tickWidth,0.5*tickWidth+glowWidth};
	Polygon(x1,y1,3);
	Arc(tickRadius,0.5*tickWidth, 2*glowWidth-1,2*glowWidth-1,90-overlapAngle,90+2*overlapAngle);


			
	
	FillRadialGradient(tickRadius+tickLength,0.5*tickWidth,tickRadius+tickLength,0.5*tickWidth,gradGlowWidth,stopsNew3,numStops);
	// void Polygon(VGfloat *x, VGfloat *y, VGint n)11
	VGfloat x2[] = {tickRadius+tickLength,tickRadius+tickLength+glowWidth,tickRadius+tickLength};
	VGfloat y2[]= {0.5*tickWidth,0.5*tickWidth,0.5*tickWidth+glowWidth};
	Polygon(x2,y2,3);
	Arc(tickRadius+tickLength,0.5*tickWidth, 2*glowWidth-1,2*glowWidth-1,0-overlapAngle,90+2*overlapAngle);

	//
	FillRadialGradient(tickRadius+tickLength,-0.5*tickWidth,tickRadius+tickLength,-0.5*tickWidth,gradGlowWidth,stopsNew3,numStops);
	// void Polygon(VGfloat *x, VGfloat *y, VGint n)
	VGfloat x3[] = {tickRadius+tickLength,tickRadius+tickLength+glowWidth,tickRadius+tickLength};
	VGfloat y3[]= {-0.5*tickWidth,-0.5*tickWidth,-0.5*tickWidth-glowWidth};
	Polygon(x3,y3,3);
	Arc(tickRadius+tickLength,-0.5*tickWidth, 2*glowWidth-1,2*glowWidth-1,270-overlapAngle,90+2*overlapAngle);

	//
	FillRadialGradient(tickRadius,-0.5*tickWidth,tickRadius,-0.5*tickWidth,gradGlowWidth,stopsNew3,numStops);
	// void Polygon(VGfloat *x, VGfloat *y, VGint n)
	VGfloat x4[] = {tickRadius,tickRadius-glowWidth,tickRadius};
	VGfloat y4[]= {-0.5*tickWidth,-0.5*tickWidth,-0.5*tickWidth-glowWidth};
	Polygon(x4,y4,3);
	Arc(tickRadius,-0.5*tickWidth, 2*glowWidth-1,2*glowWidth-1,180-overlapAngle,90+2*overlapAngle);
	
	
	Rotate(30 );
	}
	
	
	
}

void boostGaugeGraphics(float pressure, float time , float updateRate , int width, int height)
{



	int bigSw = 0;
	if(bigSw==1)
	{

		float angle = 10*pressure;
		float theta = angle;
		int centerX = width/2;
		int centerY = height/2;

		int sw=1;
		if(sw==1)
		{

			int needleLength = 200;
			int needleLengthFade1 = 205;
			int needleLengthFade2 = 210;
			int needleLengthFade3 = 240;
			int needleSubHeight = 180;
			int needleWidthB = 10;
			int needleWidthBFade1 = 13;
			int needleWidthBFade2 = 18;
			int needleWidthBFade3 = 40;

			//	int needleWidthT = 8;

			float xVertices[5];
			float yVertices[5];
			float xVertFade1[5];
			float yVertFade1[5];
			float xVertFade2[5];
			float yVertFade2[5];
			float xVertFade3[5];
			float yVertFade3[5];

			xVertices[0] = centerX - needleWidthB*cos(degToRad(angle+90));
			yVertices[0] = centerY + needleWidthB*sin(degToRad(angle+90));

			//
			xVertices[1] = centerX - needleSubHeight*cos(degToRad(angle))-7*cos(degToRad(angle+90));
			yVertices[1] = centerY + needleSubHeight*sin(degToRad(angle))+7*sin(degToRad(angle+90));


			xVertices[2] = centerX - needleLength*cos(degToRad(angle));
			yVertices[2] = centerY + needleLength*sin(degToRad(angle));


			xVertices[3] = centerX - needleSubHeight*cos(degToRad(angle))-7*cos(degToRad(angle-90));
			yVertices[3] = centerY + needleSubHeight*sin(degToRad(angle))+7*sin(degToRad(angle-90));


			xVertices[4] = centerX - needleWidthB*cos(degToRad(angle-90));
			yVertices[4] = centerY + needleWidthB*sin(degToRad(angle-90));


			/***************************************************************************************************
						APPLY GRADIENT and DRAW NEEDLE + CAP
		***************************************************************************************************/
			Fill(0,0,0,1);	//Fill for redrawing black circle background
			Circle(centerX, centerY, needleLength*2); //This is to redraw black cirlce every time to clear out previous needle draw


			VGfloat stopsNew[]={
				0.0,	0.0,	0.0,	1.0,	1,
				1.0,	1.0,	1.0,	1.0,	1};
			
			FillRadialGradient(width/2, height/2, width/2, height/2,  needleLength,stopsNew,2);
			StrokeWidth(0);
			Polygon( xVertices , yVertices , 5 );

			Stroke(10,10,10,1);
			StrokeWidth(needleWidthB*2);
			int needleCapLength = 40;
			float needleCapX = centerX - needleCapLength*cos(degToRad(angle+180));
			float needleCapY = centerY + needleCapLength*sin(degToRad(angle+180));
			Line(centerX, centerY, needleCapX, needleCapY);


		}

		
		/***********************************************************************************************************************************
							Display the FPS (Frames Per Secod) Rate & the Data Update Rate
	***********************************************************************************************************************************/
		Fill(255,255,255,1);
		StrokeWidth(0);
		Roundrect(10,10,width-20,60,15,15);
		char text[10];
		sprintf(text, "%.0f [FPS]",time);
		Fill(0,0,0,1);
		Text(width-105, 12, text, SerifTypeface, width/40);
		sprintf(text,"%.0f [Hz]",updateRate);
		
		Text(width-105,12+width/40,text, SerifTypeface, width/40);
		


	}//end to bigSw

	End();						   // End the picture



}



float degToRad(float degrees)
{

	float radians = degrees*3.14159/180.0;
	return radians;

}
