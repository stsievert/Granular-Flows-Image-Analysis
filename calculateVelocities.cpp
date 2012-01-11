

/*	
 
 Version 1.0:
 - outputs velocity profile.
 
 To be added in future versions:
 - calculate granular temperature (this may not work: the analysis software isn't accurate enough)
 - adjust for camera distortion, vertically. (horizontally isn't a problem)
 - output angle and position of banks.
 
 
 */	
/*
 
 Notes:
 This program finds the velocity profile of a flowing layer of sand/particles. It inputs the
 tracks.txt file from the IDL image tracking code. It corrects for tilted banks (w/ dot product).
 It calculates the velocity using a grid, and if any particles go through that, they contribute
 to the average velocity of that grid cell.
 
 I had to use some confusing notation. THere are streamVectors and vectors. streamVectors are
 vectors that are the particles position (from the top of the left bank) or velocity parallel
 and perpendicular to the banks of the flow, in real units. "vectors" are x and y pixel coordinates, 
 except for the two unitVectors. The unitVectors are 1 _unit_ long.
 
 For a more in-depth explanation of the whole program, and it's use, see the
 readme.txt file.
 

 
 
 */
/* 
 
 INPUT
 IDL tracks	
 column 1: x position
 column 2: y position
 column 3: ––
 column 4: ––
 column 5: –– 
 column 6: time
 column 7: ID
 
 
 // OUTPUT
 // velocity.csv (comma seperated values)
 //	column 1: distance downstream (units, from top of leftBank)
 //	column 2: distance crossstream (units, from side of leftBank)
 //	column 3: downstream velocity (units/s)
 //	column 4: crossstream velocity (units/s)
 // column 5: number of points in grid box
 
 
 */



#include <fstream.h>
#include <iomanip.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include "apvector.h"
#include "apstring.h"
#include "writeTestTracks.h"
#include "calculateVelocities.h"


#define YES 1
#define NO 0

#define MESSAGES 0
#define LESSMESSAGES 1

streamVector dotProduct(vector a, vector down, vector cross);
	// Dots vector "a" with down and cross to get streamPosition.
streamVector convertToUnits(pixelsPerUnit pixelsPerUnit, streamVector vector);
// Converts from pixels to real Units. TODO: delete. Double check before hand!
vector convertToUnitsV(pixelsPerUnit pixelsPerUnit, vector vector);
	// Converts vectors to units.
streamVector calculateStreamVelocities(streamVector position_current, streamVector position_previous, long double timestep);
	// Calculate stream velocities (in units/s)
streamVector calculateStreamPosition(long double x_current, long double y_current, line leftBank, vector unitVectorDown, vector unitVectorAcross, pixelsPerUnit pixelsPerUnit);
	// Calculates stream position. Calls dotproduct, convertToUnitsV. Accounts for disortion, tilted banks.
dataPoint* doubleAndCopyArray(int *capacityLimitPtr, dataPoint *array);
	// Doubles the size of the particleData array - dynamically allocates memory. TODO: delete?
dimensions findExtremes(int imgHeight, int imgWidth, line leftBank, vector unitVectorDown, vector unitVectorAcross, pixelsPerUnit pixelsPerUnit);
	// Finds streamPos of corners of image (to help init grid).
void wait (int seconds);
	// Wait x seconds. Used for some error messages.


int calculateVelocities(apstring config_filepath)
{
	// BEGIN INITIALIZATION	
	//
	//
	// VALUES TO CHANGE
	//
	//
	printf("\nInitializing...\n");
	
	ifstream configFile(config_filepath.c_str());
	char junkString[100];
	// Used to ignore the variable names in the file I/0 below.
	
	pixelsPerUnit pixelsPerUnit;
		configFile >> junkString >> pixelsPerUnit.top;
		configFile >> junkString >> pixelsPerUnit.bottom;
		configFile >> junkString >> pixelsPerUnit.horizontal;
	
	//TODO: CHANGE CONFIG FILES so they have pixelsPerUnit jazz.
	

	
	long double boxWidth = 0.5;
		configFile >> junkString >> boxWidth;
	// How wide do you want each box? In UNITS!
	int numberOfVerticalBoxes = 1;
		configFile >> junkString >> numberOfVerticalBoxes;
	
	int imgHeight;
		configFile >> junkString >> imgHeight;

	int imgWidth;
		configFile >> junkString >> imgWidth;
	pixelsPerUnit.height = imgHeight;


	// Should probably be pretty small. Better performance that way.
	
	long double timestep = (1.0/1000);
		configFile >> junkString >> timestep;

	// Time between each frame. Convienent to write as (1 second / (frames/second))
	
	int minPathLength = 5;
		configFile >> junkString >> minPathLength;

	// Minimum path length, to eliminate particles that are only tracked for a couple frames.
	// Should be ~5-10.
	
	streamVector maxVelocity;
		configFile >> junkString >> maxVelocity.crossstream_component;
		configFile >> junkString >> maxVelocity.downstream_component;
	// Maxiumum velocities (to protect against image tracking errors). In unit/s!
	// I would normally leave the downstream component unbounded (ie, use a big number), but make
	// the crossstream component pretty small.
	
	
	line leftBank;
		/*configFile >> junkString >> leftBank.start_x;
		configFile >> junkString >> leftBank.start_y;
		configFile >> junkString >> leftBank.end_x;
		configFile >> junkString >> leftBank.end_y;*/

	/*	
	 // The leftBank was orginally inputted by the user. Now, a more approriate name would be "flow direction."
	 */
	
	apstring inputpath; //= "/Users/scottsievert/Desktop/school/college/2011 Summer/Research w: Dr. Hill, Summer 2011/Data/6:24:11, exp1, 20min/";
	// Folder that contains the tracks.txt file (written by IDL)
	apstring tracksFilename; //= "2406110120min_tracks.txt";
	// the name of the tracks.txt file, for reading in.
	apstring fileidentifier; //= "2406110120min";
	// The thing that "identifier" that will be part of the filename. 
	// ie, the velocity profiles will be written as /.../.../identiefier_velocities.txt
	
	configFile >> junkString; configFile.get(); getline(configFile, inputpath);
	configFile >> junkString; configFile.get(); getline(configFile, tracksFilename);
	configFile >> junkString; configFile.get(); getline(configFile, fileidentifier);
	
	cout << "current file = " << fileidentifier << endl;
	


	
	//
	//
	// END OF VALUES TO CHANGE	
	//	
	//
	
	
	
	
	
	
	double temp;
	// This value is used in some places as a "holder" variable. 
	double junk;
	// This variable is used in some input/output routines as, well, junk.
	long i;
	int j;
	// This variable is used as mostly as an index to arrays, etc.
	
	
	apstring velocitypath = inputpath + fileidentifier + "_velocity.csv";
	apstring positionpath = inputpath + fileidentifier + "_particleData.txt";
    inputpath = inputpath + tracksFilename;
	// Input and output paths/filenames.
	
	ifstream input(inputpath.c_str());
	ofstream velocity(velocitypath.c_str());
	ifstream particleDataIN(positionpath.c_str());
	ofstream particleDataOUT(positionpath.c_str());
	
	
	// Opening files for input/output, using strings above.
	
	dataPoint currentParticle;
	dataPoint previousParticle;
	dataPoint buffer[minPathLength];
	int currentPathLength;
		// Returns an array with n elements. Doubles when reaches max.
												// Stream vectors = position from top, position from side of leftBank, in UNITS.
												// Vectors = straight up x&y image coordinates.
												// VelocityOutput = struct for holding information about path lengths are less than minPathLength.
	// Unit vector stuff
	// Unneeded: recalculates unit vector for each particle. (8/11/11). TODO: Double check before deleting.
	vector unitVectorDown;
	/*unitVectorDown.x_component= 1*(leftBank.end_x - leftBank.start_x);//+(rightBank.end_x - rightBank.start_x));
	unitVectorDown.y_component= 1*(leftBank.end_y - leftBank.start_y);//+(rightBank.end_y - rightBank.start_y));
		// Average the two banks and make them the x & y components of the unitVector.
		unitVectorDown = convertToUnitsV(pixelsPerUnit, unitVectorDown);
		// Convert the pixels to units.
	
		temp=unitVectorDown.x_component;
		unitVectorDown.x_component= unitVectorDown.x_component / sqrt(unitVectorDown.x_component*unitVectorDown.x_component + unitVectorDown.y_component*unitVectorDown.y_component);
		unitVectorDown.y_component= unitVectorDown.y_component / sqrt(temp*temp + unitVectorDown.y_component*unitVectorDown.y_component);
		// Make the unit vector 1 unit long.*/
	vector unitVectorAcross;
		/*unitVectorAcross.x_component= 1*unitVectorDown.y_component;
		unitVectorAcross.y_component= unitVectorDown.x_component;
		// Make a perpendicular-to-bank vector.*/
	
	// If the unit vector is straight...
	/*if ((leftBank.start_x - leftBank.end_x) ==0) 
	{
		// Make the unit vector 0. Protects against NaN (not a number).
		unitVectorDown.x_component = 0;
		unitVectorAcross.y_component = 0;
	}*/

	//int longerThanMinPath;
	// BOOL value (using #define YES, NO) to see if it the path is longer than minPathLength.
	
// INITIALIZING GRID.	
//streamVector dimensions;
//	streamVector dimensionsWithLeftBank;
	streamVector boxDimensions;
	gridCoords numberOfBoxes;
	dimensions extremes;
	// dimensions = dimensions in real units of flow, using banks. Used only for number of boxes, etc.
	// boxDimensions = size of each box, in real units.
	// numberOfBoxes = number of boxes in downstream/crossstream directions.
	/*
	// Calculating the number of boxes / box width, for the array initialization below.
	dimensions = calculateStreamPosition(rightBank.end_x, rightBank.end_y, leftBank, rightBank, unitVectorDown, unitVectorAcross, pixelsPerUnitTop);
		// Dots the furthest point downstream to get the dimensions of the flow (in units).
	dimensionsWithLeftBank = calculateStreamPosition(leftBank.end_x, leftBank.end_y, leftBank, rightBank, unitVectorDown, unitVectorAcross, pixelsPerUnitTop);
		// We have to find the dimensions using both of the bottom points in case the bank is tilted left OR right.
	if (dimensions.downstream_component < dimensionsWithLeftBank.downstream_component) 
		{dimensions.downstream_component = dimensionsWithLeftBank.downstream_component;}
	if (dimensions.crossstream_component < dimensionsWithLeftBank.crossstream_component) 
		{dimensions.crossstream_component = dimensionsWithLeftBank.crossstream_component;}
			// To make sure we have the furthest point downstream & crossstream we can.
	
	
	
	
	boxDimensions.crossstream_component = boxWidth;
	boxDimensions.downstream_component  = dimensions.downstream_component / numberOfVerticalBoxes;
		// These are the box dimensions: how wide each one is.
	
	
	numberOfBoxes.cross = dimensions.crossstream_component / boxWidth;
	numberOfBoxes.down = numberOfVerticalBoxes;
	boxDimensions.crossstream_component = boxWidth;
	boxDimensions.downstream_component = dimensions.crossstream_component / numberOfVerticalBoxes;
	// Getting the number of cross stream boxes and the width of the downstream boxes.
	
	
	if (numberOfBoxes.cross < 0) {numberOfBoxes.cross = -1*numberOfBoxes.cross;}
	if (boxDimensions.downstream_component < 0) {boxDimensions.downstream_component = -1*boxDimensions.downstream_component;}
	// Make sure they're not negative!
	*/
	
		
	// END INITIALIZATION	
	// START OF CALCULATIONS, INPUT OUTPUT.
	
	
	// This part finds the average direction of the flow, by finding the average displacement vector of all particles
	// that move.
	cout << "Finding average flow direction..." << endl;
	previousParticle.ID = -1;
	leftBank.slope = 0;
	leftBank.numberOfPoints = 0;
	leftBank.x=0;
	leftBank.y=0;
	if (!input.is_open()) 
	{
		cout << "–––––––Error!––––––"<< endl << "Couldn't open _tracks.txt file." << endl;
		cout << "Ending run in 15 seconds..." << endl;
		wait(15);
	}
	while (!input.eof()) 
	{
		input	>> currentParticle.position.x_component 
				>> currentParticle.position.y_component
				>> junk >> junk >> junk
				>> currentParticle.time
				>> currentParticle.ID;
		
		// If the current particle is the same and moved at least a pixel, use it in the calculation.
		// TODO: check for under speed limit?
		if (currentParticle.ID == previousParticle.ID) 
		{
			// Sums essentially, this adds the *total* change in x & y to 
			leftBank.x = leftBank.x + currentParticle.position.x_component - previousParticle.position.x_component;
			leftBank.y = leftBank.y + currentParticle.position.y_component - previousParticle.position.y_component;

			// This is unneeded. TODO: Delete.
			leftBank.numberOfPoints++;
		}
		if ((currentParticle.ID != previousParticle.ID) && ((int)currentParticle.ID % 50000 == 0)) 
		{
			cout <<"\tID: " <<currentParticle.ID/1000 << "k" << endl;
			//cout << "\t (x,y) : " << currentParticle.position.x_component << " , " << currentParticle.position.y_component << endl;
		}
		
		
		previousParticle = currentParticle;
	
	}
	
	leftBank.slope = leftBank.x/leftBank.y;
	leftBank.start_x = 0;
	leftBank.start_y = 0;
	leftBank.end_y = imgHeight;
	leftBank.end_x = leftBank.slope * imgHeight;
	input.close();
	// leftBank now points in the average direction of the flow. It's also on the far left of the image.
	
	// Now that we have leftBank, we can find how many boxes we need.
	extremes = findExtremes(imgHeight, imgWidth, leftBank, unitVectorDown, unitVectorAcross, pixelsPerUnit);
	// Find the streamPosition of all of the corners, and put them into the "min" and "max" stream vectors. max.cross = maximum crossstream position. min.down = minimum downstream position.
	numberOfBoxes.cross = 1+(extremes.max.crossstream_component - extremes.min.crossstream_component) / boxWidth;
	numberOfBoxes.down = numberOfVerticalBoxes;
	//cout << "numberOfBoxes.cross = " << numberOfBoxes.cross << endl;
	boxDimensions.crossstream_component = boxWidth;
	boxDimensions.downstream_component = (extremes.max.downstream_component - extremes.min.downstream_component)/numberOfBoxes.down;
	
	cell grid[numberOfBoxes.cross][numberOfBoxes.down];
	gridCoords currentBox;
	/*// [horiz. box][vert. box]
	 // Each element in the array is a struct "cell" which contains holds avg velocity, gran. temp, # of points, and center position.
	 // gridCords is just a struct with two indexes to an array.*/
	
	//particleDataOUT.open(positionpath.c_str());
	input.open(inputpath.c_str());
	//Reopen the file.
	// INITIALIZATION FOR THE WHILE LOOP BELOW	
	printf("Inputting tracks and finding positions for each particle...\n");
	previousParticle.ID = -1; // to make sure that it doesn't try to calculate a nonsense velocity.
	
	// Input, calculate velocities, etc...
	while (!input.eof()) 
	{
		/*
		// This while loop does the following:
		// 1. inputs the x&y pixel coords.
		// 2. calculates the stream position (dot product w/ unit vector)
		// 3. calculates the velocity vector, if it's the same particle
		// 4. writes the results to a file.
		 //
		 //
		 //
		 //
		//
		// The file format is this:
		//	column:
		//	1. dowstream position
		//	2. crossstraem position
		//	3. downstraem velocity
		//	4. crossstream velocity
		//	5. time
		//	6. ID
		*/
		// Input from file to variables.
		if (input.eof()) {break;}
		
		input	>> currentParticle.position.x_component 
				>> currentParticle.position.y_component
				>> junk >> junk >> junk
				>> currentParticle.time
				>> currentParticle.ID;
		
		if ((LESSMESSAGES == 1) && ((int)currentParticle.ID%50000 == 0) && (currentParticle.ID !=previousParticle.ID)) 
		{cout <<"\t" <<"ID = " << currentParticle.ID/1000<<"k" << endl;}
		
		// Calculate the position no matter what.
		currentParticle.streamPosition = calculateStreamPosition(currentParticle.position.x_component, currentParticle.position.y_component, leftBank, unitVectorDown, unitVectorAcross, pixelsPerUnit);
	
		if (currentParticle.ID != previousParticle.ID) 
		{
			currentPathLength=0; 
			previousParticle = currentParticle; 
			continue;
		}
		else {currentPathLength++;}
		
		// If it's the same particle
		if (currentParticle.ID == previousParticle.ID) 
		{
			//calculate the velocity.
			currentParticle.streamVelocity.downstream_component = (currentParticle.streamPosition.downstream_component - previousParticle.streamPosition.downstream_component)/timestep;
			currentParticle.streamVelocity.crossstream_component = (currentParticle.streamPosition.crossstream_component- previousParticle.streamPosition.crossstream_component)/timestep;
		}
		
		// If the current path isn't ready to be printed and it has a velocity (/same particle as last time)
		if ((currentPathLength < minPathLength) && currentParticle.ID == previousParticle.ID)
		{
			// Store it in the buffer.
			/*	I put this code because the buffer[...] = currentParticle was misbehaving. Now, it's working.
			buffer[currentPathLength-1].position.x_component = currentParticle.position.x_component;
			buffer[currentPathLength-1].position.y_component = currentParticle.position.y_component;

			buffer[currentPathLength-1].streamVelocity.downstream_component = currentParticle.streamVelocity.downstream_component;
			buffer[currentPathLength-1].streamVelocity.crossstream_component = currentParticle.streamVelocity.crossstream_component;

			//buffer[currentPathLength-1].streamPosition.downstream_component = currentParticle.streamPosition.downstream_component;
			//buffer[currentPathLength-1].streamPosition.crossstream_component = currentParticle.streamPosition.crossstream_component;
			buffer[currentPathLength-1].streamPosition = currentParticle.streamPosition;
			
			buffer[currentPathLength-1].ID = currentParticle.ID;
			buffer[currentPathLength-1].time = currentParticle.time;
		 */	
			buffer[currentPathLength-1] = currentParticle;
			
			/*cout << buffer[currentPathLength-1].streamPosition.downstream_component	<< "\t"
			<< buffer[currentPathLength-1].streamPosition.crossstream_component	<< "\t"
			<< buffer[currentPathLength-1].streamVelocity.downstream_component	<< "\t"
			<< buffer[currentPathLength-1].streamVelocity.crossstream_component	<< "\t"
			<< buffer[currentPathLength-1].time									<< "\t"
			<< buffer[currentPathLength-1].ID << endl;*/

			
		}
		
		// If the current path is ready to be printed...
		if (currentPathLength == minPathLength)
		{
			for (i=0; i<minPathLength-1; i++) 
			{
				// if the current element is under the speed limit,
				if (buffer[i].streamVelocity.downstream_component < maxVelocity.downstream_component
					&& buffer[i].streamVelocity.crossstream_component < maxVelocity.crossstream_component) 
				{
					// print it out.
					
					particleDataOUT << buffer[i].streamPosition.downstream_component	<< "\t"
									<< buffer[i].streamPosition.crossstream_component	<< "\t"
									<< buffer[i].streamVelocity.downstream_component	<< "\t"
									<< buffer[i].streamVelocity.crossstream_component	<< "\t"
									<< buffer[i].time									<< "\t"
									<< buffer[i].ID << endl;
					/*cout << buffer[i].streamPosition.downstream_component	<< endl
					<< buffer[i].streamPosition.crossstream_component	<< endl
					<< buffer[i].streamVelocity.downstream_component	<< endl
					<< buffer[i].streamVelocity.crossstream_component	<< endl
					<< buffer[i].time									<< endl
					<< buffer[i].ID << endl;*/
				}
			}
		}
		
		// If it's under the speed limit AND longer than the min path length,
		if (currentPathLength > minPathLength
			&& currentParticle.streamVelocity.downstream_component < maxVelocity.downstream_component
			&& currentParticle.streamVelocity.crossstream_component < maxVelocity.crossstream_component) 
		{
			// print it out.
			particleDataOUT << currentParticle.streamPosition.downstream_component		<< "\t"
							<< currentParticle.streamPosition.crossstream_component	<< "\t"
							<< currentParticle.streamVelocity.downstream_component		<< "\t"
							<< currentParticle.streamVelocity.crossstream_component	<< "\t"
							<< currentParticle.time									<< "\t"
							<< currentParticle.ID << endl;
		}

		
		// "Increment" the particle.
		previousParticle = currentParticle;
	
	}
	
	particleDataOUT.close();
		// The file /.../.../fileidentifier_particleData.txt now has all of the streamPositions+velocities+ID+time for all particles.	
		/*	
 // unneeded: implemented above, with file I/0, not array.
	while (!particleDataIN.eof()) 
	{
		// If it's a new particle AND is longer than the minPathLength
		if (particleData[i-1].ID != particleData[i].ID) 
		{
			longerThanMinPath = NO;
			if ((particleData[i].ID == particleData[i+minPathLength].ID)) 
			{
				// Say it's longer than the minPathLength
				longerThanMinPath = YES;
			}
		}
		
		// If it's the same particle and is longer than the minPathLength
		if ((particleData[i-1].ID == particleData[i].ID) && longerThanMinPath==YES)
		{
			// Calculate velocity.
			particleData[i].streamVelocity = calculateStreamVelocities(particleData[i].streamPosition, particleData[i-1].streamPosition, timestep);
			
			if (MESSAGES == 1) 
			{
				cout
				<<"down pos = " << particleData[i].streamPosition.downstream_component  << " "
				<<"\tcross pos = " << particleData[i].streamPosition.crossstream_component << " "
				<<"\tdown vel = " << particleData[i].streamVelocity.downstream_component  << " "
				<<"\tcross vel = " << particleData[i].streamVelocity.crossstream_component << " "
				<<"\tID vel = " << particleData[i].ID << endl;
			}
		}
	}
	*/
	
	printf("Calculating velocity profiles...\n");
	
	
	// Setting up grid...
	// Set up the coordinates for the left top edge/center of each box.
	// Initialization.
	for (j=0; j< numberOfBoxes.down; j++) 
	{
		i=0;
		grid[i][j].leftEdge.downstream_component  = grid[i][j-1].leftEdge.downstream_component  + boxDimensions.downstream_component;
		
		for (i=0; i<numberOfBoxes.cross; i++) 
		{
			if (i==0 && j==0) 
			{
				// First case/initialization.
				grid[0][0].leftEdge.downstream_component =  extremes.min.downstream_component;
				grid[0][0].leftEdge.crossstream_component = extremes.min.crossstream_component;
				continue;
			}
			grid[i][j].leftEdge.crossstream_component = grid[i-1][j].leftEdge.crossstream_component + boxDimensions.crossstream_component;
			grid[i][j].leftEdge.downstream_component  = grid[0][j].leftEdge.downstream_component;
			grid[i][j].numberOfPoints = 0;
			grid[i][j].avgDownstreamVelocity = 0;
			grid[i][j].avgCrossstreamVelocity = 0;
			//cout << "grid["<<i <<"]["<<j<<"].leftEdge.cross = " << grid[i][j].leftEdge.crossstream_component << endl;
			
		}
	}
	
	// Calculates the velocities in each gridbox (grid is "finer" crossstream than downstream").
	currentBox.down = 0;
	currentParticle.ID = 0; currentParticle.time = 0; currentParticle.streamPosition.downstream_component = 0; currentParticle.streamPosition.crossstream_component = 0; currentParticle.streamVelocity.downstream_component=0; currentParticle.streamVelocity.crossstream_component=0;

	if (!particleDataIN.is_open()) 
	{
		particleDataIN.open(positionpath.c_str());
		if (!particleDataIN.is_open()) 
		{
			cout << "–––––– Error! ––––––" << endl;
			cout <<"Couldn't open _particleData.txt. Ending current run in 30 seconds."<<endl;
			wait(30);
			return -1;
		}
	}
	long counter=0; // just a little counter to see how many points there are.
	while (!particleDataIN.eof()) 
	{
		counter++;
		if (counter % 1000000 == 0) {cout << "\tpoints = " << counter/1000000 <<"mil" << endl;}
		 particleDataIN >> currentParticle.streamPosition.downstream_component
						>> currentParticle.streamPosition.crossstream_component
						>> currentParticle.streamVelocity.downstream_component
						>> currentParticle.streamVelocity.crossstream_component
						>> currentParticle.time
						>> currentParticle.ID;
		if(particleDataIN.bad()) {cout << "–––––INPUT ERROR–––––" << endl;}
		if (particleDataIN.eof()) {break;}
		
		// This part finds the current box. I'll have to modify it, to make it
		// faster. When there's 110 boxes & 7,000 particles, it takes a long time.
		
		//Make a guess on the currentBox.
		currentBox.down = currentParticle.streamPosition.downstream_component / boxDimensions.downstream_component;
		currentBox.cross = currentParticle.streamPosition.crossstream_component / boxDimensions.crossstream_component;
		
		// If the particle is in the right box, don't do the rest.
		if ((currentParticle.streamPosition.crossstream_component >= grid[currentBox.cross][currentBox.down].leftEdge.crossstream_component) 
			&& (currentParticle.streamPosition.crossstream_component < grid[currentBox.cross+1][currentBox.down].leftEdge.crossstream_component)  )
		{
			// Do nothing....
		}
		// If the right box is further crossstream, go that way.
		else if (currentParticle.streamPosition.crossstream_component >= grid[currentBox.cross][currentBox.down].leftEdge.crossstream_component + boxDimensions.crossstream_component)
		{
			// Walk the box further crossstream
			for (currentBox.cross = currentBox.cross; currentBox.cross < numberOfBoxes.cross-1; currentBox.cross++) 
			{
				// If this is the right cross stream box...
				if ((currentParticle.streamPosition.crossstream_component >= grid[currentBox.cross][currentBox.down].leftEdge.crossstream_component) 
					&& (currentParticle.streamPosition.crossstream_component < grid[currentBox.cross][currentBox.down].leftEdge.crossstream_component) +boxDimensions.crossstream_component )
				{
					// Stop looking
					break;
				}
			}
		}
		// If the right box is NOT further crossstream, go that way.
		else if (currentParticle.streamPosition.crossstream_component < grid[currentBox.cross][currentBox.down].leftEdge.crossstream_component)
		{
			// Walk the box further NOT crossstream
			for (currentBox.cross = currentBox.cross; currentBox.cross >= 0; currentBox.cross--) 
			{
				// If this is the right cross stream box...
				if ((currentParticle.streamPosition.crossstream_component >= grid[currentBox.cross][currentBox.down].leftEdge.crossstream_component) 
					&& (currentParticle.streamPosition.crossstream_component < grid[currentBox.cross][currentBox.down].leftEdge.crossstream_component) +boxDimensions.crossstream_component )
				{
					// Stop looking
					break;
				}
			}
		}
		

		// Finds the downstream box/coord (brute force method, no guesses)
		for (currentBox.down = 0; currentBox.down < numberOfBoxes.down; currentBox.down++) 
		{
			// If it's in the right box or in the last box...
			if (currentBox.down == numberOfBoxes.down-1) 
			{
				break;
			}
			else if ((currentParticle.streamPosition.downstream_component >=  grid[currentBox.cross][currentBox.down].leftEdge.downstream_component ) 
					 && (currentParticle.streamPosition.downstream_component <grid[currentBox.cross][currentBox.down+1].leftEdge.downstream_component))
			{
				// Stop counting.
				break;
			}
		}
		
		
		// Update the avg. velocity.
		// This part only sums up the avg. velocities for better accuracy. They're divided in the for loop below (the one that prints all of the data).
		// If it's "under the speed limit..."
		if (currentParticle.streamVelocity.downstream_component < maxVelocity.downstream_component
			&& fabs(currentParticle.streamVelocity.crossstream_component) < maxVelocity.crossstream_component ) 
		{
			// Update the velocity.
			grid[currentBox.cross][currentBox.down].numberOfPoints++;
			grid[currentBox.cross][currentBox.down].avgDownstreamVelocity =  grid[currentBox.cross][currentBox.down].avgDownstreamVelocity +  currentParticle.streamVelocity.downstream_component;
			grid[currentBox.cross][currentBox.down].avgCrossstreamVelocity = grid[currentBox.cross][currentBox.down].avgCrossstreamVelocity + currentParticle.streamVelocity.crossstream_component;

			//cout << grid[currentBox.cross][currentBox.down].leftEdge.crossstream_component << "\t"
			//<< currentParticle.streamPosition.crossstream_component << endl;
		}	
	}
	
	velocity << "crossStream pos," << "downStream pos," << "avg downstream velo," << "avg crossstream velo," << endl;
	for (j=0; j<numberOfBoxes.down; j++) 
	{
		for (i=0; i < numberOfBoxes.cross; i++) 
		{
			grid[i][0].avgDownstreamVelocity = grid[i][j].avgDownstreamVelocity / grid[i][j].numberOfPoints;
			grid[i][0].avgCrossstreamVelocity = grid[i][j].avgCrossstreamVelocity / grid[i][j].numberOfPoints;
			
			velocity<< grid[i][j].leftEdge.crossstream_component << ","
					<< grid[i][j].leftEdge.downstream_component << ","
					<< grid[i][j].avgDownstreamVelocity << ","
					<< grid[i][j].avgCrossstreamVelocity << ","
					<< grid[i][j].numberOfPoints << endl;
		}
	}
	
	return 42;
/* DON'T PANIC */
}


streamVector dotProduct(vector a, vector down, vector cross)
{
	// Does a dot product to get from image coords to stream/flow coords.
	/* INPUTS:
	 vector a : position or velocity vector that will be transformed from x&y to flow/stream coords.
	 vector down : unit vector in the downstream direcetion.
	 vector cross : unit vector in the crossstream direction.
	 OUTPUTS:
	 streamVector result : vector transformed from image (x and y) coords to stream coords (distance downstream, distance from leftBank).
	 */
	
	streamVector result;
	
	result.downstream_component = a.x_component*down.x_component + a.y_component*down.y_component;
	result.crossstream_component = a.x_component*cross.x_component + a.y_component*cross.y_component;
	return result;
}

streamVector convertToUnits(pixelsPerUnit pixelsPerUnit, streamVector vector)
{
	// Transforms a streamVector from pixels to units.
	
	vector.downstream_component = vector.downstream_component / pixelsPerUnit.top;
	vector.crossstream_component = vector.crossstream_component / pixelsPerUnit.top;
	return vector;
}



streamVector calculateStreamVelocities(streamVector position_current, streamVector position_previous, long double timestep)
{
	// Calculates velocities.
	streamVector velocity;
	velocity.downstream_component = (position_current.downstream_component - position_previous.downstream_component) / timestep;
	velocity.crossstream_component = (position_current.crossstream_component - position_previous.crossstream_component) / timestep;
	return velocity;
	
}

streamVector calculateStreamPosition(long double x_current, long double y_current, line leftBank, vector unitVectorDown, vector unitVectorAcross, pixelsPerUnit pixelsPerUnit)
{
	// Calculates position, by dotting the two unit vectors (parallel & perpendicular to flow) with position vector (converted into units, of course).
	vector particle_position;
	streamVector particle_position_corrected;
	vector bankPoint;
	
	particle_position.x_component = x_current - leftBank.start_x;
	particle_position.y_component = y_current - leftBank.start_y;
	
	particle_position = convertToUnitsV(pixelsPerUnit, particle_position);

	
	/*
	unitVectorDown.x_component= 0.5*((leftBank.end_x - leftBank.start_x)+(rightBank.end_x - rightBank.start_x));
	unitVectorDown.y_component= 0.5*((leftBank.end_y - leftBank.start_y)+(rightBank.end_y - rightBank.start_y));
	unitVectorDown = convertToUnitsV(pixelsPerUnit, unitVectorDown);
	*/
	
	// I have some y value. I want to find the x value on the bank. This does that.
	bankPoint.x_component = ((leftBank.end_x - leftBank.start_x)/(leftBank.end_y - leftBank.start_y))*y_current-leftBank.start_x;
	bankPoint.y_component = y_current-leftBank.start_y;
	if (y_current==0)
	{
		bankPoint.x_component = leftBank.start_x;
	}
	
	
	bankPoint = convertToUnitsV(pixelsPerUnit, bankPoint);
	
	
	unitVectorDown.x_component = bankPoint.x_component / sqrt(bankPoint.x_component*bankPoint.x_component + bankPoint.y_component*bankPoint.y_component);
	unitVectorDown.y_component = (unitVectorDown.x_component*bankPoint.y_component)/bankPoint.x_component;
	// That second line there saves us from doing another sqrt. It's equivalent to the first line (x replacing y on the top though).
	//  This is a unitVector that is in the vicinity of the point to account for distortion.
	
	if (bankPoint.x_component == 0) 
	{
		unitVectorDown.x_component = leftBank.end_x / sqrt(leftBank.end_x*leftBank.end_x + leftBank.end_y*leftBank.end_y);
		unitVectorDown.y_component = (unitVectorDown.x_component*leftBank.end_y)/leftBank.end_x;
	}
	// More NaN protection. It isn't a 100% accurate unitVect, but it's only used when findExtremes() is called from main().

	unitVectorAcross.x_component= unitVectorDown.y_component;
	unitVectorAcross.y_component= unitVectorDown.x_component;
	// Make a perpendicular-to-bank vector.
	
	// If the unit vector is straight...
	if ((leftBank.start_x - leftBank.end_x) ==0) 
	{// Make the unit vector 0. Protects against NaN (not a number).
		unitVectorDown.x_component = 0; unitVectorAcross.y_component = 0;}
	
	particle_position_corrected = dotProduct(particle_position, unitVectorDown, unitVectorAcross);
	

	
	return particle_position_corrected;
}

vector convertToUnitsV(pixelsPerUnit pixelsPerUnit, vector vector)
{
	// Transforms a vector from pixels to units.
	vector.x_component = vector.x_component / pixelsPerUnit.horizontal;
	vector.y_component = 0.5*(((1/pixelsPerUnit.bottom)-(1/pixelsPerUnit.top))/pixelsPerUnit.height)*vector.y_component*vector.y_component + vector.y_component*(1/pixelsPerUnit.bottom);
	// The line above accounts for disortion. It looks complicated, but it's just an integral of a straight line. Accounts for camera distortion, and assumes that the
	// distortion is linear (ie, small). This is a good assumption: I measured it myself. Top to bottom, I found a variance of less than 5%. If you want to account for
	// some other, crazier disortion (ie, not linear), you'll have to do a different integral / make it more complicated with assuming it's linear over small regions, then
	// integrating over those regions.
	return vector;
}

dataPoint* doubleAndCopyArray(int *capacityLimitPtr, dataPoint* array)
{
	// Doubles the size of an array, for a dynamical-length array.
	int i;
	int *ptr = capacityLimitPtr;
	
	dataPoint *newArray;
	newArray = new dataPoint [*ptr *2];
	for (i=0; i< *ptr; i++) 
	{
		newArray[i] = array[i];
	}
	
	delete array;
	
	*ptr = *ptr *2;
	if (*ptr <= 128) 
	{
		float arraySize = (float)(*ptr *8);
		cout << "\tarray = " << arraySize << " bytes\n";
	}
	if (*ptr <= 1024*64 && *ptr > 128) 
	{
		float arraySize = (float)(*ptr *8)/(1024);
		cout << "\tarray = " << arraySize << " kB\n";
	}
	if (*ptr > 1024*64) 
	{
		float arraySize = (float)(*ptr *8)/(1024*1024);
		cout << "\tarray = " << arraySize << " MB\n";
	}
	
	array = new dataPoint [*ptr];
	
	for (i=0; i<*ptr; i++) 
	{
		array[i] = newArray[i];
	}
	
	delete newArray;
	
	return array;

}


dimensions findExtremes(int imgHeight, int imgWidth, line leftBank, vector unitVectorDown, vector unitVectorAcross, pixelsPerUnit pixelsPerUnit)
{
	dimensions extremes;
	extremes.min = calculateStreamPosition(imgWidth/2, imgHeight/2, leftBank, unitVectorDown, unitVectorAcross, pixelsPerUnit);
	extremes.max = calculateStreamPosition(imgWidth/2, imgHeight/2, leftBank, unitVectorDown, unitVectorAcross, pixelsPerUnit);



	// This part calculates the stream position of all four corners, and replaces the extremes if the current is "more extreme" than the high/low (end/start, respectively).
	extremes.current = calculateStreamPosition(0, 0, leftBank, unitVectorDown, unitVectorAcross, pixelsPerUnit);
		if (extremes.current.downstream_component > extremes.max.downstream_component) {extremes.max.downstream_component = extremes.current.downstream_component;}
		if (extremes.current.crossstream_component > extremes.max.crossstream_component) {extremes.max.crossstream_component = extremes.current.crossstream_component;}
		if (extremes.current.downstream_component < extremes.min.downstream_component) {extremes.min.downstream_component = extremes.current.downstream_component;}
		if (extremes.current.crossstream_component < extremes.min.crossstream_component) {extremes.min.crossstream_component = extremes.current.crossstream_component;}

	extremes.current = calculateStreamPosition(imgWidth, 0, leftBank, unitVectorDown, unitVectorAcross, pixelsPerUnit);
		if (extremes.current.downstream_component > extremes.max.downstream_component) {extremes.max.downstream_component = extremes.current.downstream_component;}
		if (extremes.current.crossstream_component > extremes.max.crossstream_component) {extremes.max.crossstream_component = extremes.current.crossstream_component;}
		if (extremes.current.downstream_component < extremes.min.downstream_component) {extremes.min.downstream_component = extremes.current.downstream_component;}
		if (extremes.current.crossstream_component < extremes.min.crossstream_component) {extremes.min.crossstream_component = extremes.current.crossstream_component;}

	extremes.current = calculateStreamPosition(0, imgHeight, leftBank, unitVectorDown, unitVectorAcross, pixelsPerUnit);
		if (extremes.current.downstream_component > extremes.max.downstream_component) {extremes.max.downstream_component = extremes.current.downstream_component;}
		if (extremes.current.crossstream_component > extremes.max.crossstream_component) {extremes.max.crossstream_component = extremes.current.crossstream_component;}
		if (extremes.current.downstream_component < extremes.min.downstream_component) {extremes.min.downstream_component = extremes.current.downstream_component;}
		if (extremes.current.crossstream_component < extremes.min.crossstream_component) {extremes.min.crossstream_component = extremes.current.crossstream_component;}

	extremes.current = calculateStreamPosition(imgWidth, imgHeight, leftBank, unitVectorDown, unitVectorAcross, pixelsPerUnit);
		if (extremes.current.downstream_component > extremes.max.downstream_component) {extremes.max.downstream_component = extremes.current.downstream_component;}
		if (extremes.current.crossstream_component > extremes.max.crossstream_component) {extremes.max.crossstream_component = extremes.current.crossstream_component;}
		if (extremes.current.downstream_component < extremes.min.downstream_component) {extremes.min.downstream_component = extremes.current.downstream_component;}
		if (extremes.current.crossstream_component < extremes.min.crossstream_component) {extremes.min.crossstream_component = extremes.current.crossstream_component;}

	return extremes;
	
}

void wait ( int seconds )
{
	clock_t endwait;
	endwait = clock () + seconds * CLOCKS_PER_SEC ;
	while (clock() < endwait) {}
}








