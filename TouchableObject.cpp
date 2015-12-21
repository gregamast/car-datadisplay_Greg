
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
	rX = 0;
	rY = 0;
	
	
	touchEnabled = false;
	visible = false;
	lpVisible = false;
	touched = false;
	
	// Initialize the fade Percentage attributes
	fadePercentage = 0;
	finalFadePercentage = 0;
	


	
	
}

void TouchableObject::move(int deltaX, int deltaY, int transTime, string motionType ){
	
	// First check to make sure user is not attempting to recommand current move
	if(moveStartRX+deltaX!=finalPosX || moveStartRY+deltaY !=finalPosY)
	{
		//set initial position
		if(isRectangular){
		
			moveStartRX = rX;
			moveStartRY = rY;
			finalPosX = moveStartRX+deltaX;
			finalPosY = moveStartRY+deltaY;
		}
		else{
			moveStartCX = cX;
			moveStartCY = cY;
			finalPosX = moveStartCX+deltaX;
			finalPosY = moveStartCY+deltaY;
		}
				
	
		
			
		moveDuration = transTime;
		moveStartTime =  bcm2835_st_read(); //This is microseconds
		motionType.assign(motionType);
	}
}

void TouchableObject::moveTo(int finalX, int finalY, int transTime, string motionType ){
	if(finalX!=finalPosX || finalY !=finalPosY)
	{
		//set initial position
		if(isRectangular){
		
			moveStartRX = rX;
			moveStartRY = rY;
		}
		else{
			moveStartCX = cX;
			moveStartCY = cY;
		}
				
		finalPosX = finalX;
		finalPosY = finalY;
		
			
		moveDuration = transTime;
		moveStartTime =  bcm2835_st_read(); //This is microseconds
		motionType.assign(motionType);
	}
}

void TouchableObject::fade(int finalPercentage, int duration, string type){
	
	if(finalPercentage!=finalFadePercentage){
		initialFadePercentage = fadePercentage;
			
		finalFadePercentage = finalPercentage;
			
		fadeDuration = duration;
		fadeStartTime =  bcm2835_st_read(); //This is microseconds
		fadeType.assign(type);
	}
	
	
}

void TouchableObject::updateVisuals(void)
{
	if(isRectangular){
		if(rX!=finalPosX || rY !=finalPosY){
			
			int totalPixDisp = sqrt(pow(finalPosX-moveStartRX,2) + pow(finalPosY-moveStartRY,2) );
			float omega = (M_PI/2)/(moveDuration); //this is ang velocity, or omega
			uint64_t currTime = bcm2835_st_read(); //This is in Microseconds (according to Mark)
			
			int deltaPixDisp = totalPixDisp*sin(omega/1000*(currTime-moveStartTime)); //convert to int in here, also convert ms to s
			double angle = atan2( finalPosY - moveStartRY, finalPosX-moveStartRX );//This takes in to account sign and reports out 0-2pi, so its know the quadrant
			
			
			if(currTime > (moveStartTime + moveDuration*1000)){
				rX = finalPosX;
				rY = finalPosY;
			}
			else{
				
				
				rX = moveStartRX + deltaPixDisp*cos(angle);
				rY = moveStartRY + deltaPixDisp*sin(angle);
			}
			
		}
	}
	
	if(fadePercentage!=finalFadePercentage){

		
		int totalFadePercentage = (finalFadePercentage-initialFadePercentage);
			float omega = (M_PI/2)/(fadeDuration); //this is ang velocity, or omega
			uint64_t currTime = bcm2835_st_read(); //This is in Microseconds (according to Mark)
			
			int deltaFadePercentage = totalFadePercentage*sin(omega/1000*(currTime-fadeStartTime)); //convert to int in here, also convert ms to s
			
			if(currTime > (fadeStartTime + fadeDuration*1000)){
				
				fadePercentage = finalFadePercentage;
				
			}
			else{
				
				fadePercentage = initialFadePercentage + deltaFadePercentage;
				
			}
		
		
	}

	
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

void TouchableObject::setRectCenter(int X , int Y){
	rX = X;
	rY = Y;

	finalPosX = rX;
	finalPosY = rY;
	
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
		//std::cout<<"touch is enabled"<<endl;
		if(touchStruct.btn_touch){
			//std::cout<<"touchStruct.btn_touch is TRUE"<<endl;
			if(isRectangular){
				//std::cout<<"Is Rectangular"<<endl;
				
				int xMin = rX-rW/2;
				int xMax = rX+rW/2;
				int yMin = rY-rH/2;
				int yMax = rY+rH/2;
				
				if(touchStruct.abs_x>= xMin && touchStruct.abs_x<= xMax && touchStruct.abs_y>= yMin && touchStruct.abs_y<= yMax){
					touched = true;
					//std::cout<<"Touched is TRUE"<<endl;
				}
				else{
					touched = false;
					//std::cout<<"Touched is FALSE"<<endl;
				}
			}
			else{//the circular case:
				
				//std::cout<<"Is Circular"<<endl;
				float dx = touchStruct.abs_x-cX;
				float dy = touchStruct.abs_y-cY;
				
				if(sqrt(pow(dx,2)+pow(dy,2))<=cRad){
					touched = true;
					//std::cout<<"Touched is TRUE"<<endl;
				}
				else{
					touched = false;
					//std::cout<<"Touched is FALSE"<<endl;
				}
			}
		} // End IF button touch
		else touched=false;
	} // End IF touch enabled
	
	
}// End TouchableObject::updateTouch



bool TouchableObject::isTouched(void){
	return touched;
}







int TouchableObject::getDesiredPosX(void){
	if(isRectangular)
	return rX;
	else
	return cX;
	
}
int TouchableObject::getDesiredPosY(void){
	
	if(isRectangular)
	return rY;
	else
	return cY;
}


int TouchableObject::getCurrentPosX(void){
	return rX;
	
}

int TouchableObject::getCurrentPosY(void){
	
	return rY;
}

int TouchableObject::getDesiredFadePercentage(void){
	
	return fadePercentage;
}








