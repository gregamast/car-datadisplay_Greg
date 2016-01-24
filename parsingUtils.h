// Parsing Utities Header

// This is start of the header guard.  ADD_H can be any unique name.  By convention, we use the name of the header file.
#ifndef PARSINGUTILS_H
#define PARSINGUTILS_H

#include <config4cpp/Configuration.h>
#include <string>
using namespace config4cpp;
using namespace std;



int parseInt(Configuration * , string , string );
/* Parse integer inside subscope */


int parseInt(Configuration * , string , string , string );

string parseString(Configuration * , string , string );


/* Parse string inside subscope */
string parseString(Configuration * , string , string , string );


/* Parse float */
float parseFloat(Configuration * , string , string );

/* Parse float inside subscope */
float parseFloat(Configuration * , string , string , string );

/* Parse color inside subscope */
void parseColor(Configuration * , string , string , float* , string );


/* Parse color */
void parseColor(Configuration * , string , float* , string );



/* Parse boolean inside subscope */
bool parseBool(Configuration * , string , string , string );
bool parseBool(Configuration * , string , string );

void parseFloatArray(Configuration *  , string , int , float* , string);
void parseFloatArray(Configuration *  , string , string ,  int , float* , string);





#endif