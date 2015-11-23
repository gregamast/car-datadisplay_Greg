using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "DataStream.h"
#include <cmath>
#include <algorithm>    // std::find

#include <string>

#include<bcm2835.h>


DataStream::DataStream( int ranges ){
	numRanges = ranges;
	lastRange = 1;
	currRange = 1;

	lastTime = 0;
	currTime = 0;
	updateRate = 0;
	
	
	weightedMAData = new float*[numRanges];
	simpleMAData = new float*[numRanges];
	weightedMACoeffs = new float*[numRanges];
	simpleMALag = new int[numRanges]; //this is an integer array: for multiiple ranges
	weightedMALag= new int[numRanges]; //this is an integer array: for multiiple ranges
	
	rangeStart = new float[numRanges]; // The min value to every range
	rangeStop = new float[numRanges]; //The max value to every range
	rangeScaling = new float[numRanges]; // the scaling value associated for every range
	EngUnits = new string[numRanges];
	
	
}

void DataStream::setCharTags(char startTag, char stopTag){
	startCharTag = startTag;
	stopCharTag = stopTag;
}

////////////////////////////////////


void DataStream::setRangeLimits(float min, float max, int range){
	rangeStart[range-1]=min;
	rangeStop[range-1]=max;
}

void DataStream::setRangeScaling(float scaling, int range){
	rangeScaling[range-1]=scaling;
}

void DataStream::setEngUnits(string EU, int range){
	EngUnits[range-1]=EU;
	//cout<<"this is the unit "<<EngUnits[range-1]<<endl;
}



/////////////////////////////////////

void DataStream::setStreamScaling(float scaling){
	streamScaling = scaling;
}

void DataStream::setWeightedMALag(int lag, int range){
	weightedMALag[range-1] = lag;
	
	// Since with lag we now know size, we should initilize the arrays to data 0
	weightedMAData[range-1]   = new float[lag];
	weightedMACoeffs[range-1] = new float[lag];
	int i=0;
	float* currMAData= weightedMAData[range-1];
	float* currMACoeffs  =weightedMACoeffs[range-1];
	for(;i<lag;i++){
		currMAData[i]=0;
		currMACoeffs[i]=0;
	}
}






void DataStream::setSimpleMALag(int lag, int range){
	simpleMALag[range-1] = lag;
	
	// Since with lag we now know size, we should initilize the arrays to data 0
	simpleMAData[range-1] = new float[lag];
	int i=0;
	float* currMAData= simpleMAData[range-1];
	for(;i<lag;i++){
		currMAData[i]=0;
	}
}

void DataStream::setWeightedMACoeffs( float* coeffs, int range){
	int i=0;
	float* currMACoeffs  =weightedMACoeffs[range-1];
	for(;i<weightedMALag[range-1];i++){
		currMACoeffs[i]=coeffs[i];
	}
}

void DataStream::setReadoutFreq(int freq){
	readoutFreq = freq;
}

string DataStream::getEngUnits(void){
	return EngUnits[currRange-1];
}


//////////////////////////////////////////////////

float DataStream::getRawDatum(){
	float* currSimpleMAData = simpleMAData[currRange-1];
	return (currSimpleMAData[simpleMALag[currRange-1]-1]);
}


float DataStream::getWeightedMADatum(){
	float* currWeightedMAData = weightedMAData[currRange-1];
	float* currentWeightedMACoeffs = weightedMACoeffs[currRange-1];
	
	int i = 0;
	float datum = 0;
	float coeffSum = 0;
	for(;i<weightedMALag[currRange-1];i++){
		datum+= (currWeightedMAData[i])*(currentWeightedMACoeffs[i]);
		coeffSum+=currentWeightedMACoeffs[i];
	}
	
	datum = datum/coeffSum;
	return datum;
}

float DataStream::getSimpleMADatum(){
	float* currSimpleMAData = simpleMAData[currRange-1];
	
	int i = 0;
	float datum = 0;
	for(;i<simpleMALag[currRange-1];i++){
		datum+= currSimpleMAData[i];
	}
	datum = datum/simpleMALag[currRange-1];
	return datum;
}

float DataStream::getRawUpdateRate(){
	return updateRate;
}

float DataStream::getReadoutUpdateRate(){
	return 0;
}


void DataStream::update(char* serialData, uint64_t updateTime){ // Data being sent over serail as ascii characters, thats why first input is type char
	
	// Calculate update time
	currTime = updateTime;
	uint64_t timeDelta = currTime - lastTime;
	lastTime = currTime;
	updateRate = 1000000./timeDelta;



	if(*serialData!=0) // Check if null pointer is returned by readSerial function(that func called in main project file)
		{
			int serialDataSize=0;
			//printf("This is the address of the buffer: %p\n",serialData);
			//printf("This is the content of the buffer:");
			//determine the size of the array:
			for( ; serialData[serialDataSize]!=0; serialDataSize++)
			{
				//printf("%c", serialData[serialDataSize]);

			}
			//printf("\n");
			
			
			
			// Find the indice of the start Character
			char *startCharPtr = std::find(serialData, serialData + serialDataSize, startCharTag);
			char *stopCharPtr = std::find(serialData, serialData + serialDataSize, stopCharTag);
			
			//Check to make sure the last element was not returned, and that the find was succesfful!
			

			if (startCharPtr != serialData + serialDataSize){
			//std::cout << "Start character Element found : " << *startCharPtr << '\n';
  
				if (stopCharPtr != serialData + serialDataSize){
					//std::cout << "Stop character Element found : " << *stopCharPtr << '\n';
					
					char dataChars[stopCharPtr-startCharPtr]; // Note the size has to be 1 more than the number of data chars, becauase we need to end with null character for use of the atoi function
					int idx = 0;
					for(;idx<(stopCharPtr-startCharPtr-1);idx++){
					dataChars[idx]=*(startCharPtr+1+idx);

					}
					dataChars[stopCharPtr-startCharPtr-1]='\0';	// null terminate in order to not break atof
					
					float scaledValue;
					scaledValue = atof(dataChars)/streamScaling;
					
					// Find out which range the stream-scaled value belongs too
					int range = 0;
					bool rangeFound = false;
					
					
					while(!rangeFound){
						//cout<<"range: "<<range<<"numRanges: "<<numRanges<<endl;
						if(range==numRanges) {
							cout<<"Range not found"<<endl;
							break;
						}
						
						//cout<<"rangeStart: "<<rangeStart[range]<<"rangeStop: "<<rangeStop[range]<<endl;
						if(scaledValue>=rangeStart[range] && scaledValue<rangeStop[range]){
							lastRange = currRange;
							rangeFound = true;
							currRange = range+1;
							//cout<<"Range is found to be: "<<currRange<<endl;
						}
						range++;
				
					}
					
					// We are checking to see values have switched ranges. if so, populate entire MAdata vectors with current value
					
					if(rangeFound){
						
						float* currSimpleMAData = simpleMAData[currRange-1]; // Creates pointer to first memory adress which stores the SMAData for the current range
						float* currWeightedMAData = weightedMAData[currRange-1]; // Creates pointer to first memory adress which stores the WMAData for the current range
						//apply proper scaling per range
						scaledValue=scaledValue*rangeScaling[currRange-1];
						
						// cout<<"This is the final scaled value: "<<scaledValue<<" , and this is the range: "<<currRange<<endl;
						if(currRange!=lastRange){
							idx=1;
							
							for(;idx<simpleMALag[currRange-1];idx++){
								currSimpleMAData[idx-1] = scaledValue; 
								
							}
							
							idx=1;
							for(;idx<weightedMALag[currRange-1];idx++){
								currWeightedMAData[idx-1] = scaledValue; 
								
							}
						}
						
						// Perform operation of shifting all data values over 1 in array, and adding in new data value
						idx=1;
						for(;idx<simpleMALag[currRange-1];idx++){
						currSimpleMAData[idx-1] = currSimpleMAData[idx];	// Shift data, overwrite oldest
						}
						currSimpleMAData[idx-1] = scaledValue;	// Add latest value to array
						
						idx=1;
						for(;idx<weightedMALag[currRange-1];idx++){
						currWeightedMAData[idx-1] = currWeightedMAData[idx];	// Shift data, overwrite oldest
						}
						currWeightedMAData[idx-1] = scaledValue;	// Add latest value to array
						
							
					}
					
				
					
					
					
			
				}
				else
			std::cout << "Stop character Element NOT found :\n";
  
			}
			else
			std::cout << "Start character Element NOT found :\n";
		
		}
	
}
	
	
	
	
	
	
	
	
	
	
	
	