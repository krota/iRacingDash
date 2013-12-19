// irplugin.cpp : Defines the entry point for the console application.
//
// an example iracing plug applcation displaying drivers nam, number and car
// and real time rpm and gear.
//
// provided as is and has been tested to compile correctly on my machine.
// no warranty or otherwise esponsablilty implied.
//
// some sections of code are from community and iracing staff feedback and are referenced
// throughout where appropriate.
//
// ive added an additional header (irplugin.h) where i stick all my own functions as a habit
// and to neaten up the code to save from twalling lines for debugging issues.
//
// p.s i havne gone through and update the c string to c++ string declarations, but they work ok.
//
// have fun! :-)
//

#include "stdafx.h"

//bring in some usual headers for various tasks////
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <time.h>
#include <string>
#include <signal.h>
#include "serialArduino.h"
#include <math.h>

//bring in the irsdk headers
#include "irsdk_defines.h"	//irsdk
#include "yaml_parser.h"	//irsdk

// set stanard namespace
using namespace std;

// 16 ms timeout
#define TIMEOUT 16

std::string strAppVers = "App Version 0.1\n"; //defines a simple string for output as your app version, and newline.

char *data = NULL;
int nData = 0;

/////////////////////////////////////////////////////////
#include "irplugin.h"  //imports application specific custom functions
////////////////////////////////////////////////////////


int _tmain(int argc, _TCHAR* argv[])
{

	//inital application startup.

	system("cls"); //clear console
	printf("%s\n\n", strAppVers);
	printf("Press CTRL-C to exit.\n");

	// trap ctrl-c
	signal(SIGINT, ex_program);
		
	// bump priority up so we get time from the sim
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	// ask for 1ms timer so sleeps are more precise
	timeBeginPeriod(1);

	
	// arduino serial initialization 
	int dataSize = 256;

	Serial* SP = new Serial("\\\\.\\COM4");    // adjust as needed

	if (SP->IsConnected()) {
		printf("Connected to Arduino!\n");
	}else {
		printf("Failed to connect to Arduino\n");
	}

	printf("Waiting for session to start..\n");

	while(true)
	{
		if(irsdk_waitForDataReady(TIMEOUT, data))
		{
			const irsdk_header *pHeader = irsdk_getHeader();
			if(pHeader)
			{
				// if header changes size, assume a new connection
				if(!data || nData != pHeader->bufLen)
				{
					if(data) 
						delete [] data;
					nData = pHeader->bufLen;
					data = new char[nData];

					///////////////////////////
					system("cls");
					printf("%s\n\n", strAppVers);
					printf("Press CTRL-C to exit.\n");
					printf("connected to sim...\n\n");
					// process header here///////
					// this is where you access and display your session info, like track, driver, etc.
					// these are the 'sessioninfo' and 'weekendinfo' sections.
					//


					//////////////////////////////////////////
					const char *valuestr;
					int valuelen;
					if(parseYaml(irsdk_getSessionInfoStr(), "WeekendInfo:TrackName:" , &valuestr, &valuelen)){
						printf("Track: %.*s\n",valuelen,valuestr);
						}else{
							printf("not found\n");
					}
					// note: printf: .* = The precision is not specified in the format string, 
					// but as an additional integer value argument preceding the argument that has to be formatted.
					// this code Thanks to Peter holt.
					//////////////////////////////////////////

					//////////////////////////////GET LOADED CAR AND DRIVER///////////////////////////////////
					////thx to Dave Tucker for this section
					const char *valstr;
					int valstrlen; 
					char str[512];
					//
					int carIdx = -1;
					char nameStr[512];
					char pathStr[512];
					char numbStr[512];
					//
					// get the playerCarIdx
					if(parseYaml(irsdk_getSessionInfoStr(), "DriverInfo:DriverCarIdx:", &valstr, &valstrlen))
						carIdx = atoi(valstr);
					////
					if(carIdx >= 0)
					{
						//////////////////////////////////
						// get drivers name
						sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:{%d}UserName:", carIdx);
						if(parseYaml(irsdk_getSessionInfoStr(), str, &valstr, &valstrlen))
						{
							strncpy_s(nameStr, 512, valstr, valstrlen);
							nameStr[valstrlen] = '\0'; //driversname
						}//
						//////////////////////////////////
						// get drivers car path
						sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:{%d}CarPath:", carIdx);
						if(parseYaml(irsdk_getSessionInfoStr(), str, &valstr, &valstrlen))
						{
							strncpy_s(pathStr, 512, valstr, valstrlen);
							pathStr[valstrlen] = '\0'; //drivers car
						}//
						//////////////////////////////////
						// get drivers car number
						sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:{%d}CarNumber:", carIdx);
						if(parseYaml(irsdk_getSessionInfoStr(), str, &valstr, &valstrlen))
						{
							strncpy_s(numbStr, 512, valstr, valstrlen);
							numbStr[valstrlen] = '\0'; //drivers number
						}//
						//////////////////////////////////
					}////
					//////////////////////////////GET LOADED CAR AND DRIVER///////////////////////////////////

					printf ( "Driver: %s %s %s \n\n", numbStr ,nameStr, pathStr );

					//////////////////////////////GET LOADED CAR AND DRIVER///////////////////////////////////
					//where 
					// numbStr = drivers car number 
					// nameStr = drivers text full name
					// pathStr = car/chassis in use by driver.

					//
					//
					/////////////////////////////end process headers here
				}
				else if(data)
				{
					// process data here///////////
					// this is where you access and work with the telemitry vars, in 1/60th itterations.
					// see 'irsdk 1_0 data vars.csv' for specific ones your after.
					//

					////////////////////////////////////
					// fetch current gear value
					const char g_gearoffset[] = "Gear";
					int gearoffset = irsdk_varNameToOffset(g_gearoffset);				
					int CurrentGear = * ((int *)(data + gearoffset));
					/////////////////////////////////////
					//
					////////////////////////////////////
					// fetch current RPM value
					const char g_RPMoffset[] = "RPM";
					int RPMoffset = irsdk_varNameToOffset(g_RPMoffset);				
					float engRPM = * ((float *)(data + RPMoffset));
					/////////////////////////////////////

					//
					
					//
					//write data to Arduino
					char rpmString[2];
					int rpmSig = engRPM/1000;
					rpmSig = 2 * pow(rpmSig, 2);
					sprintf(rpmString, "%d", rpmSig);
					printf("RPM Current: %d Gear: %d \r", int(rpmSig), CurrentGear); //current rpm and gear
					SP->WriteData(rpmString,2);
					
					while (SP->ReadData(rpmString,1)==-1)
					{

					}

					//
					//
					/////////////////////////////// end pocess data here
				}
			}//no data being recieved.
		}
		//optional test, in case you need to close out a file...
		else if(!irsdk_isConnected())
		{
			// session ended
			if(data)
				delete[] data;
			delete SP;
			data = NULL;
		}
	}

	// call on exit to close memory mapped file
	irsdk_shutdown();

	return 0;
}
