//
//  calculateVelocities.h
//  Granular Flows Image Analysis
//
//  Created by Scott Sievert on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

/*
 structs for main.cpp. Nothing special.
 */
#include "apstring.h"

int calculateVelocities(apstring config_filepath);

struct line
{
	// Used for the banks.
	long double start_x;
	long double start_y;
	long double end_x;
	long double end_y;
	long double slope;
	long double x;
	long double y;
	long double numberOfPoints;
};

struct vector
{
	// Used only for straight x&y image coords.
	long double x_component;
	long double y_component;
};
// Used for "straight" coordinates: straight across and straight down.

struct streamVector
{
	// streamVector.downstream = position/velocity in the downstream direction. Likewise for crossstream.
	long double downstream_component;
	long double crossstream_component;
};
// Used for "stream" coordinates: downstream and crossstream.

struct velocityOutput
{
	// Unused, I think.
	streamVector particle_position_corrected;
	streamVector particle_velocity_corrected;
	long double ID;
};

struct cell
{
	// Used for calculating velocity profiles.
	long double center_x;
	long double center_y;
	streamVector center;
	streamVector leftEdge;
	long double numberOfPoints;
	long double avgDownstreamVelocity;
	long double avgCrossstreamVelocity;
};

struct dataPoint
{
	// Used for information about each particle, and each point along the path.
	vector position;
	streamVector streamPosition;
	streamVector streamVelocity;
	long double ID;
	long double time; // Might be use later for time-varying flows.
	int aboveMinVelocity; // BOOL value used for finding width.
};

struct gridCoords
{
	// Used for indexes to the grid array. 
	long cross;
	long down;
};

struct dimensions
{
	streamVector min;			// Stream coords of start position of furthest point (ie, one of the corners)
	streamVector max;			// Stream coords of furthest position downstream+crosstream (another corner)
	streamVector current;	
};

struct pixelsPerUnit
{
	long double top;
	long double bottom;
	long double horizontal;
	long double height;
	// The number of pixels between the top and bottom measurements.
};







