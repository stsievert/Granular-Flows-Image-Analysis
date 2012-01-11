//
//  showPoints.cpp
//  Granular Flows Image Analysis
//
//  Created by Scott Sievert on 8/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <fstream.h>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "apstring.h"

void showPointsFrom_positions(apstring positionPath, apstring imgDirectory, apstring fileName, int imgHeight, int imgWidth, int frames)
{
	// Writes an image from some tracks path where x & y coords are in the first
	// two columns, respectively. Outputs a .pgm image.
	
	// tracksPath = tracks file you want to make a movie from.
	
	char outputPath[1000];
	char imgDirectoryAndFilename[1000];
		strcpy(imgDirectoryAndFilename, imgDirectory.c_str());
		strcat(imgDirectoryAndFilename, fileName.c_str());
	char number[10];
	
	uint8_t pic[imgWidth][imgHeight][frames];	// uint8_t holds values from 0 to 255. Cuts down on memory, hopefully.
	int maxBright = 15;
	int pointBright = 11;
	
	float x, y; // Used for input, output.
	float junk;
	float time;
	
	ifstream input(positionPath.c_str());

	for (int t=0; t<frames; t++) 
	{	
		for (int j=0; j<imgHeight; j++) 
		{
			for (int i=0; i<imgWidth; i++)
			{
				pic[i][j][t] = 0;
			}
		}
	}

	

	
	while (!input.eof()) 
	{
		input >> x >> y >> junk >> junk >> junk >> time;
		
		if (time > frames || input.eof()) 
		{
			break;
		}
		cout << time << endl;
		
		if (x > 1 && x < imgWidth-2
			&& 	y > 1 && y < imgHeight-2
			&&  time < frames)
		{
			/*// Draw a diamond type shape on the picture.
			//
			// 0 0 0 0 0 0 
			// 0 0 * 0 0 0
			// 0 * * * 0 0
			// * * * * * 0
			// 0 * * * 0 0
			// 0 0 * 0 0 0
			// 0 0 0 0 0 0
			//*/
			pic[(int)x][(int)y][(int)time] = pointBright;
			pic[(int)x-1][(int)y][(int)time] = pointBright;
			pic[(int)x][(int)y-1][(int)time] = pointBright;
			pic[(int)x][(int)y+1][(int)time] = pointBright;
			pic[(int)x+1][(int)y][(int)time] = pointBright;
			
			pic[(int)x-2][(int)y][(int)time] = pointBright;
			pic[(int)x][(int)y-2][(int)time] = pointBright;
			pic[(int)x][(int)y+2][(int)time] = pointBright;
			pic[(int)x+2][(int)y][(int)time] = pointBright;
			
			pic[(int)x-1][(int)y-1][(int)time] = pointBright;
			pic[(int)x-1][(int)y+1][(int)time] = pointBright;
			pic[(int)x+1][(int)y-1][(int)time] = pointBright;
			pic[(int)x+1][(int)y+1][(int)time] = pointBright;

		}
	}
	
	for (int t=0; t<frames; t++) 
	{
		sprintf(number, "%d", t);
		strcpy(outputPath, imgDirectoryAndFilename);
		strcat(outputPath, number);
		strcat(outputPath, ".pgm");
		ofstream img(outputPath);
		cout << outputPath << endl;
		
		img << "P2" << endl;
		img << imgWidth <<" "<< imgHeight << endl;
		img << maxBright << endl;
		
		for (int j=0; j<imgHeight; j++) 
		{
			for (int i=0; i<imgWidth; i++)
			{
				img << pic[i][j][t] << " ";
				
			}
			img << endl;
		}
		
		// Clears the string by putting a null in the first value.
		outputPath[0] = 0;

	}
}



void showPointsFrom_tracks(apstring tracksPath, apstring imgDirectory, apstring fileName, int imgHeight, int imgWidth, int frames)
{
	// Writes an image from some tracks path where x & y coords are in the first
	// two columns, respectively. Outputs a .pgm image.
	
	// tracksPath = tracks file you want to make a movie from.
	
	char outputPath[1000];
	char imgDirectoryAndFilename[1000];
	strcpy(imgDirectoryAndFilename, imgDirectory.c_str());
	strcat(imgDirectoryAndFilename, fileName.c_str());
	char number[10];
	ifstream input(tracksPath.c_str());

	//short pic[imgWidth][imgHeight][frames];	// uint8_t holds values from 0 to 255. Cuts down on memory, hopefully.
	//short *pic = new short[imgWidth*imgHeight*frames];
	using namespace std;
	vector< vector< vector<short> > > pic (imgWidth, vector< vector<short> > (imgHeight, vector<short>(frames,1)));
	// looks horrible, but this is really just an array. access as normal : pic[i][j][t]
	
	int maxBright = 15;
	int pointBright = 11;
	
	int i;
	int j;
	int t;
	
	float x, y; // Used for input, output.
	float junk;
	float time;
	
	
	for ( t=0; t<frames; t++) 
	{	
		for ( j=0; j<imgHeight; j++) 
		{
			for ( i=0; i<imgWidth; i++)
			{
				pic[i][j][t] = 1;
			}
		}
	}
	
	
	
	
	while (!input.eof()) 
	{
		input >> x >> y >> junk >> junk >> junk >> time >> junk;
		
		if (input.eof()) 
		{
			break;
		}
		//cout << time << endl;
		
		if (x > 1 && x < imgWidth-2
			&& 	y > 1 && y < imgHeight-2
			&&  time < frames)
		{
			/*// Draw a diamond type shape on the picture.
			 //
			 // 0 0 0 0 0 0 
			 // 0 0 * 0 0 0
			 // 0 * * * 0 0
			 // * * * * * 0
			 // 0 * * * 0 0
			 // 0 0 * 0 0 0
			 // 0 0 0 0 0 0
			 //*/
			pic[(int)x][(int)y][(int)time] = pointBright;
			pic[(int)x-1][(int)y][(int)time] = pointBright;
			pic[(int)x][(int)y-1][(int)time] = pointBright;
			pic[(int)x][(int)y+1][(int)time] = pointBright;
			pic[(int)x+1][(int)y][(int)time] = pointBright;
			
			pic[(int)x-2][(int)y][(int)time] = pointBright;
			pic[(int)x][(int)y-2][(int)time] = pointBright;
			pic[(int)x][(int)y+2][(int)time] = pointBright;
			pic[(int)x+2][(int)y][(int)time] = pointBright;
			
			pic[(int)x-1][(int)y-1][(int)time] = pointBright;
			pic[(int)x-1][(int)y+1][(int)time] = pointBright;
			pic[(int)x+1][(int)y-1][(int)time] = pointBright;
			pic[(int)x+1][(int)y+1][(int)time] = pointBright;
			
		}
	}
	
	for ( t=0; t<frames; t++) 
	{
		sprintf(number, "%d", t);
		strcpy(outputPath, imgDirectoryAndFilename);
		strcat(outputPath, number);
		strcat(outputPath, ".pgm");
		ofstream img(outputPath);
		cout << outputPath << endl;
		
		img << "P2" << endl;
		img << imgWidth <<" "<< imgHeight << endl;
		img << maxBright << endl;
		
		for ( j=0; j<imgHeight; j++) 
		{
			for ( i=0; i<imgWidth; i++)
			{
				img << pic[i][j][t] << " ";
				
			}
			img << endl;
		}
		
		// Clears the string by putting a null in the first value.
		outputPath[0] = 0;
		
	}
}
