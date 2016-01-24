// The DataStream header file: define DataStream class and headers

#ifndef PID_H
#define PID_H 




/******************************************************************************************************************************
									DATA STREAM CLASS
******************************************************************************************************************************/
class PID{
private: //This is an access_specifier.  
	
	string fullName;
	string shortName;
	string id;
	int numDataBytes;
	float byteGain[4];
	float byteOffset[4];
	

	float totalGain , totalOffset;
	
	float supportedMinVal , supportedMaxVal;
	
	int numRanges;
	
	
	// Range specific properties
	
	float*	scaling;
	float* rangeStart;
	float* rangeStop; //The min and max value to every range
	string* engUnits;
	
	int* simpleMALag; //this is an integer array: for multiiple ranges
	int* weightedMALag; //this is an integer array: for multiiple ranges
	float** weightedMACoeffs;
	
	int currRange;
	int lastRange;

	float** weightedMAData;
	float** simpleMAData;
	
	
	
	
	float* rangeScaling; // the scaling value associated for every range
	
	
	float readoutFreq;
	
	

	// Timing Related Member Properties
	uint64_t lastTime;
	uint64_t currTime;
	float updateRate;			// Rate at which the data stream object is being updated (when the Datastream.update method is being called)
	
	
	
	
public: //This is an access_specifier.
	
	
	PID(string); // takes in name of identifier to find PID configuration in PID configuration file
	
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
	
	
	void update(string , uint64_t); // Data being sent over serail as ascii string, thats why first input is type char
	
private: //This is an access_specifier. 

	void configure( string ); //COnfigure the PID from PIDConf file
	
	
};


#endif