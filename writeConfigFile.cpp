//
//  writeConfigFile.cpp
//  Granular Flows Image Analysis
//
//  Created by Scott Sievert on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "writeConfigFile.h"
#include "apstring.h"
#include <fstream.h>
#include "calculateVelocities.h"

int writeConfigFile(apstring configPath, apstring inputpath, apstring tracksFilename, apstring fileidentifier)
{
	ofstream config(configPath.c_str());
	pixelsPerUnit pixelsPerUnit;

	cout << endl <<"Don't worry about screwing up the input. You can always change it later, by editing the text file." << endl;
	cout << endl << "Enter pixelsPerUnit at the top of the image, measured vertically." << endl;
	cin>> pixelsPerUnit.top;
	cout << "Enter pixelsPerUnit at the bottom, measured vertically." << endl;
	cin>> pixelsPerUnit.bottom;
	cout << "Enter pixelsPerUnit, measured horizontally." << endl;
	cin >> pixelsPerUnit.horizontal;

	config << "pixelsPerUnit.top= " << pixelsPerUnit.top << endl;
	config << "pixelsPerUnit.bottom= " << pixelsPerUnit.bottom << endl;
	config << "pixelsPerUnit.horizontal= " << pixelsPerUnit.horizontal << endl;

	
	long double boxWidth;
	cout << "Enter the horizontal box width, in UNITS!" << endl;
	cin >> boxWidth;
	// How wide do you want each box? In UNITS!
	int numberOfVerticalBoxes = 1;
	cout << "How many vertical boxes do you want? (1 = reccomended)" << endl;
	cin >> numberOfVerticalBoxes;
	// Should probably be pretty small. Better performance that way.
	config << "boxWidth= " << boxWidth << endl;
	config << "numberOfVerticalBoxes= " << numberOfVerticalBoxes << endl;

	int imgHeight;
	int imgWidth;
	cout<< "imgHeight? : ";
	cin >> imgHeight;
	cout<< "imgWidth? : ";
	cin>> imgWidth;
	config << "imgHeigth= " << imgHeight << endl;
	config << "imgWidth= " << imgWidth << endl;

	
	
	long double timestep = (1.0/1000);
	cout << "Timestep? : ";
	cin >> timestep;
	// Time between each frame. Convienent to write as (1 second / (frames/second))
	config << "timestep= " << timestep << endl;

	
	int minPathLength = 5;
	cout << "Minumum path length? : ";
	cin >> minPathLength; 
	// Minimum path length, to eliminate particles that are only tracked for a couple frames.
	// Should be ~5-10.
	config << "minPathLength= " << minPathLength << endl;

	
	
	streamVector maxVelocity;
	maxVelocity.crossstream_component = 1.0;
	maxVelocity.downstream_component = 1000;
	cout << "Max crossstream velocity? (~1 reccommended) : ";
	cin >> maxVelocity.crossstream_component;
	
	cout << "Max downstream velocity? (~100 reccommended) : ";
	cin >> maxVelocity.downstream_component;
	// Maxiumum velocities (to protect against image tracking errors). In unit/s!
	// I would normally leave the downstream component unbounded (ie, use a big number), but make
	// the crossstream component pretty small.
	config << "maxVelocity.crossstream_component= " << maxVelocity.crossstream_component << endl;
	config << "maxVelocity.downstream_component= " << maxVelocity.downstream_component << endl;
	
	cout << endl <<"Your file has been written at " << configPath << endl;
	
	
	
	/*line leftBank;
	leftBank.start_x= 176-100;
	leftBank.start_y= 0;
	leftBank.end_x = 177-100;
	leftBank.end_y = 1198;
		config << "leftBank.start_x= " << leftBank.start_x << endl;
		config << "leftBank.start_y= " << leftBank.start_y << endl;
		config << "leftBank.end_x= " << leftBank.end_x << endl;
		config << "leftBank.end_y= " << leftBank.end_y << endl;

	
	line rightBank;
	rightBank.start_x= 1618+100;
	rightBank.start_y= 0;
	rightBank.end_x = 1619+100;
	rightBank.end_y = 1198;
		config << "rightBank.start_x= " << rightBank.start_x << endl;
		config << "rightBank.start_y= " << rightBank.start_y << endl;
		config << "rightBank.end_x= " << rightBank.end_x << endl;
		config << "rightBank.end_y= " << rightBank.end_y << endl;*/
	// "banks" are now found automatically! Yay!
	
	/*	
	 // The start and end points of each bank. (0,0) = top left corner. It's probably a good idea to 
	 add a horizontal offset, to make the flow wider than it actually is - just in case.
	 */
	//apstring inputpath = "/Users/scottsievert/Desktop/school/college/2011_Summer/granular_flow_research/Data/tracks_position_files/";
	// Folder that contains the tracks.txt file (written by IDL)
	//apstring tracksFilename = "2806110167min_tracks.txt";
	// the name of the tracks.txt file, for reading in.
	//apstring fileidentifier = "2806110167min";
		config << "inputpath= " << inputpath.c_str() << endl;
		config << "tracksFilename= " << tracksFilename.c_str() << endl;
		config << "fileidentifier= " << fileidentifier.c_str() << endl;
	
	// The thing that "identifier" that will be part of the filename. 
	// ie, the velocity profiles will be written as /.../.../identiefier_velocities.txt
	//
	//
	// END OF VALUES TO CHANGE	
	//	
	//

	return 42;
/* DON'T PANIC */
	

}