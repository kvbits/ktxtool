/*
 * ktxtool, A conversion and compression tool for the KTX image format
 *
 * Copyright (C) 2014 Luis Jimenez, www.kvbits.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. 
 */


#include "ktxtool.h"
#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include <string>
#include <fstream>
#include <assert.h>
#include <iomanip>
#include "InputFormat.h"
#include "ktx/Container.h"
#include "PixelData.h"

#include "ktx/Compression/Compression.h"
#include "ktx/Compression/ETC1/ETC1.h"





using namespace std;





typedef std::map<char, Option> OptionMap;
typedef std::list<InputFormat*> FormatList;


OptionMap  options;
FormatList formats;




Option* GetOption(char id)
{
	OptionMap::iterator it = options.find(id);

	if (it != options.end())
	{
		
		return &it->second;
	}


	return NULL;
}

Option& AddOption(char id, int flags, const char* desc)
{
	assert(GetOption(id) == NULL);

	options[id] = Option();

	Option& opt = options[id];
	opt.id = id;
	opt.flags = flags;
	opt.desc = desc;

	return opt;
}

void AddInputFormat(InputFormat* pFormat)
{
	assert(pFormat != NULL);
	formats.push_back(pFormat);
}

static void DumpOptions()
{
	OptionMap::iterator it = options.begin();

	cout << "  Options:" << endl;

	while (it != options.end())
	{
		Option& opt = it->second;

		string flags = "";

		if (opt.IsDefined())
		{
			flags += "OPTION_DEFINED ";
		}

		if (opt.ExpectsValue())
		{
			flags += "OPTION_EXPECTS_VALUE ";
		}

		if (opt.IsRequired())
		{
			flags += "OPTION_REQUIRED ";
		}

		cout << "    -" << opt.id << " ";
		
		//cout << setfill(' ') << setw(15) << opt.value << " : " << flags << endl;
		cout << setfill(' ') << setw(15) << "  :  ";
		cout << opt.desc;

		cout << endl;

		it++;
	}

}

static void DumpSupportedFormats()
{
	FormatList::iterator it = formats.begin();

	cout << "  Supported formats: " << endl;

	while (it != formats.end())
	{
		cout << "    " << (*it)->GetName() << endl;

		it++;
	}
}

static void DumpHelp()
{
	cout << "ktxtool v0.2.0" << endl << endl;
	cout << "  Usage: ktxtool -[OPTIONS]... FILEIN [FILEOUT]" << endl << endl;
	
	DumpOptions();

	cout << endl << endl;

	DumpSupportedFormats();

	cout << endl << endl;

}

static bool FileExists(const string& filePath)
{
	ifstream file(filePath.c_str());

	if (file.good())
	{
		file.close();
		return true;
	}

	file.close();
	return false;
}

int main (int argc, char* argv[])
{
	//define the options
	AddOption('c', 0, "Compress with ETC1");
	AddOption('v', 0, "Verbose output");
	AddOption('f', OPTION_REQUIRED | OPTION_EXPECTS_VALUE, "Input file. For multiple faces use commas (no spaces)");
	AddOption('o', OPTION_EXPECTS_VALUE, "Output file");
	AddOption('d', 0, "Dumps mipmaps as individual ppm files");
	AddOption('y', 0, "Flips the Y Axis or upside down");


	if (argc < 2)
	{
		DumpHelp();
		return 1;
	}

	if (argc >= 2)
	{
		string arg1 = argv[1];
		
		if (arg1 == "--help" || arg1 == "-h")
		{
			DumpHelp();

			return 0;
		}
	}
	
	
	//Start as option mode (looks for -{ID})
	bool optionIDMode = true;
	string argStr;
	int parsedArg = 1;

	//current option being parsed
	Option* opt = NULL;

	//parse options
	for (; parsedArg < argc; parsedArg++)
	{
		//avoid silly crashes by using std::string, at the end this is C++ anyways
		argStr = argv[parsedArg];


		bool validOptionID = (argStr.size() == 2 && argStr[0] == '-');



		if (optionIDMode)
		{
			//check if it's a valid option
			if (!validOptionID)
			{
				//At this point the execution is still valid
				break;
			}
			
			assert(argStr.size() >= 2);

			opt = GetOption(argStr[1]);

			if (opt == NULL)
			{
				cerr << "Unrecongized option " << argStr << endl;

				return 2;
				break;
			}

			opt->MarkAsDefined();

			assert(opt->IsDefined());


			//if is expecting a value then leave optionIDMode
			if (opt->ExpectsValue())
			{
				optionIDMode = false;
			}
		}
		else
		{
			assert(opt != NULL);

			if (validOptionID)
			{
				//this will trigger the error bellow
				break;
			}

			opt->value = argStr;

			optionIDMode = true;
		}

	}

	if (optionIDMode == false)
	{
		assert(opt != NULL);
		cerr << "Option -" << opt->id << " expected a value!" << endl;
		return 4;
	}

	//now parse the "auto" options -f and -o, error if already defined direclty
	Option* opt1 = GetOption('f');
	Option* opt2 = GetOption('o');
	
	assert(opt1 != NULL && opt2 != NULL);


	if (argc - parsedArg > 0)
	{
		if (opt1->IsDefined())
		{
			cerr << "Unexpected input file, -f already defined" << endl;
			return 5;
		}

		opt1->MarkAsDefined();
		opt1->value = argv[parsedArg];
	}

	if (argc - parsedArg > 1)
	{
		if (opt2->IsDefined())
		{
			cerr << "Unexpected output file, -O already defined" << endl;
			return 6;
		}

		opt2->MarkAsDefined();
		opt2->value = argv[parsedArg + 1];
	}

	//pre-validate options
	OptionMap::iterator it = options.begin();

	while (it != options.end())
	{
		Option& opt = it->second;

		if (opt.IsRequired() && !opt.IsDefined())
		{
			cerr << "option -" << opt.id << " is required (" << opt.desc << ")" << endl;

			return opt.id + 256;
		}

		it++;
	}


	
	//define some variables
	Container ktx; //this holds float/32bit color data only


	Compression* pComp = nullptr;

	if (GetOption('c')->IsDefined())
	{
		pComp = new ETC1();
		pComp->SetQuality(Compression::QUALITY_HIGH);
	}


	//The faces
	vector<string> faces;
	
	std::istringstream ss(opt1->value);
	std::string token;

	//extract the filenames as faces
	while (std::getline(ss, token, ','))
	{
		faces.push_back(token);
	}


	//all faces should match this format, width and height (taken from index 0)
	Format refFormat;
	float  refWidth;
	float  refHeight;

	//process all the faces	
	for (size_t i = 0; i < faces.size(); i++)
	{
		const string& fileName = faces[i];

		cout << "Processing face at index " << i << ": " << fileName << endl;

		//check if the file exists
		if (!FileExists(fileName))
		{
			cerr << "Input file  doesn't exist" << endl;
			return 7;
		}

		//get the extension
		string strExt = opt1->value.substr(opt1->value.find_last_of(".") + 1);

		if (strExt.size() == 0)
		{
			cerr << "Input file has no extension, therefore unabled to determine its format" << endl;
			return 10;
		}

		//Lookup for a compatible format
		InputFormat* pFormat = nullptr;

		FormatList::iterator it = formats.begin();

		while (it != formats.end())
		{

			if ((*it)->CheckExtension(strExt.c_str()))
			{
				pFormat = (*it);
				break;
			}

			it++;
		}
		
		if (pFormat == nullptr)
		{
			cerr << "Input format is not supported" << endl;
			return 8;
		}

		assert(pFormat != nullptr);

		//Create the pixel data and add the face into the ktx container
		PixelData* pPixelData = pFormat->CreatePixelData(fileName.c_str());

		if (pPixelData == nullptr)
		{
			cerr << "Couldn't create pixel data from input file" << endl;
			return 11;
		}

		const float  w = pPixelData->GetWidth();
		const float  h = pPixelData->GetHeight();
		const Format f = pPixelData->GetFormat(); 
		
		//if first face then initialize the container
		if (i == 0)
		{
			refFormat = f;
			refWidth = w;
			refHeight = h;

			ktx.Init(w, h, 1, faces.size());
			ktx.SetFormat(f, COLOR_DEPTH_8BIT, pComp);
		}

		if (!(refFormat == f && refWidth == w && refHeight == h))
		{
			cerr << "Format/Dimmension mismatch, all faces should match the face at index 0" << endl;
			return 15;
		}

		ktx.SetData(0, i, pPixelData);

		//we don't need the pixel data anymore
		delete pPixelData;
	}


	//setup the output file and write the container
	string outputFile = opt2->value;

	if (outputFile.size() == 0)
	{
		assert(faces.size() >= 1);

		size_t dotAt = faces[0].find_last_of('.');

		outputFile = faces[0].substr(0, dotAt);
		outputFile += ".ktx";
	}
	
	ktx.GenerateMipmaps();

	ktx.Write(outputFile.c_str());




	return 0;
}
















