using namespace std;
#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include <iostream>
#include "PID.h"
#include <cmath>
#include <algorithm>    // std::find

#include <string>

#include<bcm2835.h>

#include <locale.h>
#include <config4cpp/Configuration.h>
#include "parsingUtils.h"

using namespace config4cpp;


PID::PID( string PIDid ){
	id = PIDid;
	
	lastRange = 1;
	currRange = 1;

	lastTime = 0;
	currTime = 0;
	updateRate = 0;
	
	
	configure( PIDid);//configure should happen in the PID constructor
	
}

void PID::configure(string configType){
	
	setlocale(LC_ALL, "");
	
	string scope = configType;
	string rangeScope = "range";
	int currentRange = 1;
	Configuration * cfg = Configuration::create();
	
	try{
		
		cfg->parse("/home/pi/openvg/client/PIDConf.txt"); //use full path name so program can run from startup and not break...
		
		
		numDataBytes	=   parseInt( 		cfg , scope , "numDataBytes" 	);
		numRanges 		=	parseInt(		cfg , scope , "numRanges"		);
		fullName 		=	parseString( 	cfg , scope , "fullName" 		);
		shortName 		=	parseString( 	cfg , scope , "shortName" 		);

		
		byteGain[0]			=   parseFloat( 	cfg , scope , "AGain"		 	);
		byteGain[1]			=   parseFloat( 	cfg , scope , "BGain"		 	);
		byteGain[2]			=   parseFloat( 	cfg , scope , "CGain" 			);
		byteGain[3]			=   parseFloat( 	cfg , scope , "DGain" 			);
		byteOffset[0]			=   parseFloat( 	cfg , scope , "AOffset" 		);
		byteOffset[1]			=   parseFloat( 	cfg , scope , "BOffset" 		);
		byteOffset[2]			=   parseFloat( 	cfg , scope , "COffset" 		);
		byteOffset[3]			=   parseFloat( 	cfg , scope , "DOffset" 		);
		totalGain		=   parseFloat( 	cfg , scope , "totalGain" 		);
		totalOffset		=   parseFloat( 	cfg , scope , "totalOffset" 	);
		
		supportedMinVal	=   parseFloat( 	cfg , scope , "supportedMinVal" );
		supportedMaxVal =   parseFloat( 	cfg , scope , "supportedMaxVal" );
		
		
		
		/****************************************************************
		Instatiating Range Member Properties
			After we parse numRanges from the configuration file, we then	
			know we can set the new memory spaces arrays for the following
			properties.
		****************************************************************/
		engUnits 			= new string[numRanges];
		rangeStart 			= new float[numRanges];
		rangeStop			= new float[numRanges];
		scaling				= new float[numRanges];
		
		
		weightedMAData 		= new float*[numRanges];
		simpleMAData 		= new float*[numRanges];
		weightedMACoeffs 	= new float*[numRanges];
		simpleMALag 		= new int[numRanges]; //this is an integer array: for multiiple ranges
		weightedMALag		= new int[numRanges]; //this is an integer array: for multiiple ranges
		

		
		for(;currentRange<=numRanges;currentRange++){
			string currentRangeScope = rangeScope + to_string(currentRange);
			
			rangeStart[currentRange-1] 			= parseFloat(cfg , scope , currentRangeScope , ".rangeStart" 	);
			rangeStop[currentRange-1] 			= parseFloat(cfg , scope , currentRangeScope , ".rangeStop"	);
			scaling[currentRange-1] 			= parseFloat(cfg , scope , currentRangeScope , ".scaling"	);			
			simpleMALag[currentRange-1]			= parseInt(cfg , scope , currentRangeScope , ".simpleMALag");
			simpleMAData[currentRange-1]   		= new float[simpleMALag[currentRange-1]];
			
			// Let us intialize data to 0
			float * currentMAData = simpleMAData[currentRange-1];
			for(int i=0;i<simpleMALag[currentRange-1];i++){
				currentMAData[i]=0;
			}
			
			weightedMALag[currentRange-1]		= parseInt(cfg , scope , currentRangeScope , ".weightedMALag");
			weightedMAData[currentRange-1]   	= new float[weightedMALag[currentRange-1]];
			
			// Let us intialize data to 0
			currentMAData = weightedMAData[currentRange-1];
			
			for(int i=0;i<weightedMALag[currentRange-1];i++){
				currentMAData[i]=0;
			}
			
			//at this point, after parsing weightedMALag, we know the size of the "currentRange-1" floatarray to parse next_permutation
			weightedMACoeffs[currentRange-1] 	= new float[weightedMALag[currentRange-1]];
			//WE are passing the place to put the final array by reference, and so parseFLoatArray does not return anything, one of the
			//input arguments is in fact the containter (points to ) the place to put the parsed array
			parseFloatArray(cfg , scope , currentRangeScope ,  weightedMALag[currentRange-1], weightedMACoeffs[currentRange-1] , ".weightedMACoeffs" );			
			
			
			
		}
		
	}
	catch(const ConfigurationException & ex){
		cout << ex.c_str() << endl;
	}
	cfg->destroy();
	
	
}



string PID::getEngUnits(void){
	return engUnits[currRange-1];
}


float PID::getRawDatum(){
	float* currSimpleMAData = simpleMAData[currRange-1];
	return (currSimpleMAData[simpleMALag[currRange-1]-1]);
}


float PID::getWeightedMADatum(){
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

float PID::getSimpleMADatum(){
	float* currSimpleMAData = simpleMAData[currRange-1];
	
	int i = 0;
	float datum = 0;
	for(;i<simpleMALag[currRange-1];i++){
		datum+= currSimpleMAData[i];
	}
	datum = datum/simpleMALag[currRange-1];
	return datum;
}

float PID::getRawUpdateRate(){
	return updateRate;
}

float PID::getReadoutUpdateRate(){
	return 0;
}


void PID::update(string serialData, uint64_t updateTime){ // Data being sent over serail as ascii characters, thats why first input is type char
	
	cout<<serialData<<endl;
	// Calculate update time
	currTime = updateTime;
	uint64_t timeDelta = currTime - lastTime;
	lastTime = currTime;
	updateRate = 1000000./timeDelta;

	string responseID = id;
	responseID[0] = '4';
	
	cout<<responseID<<endl;


	if(!serialData.empty()){ // Check if null pointer is returned by readSerial function(that func called in main project file)
		
		// Find out which range the stream-scaled value belongs too
		int range = 0;
		bool rangeFound = false; 
		
		//remove spaces from the string
		std::string::iterator end_pos = std::remove(serialData.begin(),serialData.end(), ' ');
		serialData.erase( end_pos , serialData.end() );
		
		cout<<serialData<<endl;
		
		
		size_t found = serialData.find(responseID);
		
		cout<<found<<endl;
		if(found!=string::npos) {
			string dataBytesString = serialData.substr(found+4 , numDataBytes*2); //there are 2 characters per byte
			
			cout<<dataBytesString<<endl;
			float val = 0;
			
			cout<<"this is val: "<<val<<endl;
			for(int i=0;i<numDataBytes;i++){
				val+= byteGain[i] *  strtoul( dataBytesString.substr( 2*i , 2 ).c_str() , NULL , 16  ) + byteOffset[i];
			}
			cout<<"this is val: "<<val<<endl;
			
			val = totalGain*val+totalOffset;
			cout<<"this is val: "<<val<<endl;
			
			
			while(!rangeFound){
				//cout<<"range: "<<range<<"numRanges: "<<numRanges<<endl;
				if(range==numRanges) {
					cout<<"Range not found"<<endl;
					break;
				}
				
				//cout<<"rangeStart: "<<rangeStart[range]<<"rangeStop: "<<rangeStop[range]<<endl;
				if(val>=rangeStart[range] && val<rangeStop[range]){
					lastRange = currRange;
					rangeFound = true;
					currRange = range+1;
					cout<<"Range is found to be: "<<currRange<<endl;
				}
				range++;
				
			} //END OF WHILE
			
			// We are checking to see values have switched ranges. if so, populate entire MAdata vectors with current value
			int idx;
			if(rangeFound){
				
				float* currSimpleMAData = simpleMAData[currRange-1]; // Creates pointer to first memory adress which stores the SMAData for the current range
				float* currWeightedMAData = weightedMAData[currRange-1]; // Creates pointer to first memory adress which stores the WMAData for the current range
				//apply proper scaling per range
				val=val*scaling[currRange-1];
				
				 cout<<"This is the final scaled value: "<<val<<" , and this is the range: "<<currRange<<endl;
				if(currRange!=lastRange){
					idx=1;
					
					for(;idx<simpleMALag[currRange-1];idx++){
						currSimpleMAData[idx-1] = val; 
						
					}
					
					idx=1;
					for(;idx<weightedMALag[currRange-1];idx++){
						currWeightedMAData[idx-1] = val; 
						
					}
				}
				
				// Perform operation of shifting all data values over 1 in array, and adding in new data value
				idx=1;
				for(;idx<simpleMALag[currRange-1];idx++){
					currSimpleMAData[idx-1] = currSimpleMAData[idx];	// Shift data, overwrite oldest
				}
				currSimpleMAData[idx-1] = val;	// Add latest value to array
				
				idx=1;
				for(;idx<weightedMALag[currRange-1];idx++){
					currWeightedMAData[idx-1] = currWeightedMAData[idx];	// Shift data, overwrite oldest
				}
				currWeightedMAData[idx-1] = val;	// Add latest value to array
				
				
			} //END IF RANGEFOUND	
		}
		
		
	}
	else
	std::cout << "Stop character Element NOT found :\n";

	
	
	
	
}











