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
	float* weightedMAData;
	float* simpleMAData;
	float* weightedMACoeffs;
	int simpleMALag;
	int weightedMALag;
	float readoutFreq;
	
	
	
	
	public: //This is an access_specifier.
	
	
	DataStream(int); //takes in depth: size to descibe # of data points to hold
	
	void setCharTags(char , char);
	void setScaling(float);
	void setWeightedMALag(int); // number of previous data points to use in filter
	void setWeightedMACoeffs(float*);
	void setSimpleMALag(int); // number of previous data points to use in filter
	void setReadoutFreq(int);

	float* getRawDatum(void); // oujtputs a float of eng units
	float* getWeightedMADatum(void);
	float* getSimpleMADatum(void);
	float getRawUpdateRate(void);
	float getReadoutUpdateRate(void);
	
	void update(char* , uint64_t); // Data being sent over serail as ascii characters, thats why first input is type char
	
	private: //This is an access_specifier. 
	
	
};


#endif