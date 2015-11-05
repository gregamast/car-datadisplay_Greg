
using namespace std;
#include <stdio.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "Gauge.h"
#include <cmath>

#include "touchScreen.h"
#include "TouchableObject.h"





/****************************************************************
	CONSTRUCTOR
****************************************************************/
TouchableObject::TouchableObject( void ){
	isRectangular = true;
	cX = 0;
	cY = 0;
	cRad = 0;
	rW = 0;
	rH = 0;
	rblX = 0;
	rblY = 0;

	touchEnabled = false;
	visible = false;
	lpVisible = false;
	touched = false;
	
	
}


/****************************************************************
	
	****************************************************************/


/****************************************************************
		
	****************************************************************/






/****************************************************************
	Define SET METHODS
****************************************************************/


bool TouchableObject::getVisibility(void){
	return visible;
	
}

bool TouchableObject::getLPVisibility(void){
	return lpVisible;
	
}

void TouchableObject::setCircular(void){
	isRectangular = false;
	
}

void TouchableObject::setCircleCenter(int centerx , int centery){
	cX = centerx;
	cY = centery;
	
}

void TouchableObject::setCircleRadius(int crad){
	
	cRad = crad;
}

void TouchableObject::setRectangular(void){
	isRectangular = true;
	
}

void TouchableObject::setRectWidthHeight(int w, int h){
	rW = w;
	rH = h;
	
}

void TouchableObject::setRectBottomLeft(int rblX , int rblY){
	rblX = rblX;
	rblY = rblY;
}


void TouchableObject::touchEnable(void){
	touchEnabled = true;

}

void TouchableObject::touchDisable(void){
	touchEnabled = false;

}

void TouchableObject::setVisible(void){
	lpVisible = visible;
	visible = true;

}


void TouchableObject::setInvisible(void){
	lpVisible = visible;
	visible = false;

}

void TouchableObject::updateTouch(touch_t touchStruct){
	
	if(touchEnabled){
		std::cout<<"touch is enabled"<<endl;
		if(touchStruct.btn_touch){
			std::cout<<"touchStruct.btn_touch is TRUE"<<endl;
			if(isRectangular){
				std::cout<<"Is Rectangular"<<endl;
				
				int xMin = rblX;
				int xMax = rblX+rW;
				int yMin = rblY-rH;
				int yMax = rblY;
				
				if(touchStruct.abs_x>= xMin && touchStruct.abs_x<= xMax && touchStruct.abs_y>= yMin && touchStruct.abs_y<= yMax){
					touched = true;
					std::cout<<"Touched is TRUE"<<endl;
				}
				else{
					touched = false;
					std::cout<<"Touched is FALSE"<<endl;
				}
			}
			else{//the circular case:
			
			std::cout<<"Is Circular"<<endl;
			float dx = touchStruct.abs_x-cX;
			float dy = touchStruct.abs_y-cY;
			
				if(sqrt(pow(dx,2)+pow(dy,2))<=cRad){
					touched = true;
					std::cout<<"Touched is TRUE"<<endl;
				}
				else{
					touched = false;
					std::cout<<"Touched is FALSE"<<endl;
				}
			}
		} // End IF button touch
		else touched=false;
	} // End IF touch enabled
	
	
}// End TouchableObject::updateTouch



bool TouchableObject::isTouched(void){
	return touched;
}















