//
//  fileIOTest.cpp
//  Granular Flows Image Analysis
//
//  Created by Scott Sievert on 8/1/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "apstring.h"
#include <fstream.h>
#include <iomanip.h>

int testIO()
{
	apstring path = "/Users/scottsievert/Desktop/Untitled.txt";
	ifstream in(path.c_str());
	ofstream out(path.c_str());
	
	int array[6];
	
	out << 1 << endl
	<< 2 << endl
	<< 3 << endl
	<< 4 << endl
	<< 5 << endl
	<< 6 << endl;
	out.close();
	
	for (int i=0; i<6; i++) 
	{
		in >> array[i];
		cout << array[i]<< endl;
	}

}






