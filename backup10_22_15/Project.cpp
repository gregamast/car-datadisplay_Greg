// This is the main project file

/***********************************************************************************************************************************
							INCLUDES
***********************************************************************************************************************************/

using namespace std;
#include <iostream>
#include <stdio.h>

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
#include <stdio.h>


/***********************************************************************************************************************************
							VARIABLE DECLARATIONS
***********************************************************************************************************************************/

	// Label and readout fonts
	Fontinfo avengeance;
	Fontinfo digits;
	
	uint64_t loopTime;
	
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
	//float gaugeColor[] = {0 , 1, 0, 1};
	float gaugeColor[] = {1 , 0, 127.0/255.0, 1};
	//float majorTickColor[] = {0, 1, 0, 1};
	float majorTickColor[] = {1 , 0, 127.0/255.0, 1};
	float minorTickColor[] = {1, 1, 1, 1};
	
	float labelColor[] = {102.0/255.0 , 178.0/255.0, 1, 1};
	//float majorTickColor2[] = {0, 1, 0, 1};
	float majorTickColor2[] = {1 , 0, 127.0/255.0, 1};
	float minorTickColor2[] = {1, 1, 1, 1};
	float black[] = {0, 0, 0, 1};
	
	/****************************************************************
		Initialize Screen Graphics
	****************************************************************/
	setupGraphics(&width,&height);	
	
	/****************************************************************
		Serial Communication
	****************************************************************/
	int uart0_filestream = openSerial(); // opens the serial communication
	
	
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
	DataStream BoostDataStream;
	
	BoostDataStream.setCharTags('A' , 'B');
	BoostDataStream.setScaling(100.0);
	BoostDataStream.setWeightedMALag(3);
	float coefficients[]={0.25,0.25,0.5};
	BoostDataStream.setWeightedMACoeffs(coefficients);
	BoostDataStream.setSimpleMALag(3);
	
	
	
	/****************************************************************
		Create BOOST GAUGE Object
	****************************************************************/
	Gauge BoostGauge(width/6, height/2, width/6); // used the constructor of class Gauge to instantiate object BoostGauge
	

	BoostGauge.setBorderColor(gaugeColor);
	BoostGauge.setBackgroundColor(black);
	BoostGauge.setNeedleColor(gaugeColor);
	
	
	/****************************************************************
		Set up Boost Gauge Object Range 1
	****************************************************************/
	BoostGauge.setNumRanges(1);
	BoostGauge.setDataRange(0 , 30, 1); // Always provide in stern order: min value first, then max
	BoostGauge.setDataAngleRange(0,-180, 1);
	BoostGauge.setMajorInterval(5,1);
	BoostGauge.setMinorInterval(1,1);
	BoostGauge.setMajorTickColor(majorTickColor, 1);
	BoostGauge.setMinorTickColor(minorTickColor, 1);
	
	
	BoostGauge.setLabelFont(avengeance, 1);
	BoostGauge.setLabelColor(labelColor, 1);			// Set label label color 		(rgba, range #)
	BoostGauge.setLabelRange(0, 30, 1);		// Set label display range 		(min, max, range #)
	BoostGauge.setLabelAngleRange(0, 180, 1);	// Set label angle range 		(min, max, range #)
	BoostGauge.setLabelIncrement(5, 1);			// Set label increment 			(increment, range #)
	BoostGauge.setLabelDecPlaces(0, 1);			// Set label # of dec places 	(# of decimal places, range #)
	BoostGauge.setLabelFont( avengeance,1);				// Set label font				(font)
	
	/****************************************************************
		Set up Boost Gauge Object Range 2
	****************************************************************/
	BoostGauge.setNumRanges(2);
	BoostGauge.setDataRange(0 , 30, 2); // Always provide in stern order: min value first, then max
	BoostGauge.setDataAngleRange(0,90, 2);
	BoostGauge.setMajorInterval(10,2);
	BoostGauge.setMinorInterval(2, 2);
	BoostGauge.setMajorTickColor(majorTickColor2, 2);
	BoostGauge.setMinorTickColor(minorTickColor2, 2);
	
	
	BoostGauge.setLabelFont(avengeance, 2);
	BoostGauge.setLabelColor(labelColor, 2);			// Set label label color 		(rgba, range #)
	BoostGauge.setLabelRange(10, 30, 2);		// Set label display range 		(min, max, range #)
	BoostGauge.setLabelAngleRange(-30, -90, 2);	// Set label angle range 		(min, max, range #)
	BoostGauge.setLabelIncrement(10, 2);			// Set label increment 			(increment, range #)
	BoostGauge.setLabelDecPlaces(0, 2);			// Set label # of dec places 	(# of decimal places, range #)
	BoostGauge.setLabelFont( avengeance,2);				// Set label font				(font)
	
	BoostGauge.draw();
	End();//This ends the picture
	
	
	/****************************************************************
		Create + Draw {NEXT}Object
	****************************************************************/
	
	/****************************************************************
		Create + Draw {NEXT}Object
	****************************************************************/

	
	
	/****************************************************************
		Execution Loop
	****************************************************************/
	while(1){
		// Read the current time
		loopTime = bcm2835_st_read();
		char serialData[256];
		readSerial(uart0_filestream , serialData);
		
		/****************************************************************
			Update all Data Stream Objects with New Data 
		****************************************************************/
		
		
		BoostDataStream.update( serialData, loopTime );
		float x = BoostDataStream.getWeightedMADatum();
		std::cout<< "data: " << x << '\n';
		
		BoostGauge.update(0); //Temp comment: 0 for a junk input for now, just draws a nonupdating needle
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

	