/* 
 This is the main loop of the program. This program is also pretty easy to use. There are two 
 things you can do. You can either write a configuration file, which has important information that
 the program needs to analyze the image. Stuff like where the banks start and end, stuff like that.
 
 The second thing this program does is analyze images. It does this by first reading in the config
 file (which tells it where to read). It then analyzes it, then prints out the results. 
 
 writeConfigFile(configpath, inputpath, tracksFilename, fileidentifier);
 
 This line writes a config file. Here's the code I used to write it.
 
 apstring inputpath = "/Users/scott/Desktop/.../.../.../folder/"
 fileidentifier = "2406110120min";
 tracksFilename = fileidentifier + "_tracks.txt";
 configpath = inputpath + fileidentifier + "_config.txt";
 writeConfigFile(configpath, inputpath, tracksFilename, fileidentifier);
 
						–––––––––What the variables mean:-––––––––
 fileidentifier: a string that contains some information about the data run. ie, that little bit of
				 text that you use to tell you which run you're looking at.
 
 inputpath:	the path to the folder. use "/" for macs and "\" for PCs to indicate a new folder/directory. 
 
 configpath: the path where you want the config file to be written. I used the following:
			 configpath = inputpath + fileidentifier + "_config.txt", but you can use whatever you want.
 
 tracksFilename: the name of the tracks.txt file outputted from IDL. I used this code:  
				 tracksFilename = fileidentifier + "_tracks.txt";
				 because that's how I saved it in IDL.
 
 So that's the low-down on writeConfigFile.
 
 ANALYZING TRACK.txt FILES
 To analyze the tracks.txt file, all you need to do is call calculateVelocities(apstring configpath);
 It does everything for you. It reads the config file at the path given, and outputs a couple files.
 The files are outputted according the the config. file. It outputs the files into the folder 
 "inputpath". The files it outputs are in the form "fileidentifier_velocity.txt".
 
 
 
 Hopefully, this makes it easier to do batch runs. You can set up all of the config files ahead of 
 time (that'll be the longest part: you'll have to edit the banks, pixelsPerUnit, etc...), then copy
 and paste the same code over and over again to have it repeat itself. It'd look something like this:
 
 ...
 ...
 ...
 
 fileidentifier = "2406110100min"
 configpath = inputpath + fileidentifier + "_config.txt";
 calculateVelocities(configpath);
 
 fileidentifier = "2406110120min"
 configpath = inputpath + fileidentifier + "_config.txt";
 calculateVelocities(configpath);
 
 fileidentifier = "2406110140min"
 configpath = inputpath + fileidentifier + "_config.txt";
 calculateVelocities(configpath);
 
 fileidentifier = "2406110160min"
 configpath = inputpath + fileidentifier + "_config.txt";
 calculateVelocities(configpath);
 
 ...
 ...
 ...
 
 It's also useful to have a multi-file search-and-replace tool, to edit the 
 all of the values as many _config files as you want. I use TextWrangler, by
 BareBones Software. One for windows can be found at this link: 
 http://www.multiplefindreplace.com/
 
 
 – Scott Sievert, 7/26/2011
 
 */
 


 
 
 
#include <stdio.h>
#include <fstream.h>
#include "calculateVelocities.h"
#include "writeConfigFile.h"
#include "apstring.h"
#include "fileIOTest.h"
#include "showPoints.h"
#include "sw1.h"
#include "widthFinder.h"

#define YES 1
#define NO 0


void wait2(double seconds);

int main()
{
	//apstring inputpath = "/Users/scottsievert/Desktop/school/college/2011_Summer/granular_flow_research/Data/tracks_position_files/";
	//apstring fileidentifier = "2806110260min";
	/*apstring tracksFilename = fileidentifier + "_tracks.txt";
	apstring configpath = inputpath + fileidentifier;
			 configpath = configpath + "_config.txt";
	
	fileidentifier = "2806110320min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	
	fileidentifier = "2806110340min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	
	fileidentifier = "2806110362min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	 */
	//apstring tracksPath = "/Users/scottsievert/Desktop/807110340minTEST/807110340minTEST_positions.txt";
	//apstring imgDirectory = "/Users/scottsievert/Desktop/testIO/";
	//apstring filename = "807110340min_filteredpts0";
	//showPoints(tracksPath, imgDirectory, filename, 100, 100, 10);
	
	
	char choice;
	int quit=NO;
	while (quit==NO) 
	{
		//cin.ignore(INT_MAX,'\n');  // clears input stream

		cout << endl;
   cout << "[c]alculate velocity profiles" << endl
		<< "[w]rite a configuration file" << endl
		<< "[s]how tracks" << endl
		<< "[p]rocess _velocity.csv files (width, avg. velo)" << endl
		<< "[h]elp" << endl
		<< "[q]uit" << endl;
		cout << "Your choice : ";
		cin >> choice;
		cout << endl;
		cin.ignore();
		//cin.ignore(INT_MAX,'\n');  // clears input stream
		
		switch (choice) {
			case 'c':
			{
				apstring path;
				apstring config_filepath;
				cout << "Enter the path to a file that contains the paths to configuration files on each line:" << endl;
				getline(std::cin, path);
				std::ifstream input(path.c_str());
				if (!input.is_open()) 
				{
					cout << "Error loading file." << endl;
					cout << "Restarting program in 5 seconds." << endl;
					cin.ignore();  // clears input stream

					wait2(5);
					break;
				}
				// More statements
				while (!input.eof()) 
				{
					getline(input, config_filepath);
					cout << endl << "––––––––––– NEW DATASET –––––––––––" << endl;
					cout <<"Current config file : " << config_filepath;
					calculateVelocities(config_filepath);
				}
				break;
				
			}
			case 'p':
			{
				apstring path;
				apstring outputPath;
				apstring velocity_filepath;
				double percentageOfMax;
				double percentageOfMaxVelocity;
				long double width;
				long double averageVelocity;
				

				
				
				cout << "Enter the the path+name of a file that contains a list of velocity.csv files on each line that you want to calculate the width of :" << endl;
				getline(std::cin, path);
				cout << "Enter the the path+name of a file where you want the results to be written :" << endl;
				getline(std::cin, outputPath);
				
				cout << "What percentage of the maxVelocity do you want to be called the width? (enter 50% as 0.50) : ";
				cin >> percentageOfMax;
				cin.ignore();  // clears input stream
				std::ifstream input(path.c_str());
				std::ofstream output(outputPath.c_str());
				
				cout << "What percentage of max velocity do you want to average the velocity over? (top 5%? Enter 0.95) : ";
				cin >> percentageOfMaxVelocity;
				cin.ignore();  // clears input stream
				
				if (!input.is_open()) 
				{
					cout << "Error loading file." << endl;
					cout << "Restarting program in 5 seconds." << endl;
					
					wait2(5);
					break;
				}
				// More statements
				output << "ID" << "\t" << "width" <<"\t" <<"avg. velocity" <<endl;
				while (!input.eof()) 
				{
					getline(input, velocity_filepath);
					cout << endl << "––––––––––– NEW DATASET –––––––––––" << endl;
					cout <<"Current velocity file : " << velocity_filepath << endl;
					
					width = findWidth(velocity_filepath, percentageOfMax);
					averageVelocity = findAvgVelocity(velocity_filepath, percentageOfMaxVelocity);
					cout <<"\t" <<"width: " << width << endl <<"\t" << "velocity: " << averageVelocity << endl;
					
					if (width==-1) 
					{
						cout << "–––––– Error! ––––––" << endl << "widthFinder returned an error." << endl;
					}

					output << velocity_filepath << "\t" << width <<"\t"<< averageVelocity <<endl;
				}
				cout << "Your widths + velocities have been found and written to " << outputPath << " in the same order as the input file." << endl;
				break;
			}
			case 'w':
			{
				apstring configPath;
				apstring inputPath;
				apstring tracksName;
				apstring fileID;
				cout << "Enter the path + filename of your desired config file: " << endl;
				getline(cin, configPath);

				cout << "Enter the path to the directory that contains the tracks.txt file, including final slash : " << endl;
				getline(cin, inputPath);

				cout << "Enter the name of the tracks.txt file, including the extensions." << endl;
				getline(cin, tracksName);

				cout << "Enter the fileID – the unique bit that identifies which run it was." << endl;
				getline(cin, fileID);

				writeConfigFile(configPath, inputPath, tracksName, fileID);
				
				break;
			}
			case 's':
			{
				apstring tracksPath;
				apstring imgDirectory;
				apstring fileName;
				int imgHeight;
				int imgWidth;
				int frames;
				char fileChoice;

				
				cout << "Show tracks from a..."<< endl << "_[p]ositions.txt file" << endl << "_[t]racks.txt file" << endl;
				cin >> fileChoice;
				cin.ignore();  // clears input stream

				
				switch (fileChoice) 
				{
					case 't':
					{	
						cout << "Enter the full path to the _tracks.txt file : " << endl;				
						getline(cin, tracksPath);
						
						cout << "Enter the directory to contain all of the images : " << endl;
						getline(cin, imgDirectory);
						
						cout << "Enter the image filename (ie, 1107110140min, not 1107110140min00001.pgm) : " << endl;
						getline(cin, fileName);
						//cin.ignore(INT_MAX,'\n');  // clears input stream
						
						cout << "Image width? : ";
						cin >> imgWidth;
						cin.ignore();  // clears input stream

						cout << "Image height? : ";
						cin >> imgHeight;
						cin.ignore();  // clears input stream

						cout << "Number of frames? : ";
						cin >> frames;
						cin.ignore();  // clears input stream

						
						showPointsFrom_tracks(tracksPath, imgDirectory, fileName, imgHeight, imgWidth, frames);
						cin.ignore();  // clears input stream
						break;
					}
					case 'p':
					{
						apstring positionPath;
						cout << "Enter the full path to the _positions.txt file : " << endl;				
						getline(cin, positionPath);
						cin.ignore();  // clears input stream
						
						cout << "Enter the directory to contain all of the images : " << endl;
						getline(cin, imgDirectory);
						cin.ignore();  // clears input stream
						
						cout << "Enter the image filename (ie, 1107110140min, not 1107110140min00001.pgm) : " << endl;
						getline(cin, fileName);
						cin.ignore();  // clears input stream
						
						cout << "Image width? : ";
						cin >> imgWidth;
						cin.ignore();  // clears input stream

						cout << "Image height? : ";
						cin >> imgHeight;
						cin.ignore();  // clears input stream

						cout << "Number of frames? : ";
						cin >> frames;
						cin.ignore();  // clears input stream

						
						showPointsFrom_positions(positionPath, imgDirectory, fileName, imgHeight, imgWidth, frames);
						cin.ignore();  // clears input stream
						break;
					}
						
					default:
					{
						cout << "Sorry, input not recognized." << endl;
					}
						break;
				}

				break;
			}
			case 'h':
			{
				cout <<endl <<"Help: " << endl;
				cout << "If you haven't done so already, read the readme.txt file in the same directory as this program. It should prove useful. " << endl;
				cout << "Anyways, here's the basic how-to for this program. " << endl;
				cout << "This program does 3 things. " << endl;
				cout << "1. Calculate velocity profiles from _tracks.txt files outp";
				cout << "utted from the IDL code. It does this by reading in a _config";
				cout << "file that contains all of the information needed to calculate";
				cout << "all that stuff: pixelsPerUnit, image dimensions, etc..." << endl << endl;
				
				cout << "2. This program writes those config files. It prompts you ";
				cout << "to input each variable, but if you screw up, that's okay. ";
				cout << "You can always edit them with a text editor later, too" << endl << endl;
				
				cout << "3. This program also makes movies of particles that you have left, ";
				cout << "since the IDL code overlays *every* feature it finds, you don't";
				cout << " know what particles you have left. It writes them in the .pgm";
				cout << " format, which ImageJ can open.";
				cout << endl << endl << "Enter any key to continue: ";
				cin >> choice;
				
				
				
				cin.ignore();  // clears input stream
				break;
			}
			case 'v':
			{
				apstring path;
				cout << "Your choice: vader!" << endl;
				cout << "Please make your window a little bigger – at least 90 columns wide. Thanks!" << endl;
				swiv();
				/*uint32_t size = sizeof(path);
				 _NSGetExecutablePath(path, &size);
				 cout << "Current working dir: " << path;
				 
				 std::string sw1Path;
				 sw1Path = path;
				 sw1Path = sw1Path + "/Resources/sw1.txt";
				 cout << endl << sw1Path;*/
				
				/*cout << "Drag and drop the sw1.txt file here, then press enter: ";
				getline(std::cin, path);
				std::ifstream sw(path.c_str());
				
				char line[100];
				if (!sw.is_open()) 
				{
					cout << "ERROR LOADING MOVIE" << endl;
				}
				
				cout << endl << endl << endl << endl << endl << endl;
				cout<<"            .                                                                     " << endl; //wait2(1.45);
				cout<<"      .               .              .         .        .               .         " << endl; //wait2(1.45);
				cout<<"                                         .                 .       .      .       " << endl; //wait2(1.45);
				cout<<"                  .         .   .                                                 " << endl; //wait2(1.45);
				cout<<"           .                                     .    .               .       .   " << endl; //wait2(1.45);
				cout<<"    .                  .                 .    .                   .               " << endl; //wait2(1.45);
				cout<<"          .                    .                        .            .            " << endl; //wait2(1.45);
				cout<<"             .                           .    .                          .     .  " << endl; //wait2(1.45);
				cout<<"      .             .               .                   .     .                   " << endl; //wait2(1.45);
				cout<<"                            .                       .                             " << endl; //wait2(1.45);
				cout<<"       .      . .       .                                      .       .          " << endl; //wait2(1.45);
				cout<<"          .                       .                                         .     " << endl; //wait2(1.45);
				cout<<"                  .                                  .                .           " << endl; //wait2(1.45);
				cout<<"            .       A long time ago, in a galaxy far, far away...    .   .        " << endl; //wait2(1.45);
				cout<<"                                        .                                         " << endl; //wait2(1.45);
				cout<<"         .              .                           .                  .          " << endl; //wait2(1.45);
				cout<<"            .                                                                     " << endl; //wait2(1.45);
				cout<<"      .               .              .         .        .               .         " << endl; //wait2(1.45);
				cout<<"                                         .                 .       .      .       " << endl; //wait2(1.45);
				cout<<"      .       .               .       .                     .        .      .     " << endl; //wait2(1.45);
				cout<<"      .                                           .             .                 " << endl; //wait2(1.45);
				cout<<"                     .           . .                         .                    " << endl; //wait2(1.45);
				cout<<"  .                         .                                      .              " << endl; wait2(5.0);
				cout<<"             .                                  .      .                    .     " << endl; //wait2(1.45);
				cout<<"      .             .               .                                             " << endl; //wait2(1.45);
				cout<<"                            .                       .                  .          " << endl; wait2(1.4);
				cout<<"              .                 .                             .                   " << endl; wait2(1.4);
				cout<<"   .                            E P I S O D E    IV                         .     " << endl; wait2(1.4);
				cout<<"                       .          .                                               " << endl; wait2(1.4);
				cout<<"            .                   A   N E W   H O P E      .           .            " << endl; wait2(1.4);
				cout<<"                                              .                                   " << endl; wait2(1.4);
				cout<<"    .                       It is a period of civil war.           .            . " << endl; wait2(1.4);
				cout<<"                          Rebel spaceships, striking from   .  .                  " << endl; wait2(1.4);
				cout<<"              .         a hidden base, have won their first          .            " << endl; wait2(1.4);
				cout<<"     .                victory against the evil Galactic Empire.                 . " << endl; wait2(1.4);
				cout<<"                                                            .      .              " << endl; wait2(1.4);
				cout<<"                   During the battle, Rebel spies managed to steal                " << endl; wait2(1.4);
				cout<<".       .         secret plans to the Empire's ultimate weapon, the  .   .        " << endl; wait2(1.4);
				cout<<"                DEATH STAR, an armored space station with enough power         .  " << endl; wait2(1.4);
				cout<<"               to destroy an entire planet.                                .      " << endl; wait2(1.4);
				cout<<"    .                                                                             " << endl; wait2(1.4);
				cout<<"             Pursued by the Empire's sinister agents, Princess Leia races         " << endl; wait2(1.4);
				cout<<"           home aboard her starship, custodian of the stolen plans that can     . " << endl; wait2(1.4);
				cout<<"          save her people and restore freedom to the galaxy...           .   .    " << endl; wait2(1.4);
				cout<<"    .                            .     .                                          " << endl; wait2(1.4);
				cout<<"            .                                                                     " << endl; wait2(1.4);
				cout<<"      .               .              .         .        .               .         " << endl; wait2(1.4);
				cout<<"                                         .                 .       .      .       " << endl; wait2(1.4);
				cout<<"                          .                 .           .                         " << endl; wait2(1.4);
				cout<<"      .       .               .       .                     .        .      .     " << endl; wait2(1.4);
				cout<<"      .                                           .             .                 " << endl; wait2(1.4);
				cout<<"                     .           . .                         .                    " << endl; wait2(1.4);
				cout<<"  .                         .                                      .              " << endl; wait2(2.0);
				cout<<"               .       .                    .      .                              " << endl; //wait2(0.6);
				cout<<"         .               .                                                .       " << endl; //wait2(0.6);
				cout<<"            .        .                     .                     .                " << endl; //wait2(0.6);
				cout<<"                .               .              .             .         .          " << endl;
				cout<<"                 _________________      ____         __________                   " << endl; //wait2(0.01);
				cout<<"           .    /                 |    /    \\    .  |          \\                  " << endl; //wait2(0.01);
				cout<<"       .       /    ______   _____| . /      \\      |    ___    |     .     .     " << endl; //wait2(0.01);
				cout<<"               \\    \\    |   |       /   /\\   \\     |   |___>   |                 " << endl; //wait2(0.01);
				cout<<"             .  \\    \\   |   |      /   /__\\   \\  . |         _/             .    " << endl; //wait2(0.01);
				cout<<".        ________>    |  |   | .   /            \\   |   |\\    \\_______    .       " << endl; //wait2(0.01);
				cout<<"        |            /   |   |    /    ______    \\  |   | \\           |           " << endl; //wait2(0.01);
				cout<<"        |___________/    |___|   /____/      \\____\\ |___|  \\__________|    .      " << endl;  //wait2(0.01);	// This looks funny beacuse you need to use "\\" to print "\"
				cout<<".        ____    __  . _____   ____      .  __________   .  _________             " << endl; //wait2(0.01);
				cout<<"        \\    \\  /  \\  /    /  /    \\       |          \\    /         |      .     " << endl; //wait2(0.01);
				cout<<"         \\    \\/    \\/    /  /      \\      |    ___    |  /    ______|  .         " << endl; //wait2(0.01);
				cout<<"          \\              /  /   /\\   \\ .   |   |___>   |  \\    \\                  " << endl; //wait2(0.01);
				cout<<"  .        \\            /  /   /__\\   \\    |         _/.   \\    \\              .  " << endl; //wait2(0.01);
				cout<<"            \\    /\\    /  /            \\   |   |\\    \\______>    |   .            " << endl; //wait2(0.01);
				cout<<"             \\  /  \\  /  /    ______    \\  |   | \\              /                 " << endl; //wait2(0.01);
				cout<<"       .      \\/    \\/  /____/      \\____\\ |___|  \\____________/           .      " << endl; //wait2(0.01);
				cout<<"                                 .                                  .             " << endl; //wait2(0.01);
				cout<<".               .            .                                                .   " << endl; //wait2(0.01);
				cout<<"                       .                    .                  .                  " << endl; wait2(5);
				
				double frames;
				while (!sw.eof())
				{
					//sw.getline(line,100);
					// Gets first "/ *" thing.
					sw >> frames;
					//cout <<"frames: " << frames << endl;
					cout << endl << endl << endl << endl << endl <<endl <<endl <<endl <<endl;
					cout <<"      " << "–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––" << endl;
					for (int i=1; i<=14; i++) 
					{
						sw.getline(line,100);
						cout <<"      " << line << endl;
					}
					cout <<"      " << "–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––" << endl << endl << endl;
					wait2((double)(frames/(15)));
					
				}*/
				
				break;
			}
			case 'q':
			{
				quit = YES;
				break;
			}
			default:
			{
				cout << endl << endl;
				cout << "    ▄██████████████▄▐█▄▄▄▄█▌" << endl;
				cout <<	"    ██████▌▄▌▄▐▐▌███▌▀▀██▀▀" << endl;
				cout << "    ████▄█▌▄▌▄▐▐▌▀███▄▄█▌" << endl;
				cout <<	"    ▄▄▄▄▄██████████████▀"<< endl;
				cout << "Sorry, input wasn't recognized." << endl;
				//cin.ignore(INT_MAX,'\n');  // clears input stream
				break;
			}
				
				
				
		}

	}
	
		



	/*
	 // Run on 7/27/11 @ 12:05.
	 // Intial flows not analyzed.
	fileidentifier = "2406110146min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	
	fileidentifier = "2406110166min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	
	fileidentifier = "2406110181min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	
	fileidentifier = "2406110220min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	
	fileidentifier = "2406110240min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	
	fileidentifier = "2406110260min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	
	fileidentifier = "2806110120min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	
	fileidentifier = "2806110152min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	
	fileidentifier = "2806110167min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	
	fileidentifier = "2806110220min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	
	fileidentifier = "2806110240min";
	configpath = inputpath + fileidentifier + "_config.txt";
	calculateVelocities(configpath);
	 
	 */
	

	
	
}

void wait2 ( double seconds )
{
	clock_t endwait;
	endwait = clock () + seconds * CLOCKS_PER_SEC ;
	while (clock() < endwait) {}
}




/*
 cout<<"                      A terrible civil war burns throughout the           .     . " << endl; wait2(1.4);
 cout<<"                     galaxy: a rag-tag group of freedom fighters   .  .           " << endl; wait2(1.4);
 cout<<"              .     has risen from beneath the dark shadow of the            .    " << endl; wait2(1.4);
 cout<<"     .          .  evil monster the Galactic Empire has become.                 . " << endl; wait2(1.4);
 cout<<"                     Imperial  forces  have  instituted  a reign of   .      .    " << endl; wait2(1.4);
 cout<<"                 terror,  and every  weapon in its arsenal has  been              " << endl; wait2(1.4);
 cout<<".       .       turned upon the Rebels  and  their  allies:  tyranny, .   .       " << endl; wait2(1.4);
 cout<<"               oppression, vast fleets, overwhelming armies,  and fear.        .  " << endl; wait2(1.4);
 cout<<"         .    Fear  keeps  the  individual systems in line,  and is the   .       " << endl; wait2(1.4);
 cout<<"    .        prime motivator of the New Order.              .                     " << endl; wait2(1.4);
 cout<<"               Outnumbered and outgunned,  the Rebellion burns across the   .     " << endl; wait2(1.4);
 cout<<"           vast reaches of space and a thousand-thousand worlds, with only     .  " << endl; wait2(1.4);
 cout<<"      .   their great courage - and the mystical power known as the Force -       " << endl; wait2(1.4);
 cout<<"   .     flaming a fire of hope.                                     .            " << endl; wait2(1.4);
 cout<<"           This is a  galaxy  of wondrous aliens,  bizarre monsters,  strange   . " << endl; wait2(1.4);
 cout<<"       Droids, powerful weapons, great heroes, and terrible villains.  It is a    " << endl; wait2(1.4);
 cout<<"      galaxy of fantastic worlds,  magical devices, vast fleets, awesome machi-  ." << endl; wait2(1.4);
 cout<<"     nery, terrible conflict, and unending hope.              .         .         " << endl; wait2(1.4);

 
 */














