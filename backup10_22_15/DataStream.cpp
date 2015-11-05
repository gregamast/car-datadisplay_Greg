using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "DataStream.h"
#include <cmath>
#include <algorithm>    // std::find

#include<bcm2835.h>


// DataStream::DataStream(void );

void DataStream::setCharTags(char startTag, char stopTag){
	startCharTag = startTag;
	stopCharTag = stopTag;
}

void DataStream::setScaling(float scaling){
	streamScaling = scaling;
}

void DataStream::setWeightedMALag(int lag){
	weightedMALag = lag;
	
	// Since with lag we now know size, we should initilize the arrays to data 0
	weightedMAData = new float[lag];
	weightedMACoeffs = new float[lag];
	int i=0;
	for(;i<lag;i++){
		weightedMAData[i]=0;
		weightedMACoeffs[i]=0;
	}
}

void DataStream::setSimpleMALag(int lag){
	simpleMALag = lag;
	
	// Since with lag we now know size, we should initilize the arrays to data 0
	simpleMAData = new float[lag];
	int i=0;
	for(;i<lag;i++){
		simpleMAData[i]=0;
	}
}

void DataStream::setWeightedMACoeffs( float* coeffs){
	int i=0;
	for(;i<weightedMALag;i++){
		weightedMACoeffs[i]=coeffs[i];
	}
}

void DataStream::setReadoutFreq(int freq){
	readoutFreq = freq;
}

float DataStream::getRawDatum(){
	return (simpleMAData[simpleMALag-1]);
}


float DataStream::getWeightedMADatum(){
	
	int i = 0;
	float datum = 0;
	float coeffSum = 0;
	for(;i<weightedMALag;i++){
		datum+= (weightedMAData[i])*(weightedMACoeffs[i]);
		coeffSum+=weightedMACoeffs[i];
	}
	
	datum = datum/coeffSum;
	return datum;
}

float DataStream::getSimpleMADatum(){
	
	int i = 0;
	float datum = 0;
	for(;i<simpleMALag;i++){
		datum+= simpleMAData[i];
	}
	datum = datum/simpleMALag;
	return datum;
}

float DataStream::getRawUpdateRate(){
	return 0;
}

float DataStream::getReadoutUpdateRate(){
	return 0;
}


void DataStream::update(char* serialData, uint64_t updateTime){ // Data being sent over serail as ascii characters, thats why first input is type char
	
	if(*serialData!=0) // Check if null pointer is returned by readSerial function(that func called in main project file)
		{
			int serialDataSize=0;
			printf("This is the address of the buffer: %p\n",serialData);
			printf("This is the content of the buffer:");
			//determine the size of the array:
			for( ; serialData[serialDataSize]!=0; serialDataSize++)
			{
				printf("%c", serialData[serialDataSize]);

			}
			printf("\n");
			
			
			
			// Find the indice of the start Character
			char *startCharPtr = std::find(serialData, serialData + serialDataSize, startCharTag);
			char *stopCharPtr = std::find(serialData, serialData + serialDataSize, stopCharTag);
			
			//Check to make sure the last element was not returned, and that the find was succesfful!
			

			if (startCharPtr != serialData + serialDataSize){
			std::cout << "Start character Element found : " << *startCharPtr << '\n';
  
				if (stopCharPtr != serialData + serialDataSize){
					std::cout << "Stop character Element found : " << *stopCharPtr << '\n';
					
					char dataChars[stopCharPtr-startCharPtr]; // Note the size has to be 1 more than the number of data chars, becauase we need to end with null character for use of the atoi function
					int idx = 0;
					for(;idx<(stopCharPtr-startCharPtr-1);idx++){
					dataChars[idx]=*(startCharPtr+1+idx);

					}
					dataChars[stopCharPtr-startCharPtr-1]='\0';	// null terminate in order to not break atoi
					
					float scaledValue;
					scaledValue = atof(dataChars)/streamScaling;
					
					//popoulate and shift values for simpleMAData array
					idx=1;
						for(;idx<simpleMALag;idx++){
						simpleMAData[idx-1] = simpleMAData[idx];	// Shift data, overwrite oldest
						}
						simpleMAData[idx-1] = scaledValue;	// Add latest value to array
					
					
					//popoulate and shift values for weightedMAData array
					idx=1;
						for(;idx<weightedMALag;idx++){
						weightedMAData[idx-1] = weightedMAData[idx];	// Shift data, overwrite oldest
						}
						weightedMAData[idx-1] = scaledValue;	// Add latest value to array
					
				}
				else
			std::cout << "Stop character Element NOT found :\n";
  
			}
			else
			std::cout << "Start character Element NOT found :\n";
		
			
			///////////////////////////////////////////////////////////////////////////////////////
			// char outputData[256];
			// int serialDataIndex=0;
			// printf("This is the address of the buffer: %p\n",ptr);
			// printf("This is the content of the buffer:");
			// for( ; *ptr!=0; ++ptr)
			// {
				// printf("%c", *ptr);
				// serialData[serialDataIndex]=*ptr;
				// serialDataIndex++;
			// }
			// printf("\n");
			// serialData[serialDataIndex]='\0';
			// int serialDataLength = serialDataIndex;
			/* if((serialData[0]=='A') && (serialData[serialDataIndex-	3]== 'B'))
			{
				
				**********************************************************************************************************************************
												Moving Average Display of Data Update Rate
				***********************************************************************************************************************************/
				
				// lastUpdateTime = updateTime;
				// updateTime = bcm2835_st_read();
				// uint64_t deltaUpdateTime = (updateTime - lastUpdateTime);
				// if(deltaUpdateTime==0) deltaUpdateTime=1;
				// float updateRate = 1000000./deltaUpdateTime;
				// updateRates[updateCount] = updateRate;
				
				// if(updateCount==9)
				// {
					// int i=0;
					// sampleUpdateRate = 0;
					// for(i;i<=9;i++)
					// {
						// sampleUpdateRate+= 0.1*updateRates[i];
						
					// }
					// updateCount = 0;
				// }
				// updateCount++;
				
				// printf("Good Message Received!\n");
				// int outputDataIndex = 1;
				// for(outputDataIndex; outputDataIndex<serialDataLength; outputDataIndex++)
				// {
					// outputData[outputDataIndex-1] = serialData[outputDataIndex];
				// }
				// outputData[outputDataIndex] = '\0';	// null terminate in order to not break atoi
				// pressure0 = atoi(outputData);
				// printf("Processed Pressure = %i\n", pressure0);
			// }
			// else printf("Message processing error!!\n");
		}
	
}
	
	
	
	
	
	
	
	
	
	
	
	