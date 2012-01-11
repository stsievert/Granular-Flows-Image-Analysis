//
//  widthFinder.cpp
//  Granular Flows Image Analysis
//
//  Created by Scott Sievert on 8/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
/*
                                                                   
                                                                   
                                                                   
				 1 1 1 1 1 1 1                                    
			   1               1                             
			 1   <–––– w ––––>   1                           
		   0                       0                          
		 0                           0                       
	   0                               0                    
 0 0 0                                   0 0 0 
 
 */



#include <fstream.h>
#include <iomanip.h>
#include <iostream>
#include <math.h>
//#include <time.h>
#include "apvector.h"
#include "apstring.h"
//#include "writeTestTracks.h"
#include "calculateVelocities.h"
#include <vector>
#include "widthFinder.h"

#define YES 1
#define NO 0

long double findXWhereYHappens(dataPoint left, dataPoint right, long double Y);

long double findWidth(apstring velocityPath, double percentageOfMax)
{
	using namespace std;
	ifstream input(velocityPath.c_str());
	dataPoint empty;
		empty.streamPosition.downstream_component = 0;
		empty.streamPosition.crossstream_component = 0;
		empty.streamVelocity.downstream_component = 0;
		empty.streamVelocity.crossstream_component = 0;



	vector<dataPoint> velocityProfile;
	dataPoint current;
	
	long double junk;
	long double maxVelocity=0;  // the maximum velocity (duh).
	int i;					 // Used as an index to arrays.
	long double leftSide;	// X / crossstream pos of leftSide.
	long double rightSide; // Ditto, but for rightSide.
	long double width;	  // Width of the flow (duh?)
	int calledBefore=0;
	int calledBeforeRight=0;
	
	char posStr[30];
	char veloStr[30];
	char junkStr[30];

	input.getline(junkStr, 30, ',');
	input.getline(junkStr, 30, ',');
	input.getline(junkStr, 30, ',');
	input.getline(junkStr, 30, ',');
	input.getline(junkStr, 30, '\n');
	// Remove column names.

	
	// Input. Put all of the stuff into an array (aka "vector").
	while (!input.eof()) 
	{
		//input >> current.streamPosition.crossstream_component >> junk
		//	  >> current.streamVelocity.downstream_component >> junk >> junk;
		
		input.getline(posStr, 30, ',');
			input.getline(junkStr, 30, ',');
		input.getline(veloStr, 30, ',');
			input.getline(junkStr, 30, ',');
			input.getline(junkStr, 30, '\n');

		current.streamPosition.crossstream_component = atof(posStr);
		current.streamVelocity.downstream_component = atof(veloStr);

		velocityProfile.push_back(current);
		
	}
	
	// Find the maximum velocity.
	for (i=0; i<velocityProfile.size(); i++) 
	{
		if (velocityProfile[i].streamVelocity.downstream_component > maxVelocity) 
		{
			maxVelocity = velocityProfile[i].streamVelocity.downstream_component;
		}
	}
	
	// See if each point is above the max velocity or not.
	//difference = velocityProfile[1].streamPosition.crossstream_component - velocityProfile[0].streamPosition.crossstream_component;
	
	for (i=0; i<velocityProfile.size(); i++) 
	{
		if (velocityProfile[i].streamVelocity.downstream_component > percentageOfMax * maxVelocity)
		{
			velocityProfile[i].aboveMinVelocity = YES;
		}
	}
	
	for (i=0; i<velocityProfile.size()-1; i++) 
	{
		if (velocityProfile[i].aboveMinVelocity==YES && velocityProfile[i+1].aboveMinVelocity == NO) 
		{
			if (calledBeforeRight) 
			{
				cout << "WidthFinder: The velocity profile is jagged. Widthfinder reports the rightmost low/high transitions." << endl;
			}
			calledBeforeRight = 1;
			
			rightSide = findXWhereYHappens(velocityProfile[i], velocityProfile[i+1], maxVelocity*percentageOfMax);
		}
		if (velocityProfile[i].aboveMinVelocity==NO && velocityProfile[i+1].aboveMinVelocity == YES) 
		{
			if (calledBefore) 
			{
				cout << "WidthFinder: The velocity profile is jagged. Widthfinder reports the rightmost low/high transitions." << endl;
			}
			calledBefore = 1;
			
			leftSide = findXWhereYHappens(velocityProfile[i], velocityProfile[i+1], maxVelocity*percentageOfMax);
		}
	}
	
	//velocityProfile.~vector();
	// Destruct vector.
	
	if (rightSide == -1 || leftSide==-1) 
	{
		return -1;
	}
	else
	{
		width = rightSide - leftSide;
		return width;
	}
	
}

long double findAvgVelocity(apstring velocityPath, double percentageOfMax)
{
	using namespace std;
	ifstream input(velocityPath.c_str());
	dataPoint empty;
	empty.streamPosition.downstream_component = 0;
	empty.streamPosition.crossstream_component = 0;
	empty.streamVelocity.downstream_component = 0;
	empty.streamVelocity.crossstream_component = 0;
	
	
	
	vector<dataPoint> velocityProfile;
	
	long double junk;
	long double maxVelocity=0;  // the maximum velocity (duh).
	int i;					 // Used as an index to arrays.
	double avgVelocity=0;
	int counter=0;
	char posStr[30];
	char veloStr[30];
	char junkStr[30];
	dataPoint current;
	
	
	input.getline(junkStr, 30, ',');
	input.getline(junkStr, 30, ',');
	input.getline(junkStr, 30, ',');
	input.getline(junkStr, 30, ',');
	input.getline(junkStr, 30, '\n');
	// Remove column names.
	// Input. Put all of the stuff into an array (aka "vector").
	while (!input.eof()) 
	{
		//input >> current.streamPosition.crossstream_component >> junk
		//	  >> current.streamVelocity.downstream_component >> junk >> junk;
		
		input.getline(posStr, 30, ',');
		input.getline(junkStr, 30, ',');
		input.getline(veloStr, 30, ',');
		input.getline(junkStr, 30, ',');
		input.getline(junkStr, 30, '\n');
		
		current.streamPosition.crossstream_component = atof(posStr);
		current.streamVelocity.downstream_component = atof(veloStr);
		
		velocityProfile.push_back(current);
		
	}
	
	// Find the maximum velocity.
	for (i=0; i<velocityProfile.size(); i++) 
	{
		if (velocityProfile[i].streamVelocity.downstream_component > maxVelocity) 
		{
			maxVelocity = velocityProfile[i].streamVelocity.downstream_component;
		}
	}
	
	avgVelocity = 0;
	counter = 0;
	for (i=0; i<velocityProfile.size(); i++) 
	{
		if (velocityProfile[i].streamVelocity.downstream_component > percentageOfMax*maxVelocity) 
		{
			avgVelocity = avgVelocity + velocityProfile[i].streamVelocity.downstream_component;
			counter = counter+1;
		}
	}
	avgVelocity = avgVelocity / counter;
	
	//velocityProfile.~vector();
	return avgVelocity;
	
	
}


long double findXWhereYHappens(dataPoint left, dataPoint right, long double Y)
{
	long double X;
	long double slope;
	
	slope = (right.streamVelocity.downstream_component - left.streamVelocity.downstream_component) 
			/ (right.streamPosition.crossstream_component - left.streamPosition.crossstream_component);
	
	X = (Y - left.streamVelocity.downstream_component) / slope; // ie, y = mx + b ––> x = (y-b)/m
	X = X + left.streamPosition.crossstream_component;
	if (X < left.streamPosition.crossstream_component || X > right.streamPosition.crossstream_component) 
	{
		cout << "––––– Error! –––––"<< endl << "Width finder didn't find Y in given X interval" << endl;
		return -1;
	}
	else
	{
		return X;
	}
}












