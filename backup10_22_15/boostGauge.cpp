#include <iostream>
using namespace std;

#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>			//Used for UART

#include "VG/openvg.h"			//
#include "VG/vgu.h"			//
#include "fontinfo.h"			// OpenVG
#include "shapes.h"			//
#include <stdlib.h>			//


#include <stdlib.h> 			// Used for atoi

#include "math.h"

#include "serial.h"		// this is our header file for serial functions

#include "boostGaugeGraphics.h" // this is our header file for the boostGaugeGraphics function

#include<bcm2835.h>
//float degToRad(float);
//int width, height;

int main()
{

	if(!bcm2835_init())
	return 1;

	int width, height;
	setupGraphics(&width,&height);
	cout<< "Hello Project"<< endl;
	int uart0_filestream = openSerial(); // opens the serial communication
	cout<< "Serial port opened" <<endl;
	int pressure0 = 0;  // This is the current value of pressure read in from the arduino through the UART on the RPi
	int pressure1 = 0; // THis is the first last pass value to pressure; for purposes of the moving average smoothing functionality
	int pressure2 = 0; // THis is the first last pass value to pressure; for purposes of the moving average smoothing functionality

	int pressureFilt = 0; // This is the filtered pressure value for display on screen


	uint64_t 	time = 0;
	uint64_t 	lastTime = 0;
	float		frameRates[10]; //This is for moving average display of framerate
	float 		sampledFrameRate = 0.;
	int 		cycleCount = 0;
	
	// For Update Rate for data from arduino
	uint64_t 	updateTime 	= 0;
	uint64_t 	lastUpdateTime 	= 0;
	float 		updateRates[10];
	float 		sampleUpdateRate = 0;
	int 		updateCount = 0;
	
	
	while(1)
	{

		unsigned char *ptr = readSerial(uart0_filestream);

		if(*ptr!=0)
		{
			char serialData[256];
			char outputData[256];
			int serialDataIndex=0;
			printf("This is the address of the buffer: %p\n",ptr);
			printf("This is the content of the buffer:");
			for( ; *ptr!=0; ++ptr)
			{
				printf("%c", *ptr);
				serialData[serialDataIndex]=*ptr;
				serialDataIndex++;
			}
			printf("\n");
			serialData[serialDataIndex]='\0';
			int serialDataLength = serialDataIndex;
			if((serialData[0]=='A') && (serialData[serialDataIndex-	3]== 'B'))
			{
				
				/***********************************************************************************************************************************
												Moving Average Display of Data Update Rate
				***********************************************************************************************************************************/
				
				lastUpdateTime = updateTime;
				updateTime = bcm2835_st_read();
				uint64_t deltaUpdateTime = (updateTime - lastUpdateTime);
				if(deltaUpdateTime==0) deltaUpdateTime=1;
				float updateRate = 1000000./deltaUpdateTime;
				updateRates[updateCount] = updateRate;
				
				if(updateCount==9)
				{
					int i=0;
					sampleUpdateRate = 0;
					for(i;i<=9;i++)
					{
						sampleUpdateRate+= 0.1*updateRates[i];
						
					}
					updateCount = 0;
				}
				updateCount++;
				
				printf("Good Message Received!\n");
				int outputDataIndex = 1;
				for(outputDataIndex; outputDataIndex<serialDataLength; outputDataIndex++)
				{
					outputData[outputDataIndex-1] = serialData[outputDataIndex];
				}
				outputData[outputDataIndex] = '\0';	// null terminate in order to not break atoi
				pressure0 = atoi(outputData);
				printf("Processed Pressure = %i\n", pressure0);
			}
			else printf("Message processing error!!\n");
		}

		pressureFilt = 0.5*pressure0 + 0.25*pressure1 + 0.25*pressure2;
		pressure2 = pressure1;
		pressure1 = pressure0;
		float PSI = pressureFilt/100.;
		lastTime = time;
		
		time = bcm2835_st_read();
		
		uint64_t timeDelta =time-lastTime;

		if(timeDelta==0) timeDelta=1;
		float frameRate = 1000000.0/timeDelta; 

		frameRates[cycleCount] = frameRate;
		
		if(cycleCount==9)
		{
			cycleCount=0;
			sampledFrameRate = 0.;
			int i=0;
			for(i;i<=9;i++)
			{
				sampledFrameRate += 0.1*frameRates[i];
			}
			
		}
		
		cycleCount++;
		boostGaugeGraphics(PSI,sampledFrameRate ,sampleUpdateRate, width,height);

	}
	return 0;
}
