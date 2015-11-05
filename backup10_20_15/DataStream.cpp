using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "DataStream.h"
#include <cmath>

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

float* DataStream::getRawDatum(){
	return &(simpleMAData[simpleMALag-1]);
}


float* DataStream::getWeightedMADatum(){
	
	int i = 0;
	float datum = 0;
	float coeffSum = 0;
	for(;i<weightedMALag;i++){
		datum+= (weightedMAData[i])*(weightedMACoeffs[i]);
		coeffSum+=weightedMACoeffs[i];
	}
	
	datum = datum/coeffSum;
	return &datum;
}

float* DataStream::getSimpleMADatum(){
	
	int i = 0;
	float datum = 0;
	for(;i<simpleMALag;i++){
		datum+= simpleMAData[i];
	}
	datum = datum/simpleMALag;
	return &datum;
}

float DataStream::getRawUpdateRate(){
	return 0;
}

float DataStream::getReadoutUpdateRate(){
	return 0;
}


void DataStream::update(char* dataOverSerial, uint64_t timeStamp){ // Data being sent over serail as ascii characters, thats why first input is type char
	
	
}
	
	
	
	
	
	
	
	
	
	
	
	