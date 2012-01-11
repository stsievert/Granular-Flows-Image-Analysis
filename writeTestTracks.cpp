//
//  writeTestTracks.cpp
//  Granular Flows Image Analysis
//
//  Created by Scott Sievert on 7/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "writeTestTracks.h"
#include "calculateVelocities.h"
#include <fstream.h>
#include <math.h>
#include "apstring.h"
//#include "apvector.h"
#include <iomanip.h>
#include <iostream>
#include "writeTestTracks.h"



int writeTracks()
{
	apstring testTracks = "/Users/scottsievert/Desktop/school/college/2011 Summer/Research w: Dr. Hill, Summer 2011/test input&output/testTracksv2.txt";
	ofstream tracks(testTracks.c_str());
	//tracks.open(testTracks.c_str());
	//tracks << "Can you hear me?" << endl;
	
	double crossstreamPosition;
	int ID;
	double downstreamPosition;
	double increment;
	int length;
	
	ID = 0; increment = 0;
	for (crossstreamPosition = 1; crossstreamPosition <= 1000; crossstreamPosition = crossstreamPosition + 1) 
	{
		//ofstream tracks(testTracks.c_str());

		ID = ID +1;
		length = 0;
		for (downstreamPosition = 1; length < 100; length++) 
		{
			downstreamPosition = downstreamPosition + increment;
			
			tracks << crossstreamPosition <<"\t" <<downstreamPosition << "\t1"<< "\t1"<< "\t1"<< "\t1\t"<< ID << endl;
		}
		if (crossstreamPosition < 500) 
		{
			increment = increment + .01;
		}
		else
		{
			increment = increment - .01;
		}

	}
	tracks.close();
}