#include <iostream>
using namespace std;

#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>			//Used for UART

#include "VG/openvg.h"			//\\
#include "VG/vgu.h"			// \\
#include "fontinfo.h"			// OpenVG
#include "shapes.h"			// //
#include <stdlib.h>			////

#include <stdlib.h> 			// Used for atoi

#include "math.h"
int openSerial(void);
unsigned char* readSerial(int);
unsigned char rx_buffer[256];
void graphics(float);
float degToRad(float);
int width, height;

int main()	
{
	
	init(&width, &height); // INIT GRAPHICS

	cout<< "Hello Project"<< endl;
	int uart0_filestream = openSerial();
	cout<< "Serial port opened" <<endl;
	int pressure0 = 0;  // This is the current value of pressure read in from the arduino through the UART on the RPi
	int pressure1 = 0; // THis is the first last pass value to pressure; for purposes of the moving average smoothing functionality
	int pressure2 = 0; // THis is the first last pass value to pressure; for purposes of the moving average smoothing functionality

	int pressureFilt = 0; // This is the filtered pressure value for display on screen

	while(1)
	{

		unsigned char *ptr = readSerial(uart0_filestream);

		if(*ptr!=NULL)
		{
			char serialData[256];
			char outputData[256];
			int serialDataIndex=0;
			printf("This is the address of the buffer: %p\n",ptr);
			printf("This is the content of the buffer:");
			for( ; *ptr!=NULL; ++ptr)
			{
				printf("%c", *ptr);
				serialData[serialDataIndex]=*ptr;
				serialDataIndex++;
			}
			printf("\n");
			serialData[serialDataIndex]='\0';
			int serialDataLength = serialDataIndex;
			if((serialData[0]=='A') && (serialData[serialDataIndex-	3]== 'B'))
			{
				printf("Good Message Received!\n");
				int outputDataIndex = 1;
				for(outputDataIndex; outputDataIndex<serialDataLength; outputDataIndex++)
				{
					outputData[outputDataIndex-1] = serialData[outputDataIndex];
				}
				outputData[outputDataIndex] = '\0';	// null terminate in order to not break atoi
				pressure0 = atoi(outputData);
				printf("Processed Pressure = %i\n", pressure0);
			}
			else printf("Message processing error!!\n");
		}

		pressureFilt = 0.5*pressure0 + 0.25*pressure1 + 0.25*pressure2;
		pressure2 = pressure1;
		pressure1 = pressure0;
		float PSI = pressureFilt/100.;
		graphics(PSI);

	}
	return 0;
}

void graphics(float pressure)
{
	Start(width, height);				   // Start the picture
	Background(0, 0, 0);				   // Black background
	int borderWidth = 5;
	StrokeWidth(borderWidth);
	Stroke(44, 77, 232, 1);
	Fill( 0 , 0 , 0 , 1 );				   // Big blue marble
	Circle(width / 2, height/2 , height-(2*borderWidth));

	StrokeWidth(borderWidth+2.5);
	float alpha = 0.5;
	Stroke(44,77,232,alpha);			   // The "world"
	Circle(width/2, height/2, height-(2*borderWidth));
	
	StrokeWidth(borderWidth+6);
	Stroke(44,77,232,0.2);
	Circle(width/2, height/2, height-(2*borderWidth));
	
	Fill(255, 255, 255, 1);				   // White text
	char text[10];
	sprintf(text, "%.2f", pressure);
	TextMid(width / 2, height / 2, text, SerifTypeface, width / 10);	// Greetings 


	int centerX = width/2;
	int centerY = height/2;
	int needleLength = 200;
	int centerRadius = 10;
	int centerRadiusFade = 15;
	int needleLengthFade = 220;

	//int angle = 0;
	float angle = 10*pressure;
	
	float xVertices[3];
	float yVertices[3];

	float xVertFade[3];
	float yVertFade[3];

	int fadeTh = 50;	//Fade thickness
	xVertices[0] = centerX - centerRadius*cos(degToRad(angle+90));
	yVertices[0]  = centerY + centerRadius*sin(degToRad(angle+90));
	xVertFade[0] = centerX - centerRadiusFade*cos(degToRad(angle+90));
	yVertFade[0]  = centerY + centerRadiusFade*sin(degToRad(angle+90));


	xVertices[1] = centerX - needleLength*cos(degToRad(angle));
	yVertices[1] = centerY + needleLength*sin(degToRad(angle));
	xVertFade[1] = centerX - needleLengthFade*cos(degToRad(angle));
	yVertFade[1] = centerY + needleLengthFade*sin(degToRad(angle));

	xVertices[2] = centerX - centerRadius*cos(degToRad(angle-90));
	yVertices[2] = centerY + centerRadius*sin(degToRad(angle-90));
	xVertFade[2] = centerX - centerRadiusFade*cos(degToRad(angle-90));
	yVertFade[2] = centerY + centerRadiusFade*sin(degToRad(angle-90));


	
	Fill(255,0,0,0.4);
	Polygon(xVertFade,yVertFade,3);
	Circle(centerX,centerY,(centerRadiusFade)*2);


	Fill(255,0,0,1);			// Fill colore for needle
	Polygon( xVertices , yVertices , 3 );

	StrokeWidth(0);
	Circle(centerX,centerY,(centerRadius)*2);

	Fill(255,255,255,1);
	Rect(300,300,10,10);
	Rotate(30);
	Rect(300,300,10,10);





	End();						   // End the picture



}



int openSerial()
{
	//-------------------------
	//----- SETUP USART 0 -----
	//-------------------------
	//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
	int uart0_filestream = -1;
	
	//OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//		O_RDONLY - Open for reading only.
	//		O_RDWR - Open for reading and writing.
	//		O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//											immediately with a failure status if the output can't be written immediately.
	//
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
	uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
		return -1;
	}
	
	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
	return uart0_filestream;
}


unsigned char* readSerial(int uart0_filestream)
{
//----- CHECK FOR ANY RX BYTES -----
	rx_buffer[0] = '\0';
	if (uart0_filestream != -1)
	{
		// Read up to 255 characters from the port if they are there
		int rx_length = read(uart0_filestream, (void*)rx_buffer, 255);		//Filestream, buffer to store in, number of bytes to read (max)
		if (rx_length < 0)
		{
			//An error occured (will occur if there are no bytes)
		}
		else if (rx_length == 0)
		{
			//No data waiting
		}
		else
		{
			//Bytes received
			rx_buffer[rx_length] = '\0';
			printf("%i bytes read : %s\n", rx_length, rx_buffer);
		}
	}
	return rx_buffer;
}


float degToRad(float degrees)
{

float radians = degrees*3.14159/180.0;
return radians;



}
