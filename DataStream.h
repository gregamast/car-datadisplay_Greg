// The DataStream header file: define DataStream class and headers

#ifndef DATASTREAM_H
#define DATASTREAM_H 




/******************************************************************************************************************************
									DATA STREAM CLASS
******************************************************************************************************************************/
class DataStream{
private: //This is an access_specifier.  
	
	int streamDepth;
	char startCharTag;
	char stopCharTag;
	
	float streamScaling; // The reverse scaling to drive the arudino value back into the proper Eng Units
	float** weightedMAData;
	float** simpleMAData;
	float** weightedMACoeffs;
	int* simpleMALag; //this is an integer array: for multiiple ranges
	int* weightedMALag; //this is an integer array: for multiiple ranges
	
	float* rangeStart; // The min value to every range
	float* rangeStop; //The max value to every range
	float* rangeScaling; // the scaling value associated for every range
	string* EngUnits;
	
	float readoutFreq;
	
	int numRanges;
	
	int currRange;
	int lastRange;

	// Timing Related Member Properties
	uint64_t lastTime;
	uint64_t currTime;
	float updateRate;			// Rate at which the data stream object is being updated (when the Datastream.update method is being called)
	
	
	
	
public: //This is an access_specifier.
	
	
	DataStream(int); // takes in the numranges
	
	void setCharTags(char , char);
	void setStreamScaling(float); // this is because arduino sends in the psi data *100 for boost gauge example
	void setRangeLimits(float, float ,int);// Set drange limits 		(min, max, range #)
	
	void setWeightedMALag(int, int); // number of previous data points to use in filter, the applicable range number ID
	void setWeightedMACoeffs(float*, int);
	void setSimpleMALag(int, int); // number of previous data points to use in filter
	void setEngUnits(string , int);
	void setRangeScaling(float, int);
	
	void setReadoutFreq(int);
	
	
	

	float getRawDatum(void); // oujtputs a float of eng units
	float getWeightedMADatum(void);
	float getSimpleMADatum(void);
	float getRawUpdateRate(void);
	float getReadoutUpdateRate(void);
	string getEngUnits(void);
	
	
	void update(char* , uint64_t); // Data being sent over serail as ascii characters, thats why first input is type char
	
private: //This is an access_specifier. 
	
	
};


#endif