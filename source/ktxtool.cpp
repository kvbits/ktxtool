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
#include <map>
#include <list>
#include <string>
#include <fstream>
#include <assert.h>
#include <iomanip>
#include "InputFormat.h"
#include "ktx/Container.h"
#include "PixelData.h"




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
	cout << "ktxtool v0.1.0" << endl << endl;
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
	AddOption('c', OPTION_EXPECTS_VALUE, "Use a compression format [ETC1, ETC2, PTVPR, ] (Currently not supported)");
	AddOption('v', 0, "Verbose output");
	AddOption('f', OPTION_REQUIRED | OPTION_EXPECTS_VALUE, "Input file");
	AddOption('o', OPTION_EXPECTS_VALUE, "Output file");


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

	
	//now that we have all the required options let's validate them
	
	//Make sure the input file exists
	if (!FileExists(opt1->value))
	{
		cerr << "Input file doesn't exist" << endl;
		return 7;
	}


	string strExt = opt1->value.substr(opt1->value.find_last_of(".") + 1);

	if (strExt.size() == 0)
	{
		cerr << "Input file has no extension, therefore unabled to determine its format" << endl;
		return 10;
	}

	InputFormat* pFormat = NULL;

	//Look for compatible formats and create the pixel data
	{	
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
	}
	
	if (pFormat == NULL)
	{
		cerr << "Input format is not compatible" << endl;
		return 8;
	}

	assert(pFormat != NULL);


	//Get the pixel data and proceed with the convertion and compression
	PixelData* pPixelData = pFormat->CreatePixelData(opt1->value.c_str());

	if (pPixelData == NULL)
	{
		cerr << "Couldn't create pixel data from input file" << endl;
		return 11;
	}


	//ktx container
	Container ktx;

	ktx.Init(pPixelData->GetWidth(), pPixelData->GetHeight());


	return 0;
}
















