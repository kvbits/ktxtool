#include <iostream>
#include <map>
#include <string>
#include <assert.h>







using std::string;



enum OptionFlags
{
	OPTION_DEFINED = 0x01,
	OPTION_EXPECTS_VALUE = 0x02,
	OPTION_REQUIRED = 0x08
};


struct Option
{
	char   id;
	int    flags;
	string value;
	string desc;

	Option()
	{
		id = char(0);
		flags = 0;
	}

	inline bool IsDefined() const
	{
		return (flags & OPTION_DEFINED) != 0;
	}

	inline bool ExpectsValue() const
	{
		return (flags & OPTION_EXPECTS_VALUE) != 0;
	}

	inline bool IsRequired() const
	{
		return (flags & OPTION_REQUIRED) != 0;
	}

	inline void MarkAsDefined()
	{
		flags |= OPTION_DEFINED; 
	}
};

typedef std::map<char, Option> OptionMap;

OptionMap  options;

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



int main (int argc, char* argv[])
{
	using namespace std;


	//define the options
	AddOption('c', OPTION_EXPECTS_VALUE, "Use a compression format [ETC1, ETC2, PTVPR, ] (Currently not supported)");
	AddOption('v', 0, "Verbose output");


	if (argc < 2)
	{
		cout << "ktxtool v0.1.0" << endl << endl;
		cout <<  "Usage: ktxtool -[OPTIONS]... FILEIN [FILEOUT]" << endl << endl;

		return 1;
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
	}

	//now parse some utility arguments (the last 2) as FILEIN FILEOUT
	if ((argc - parsedArg) > 0)
	{
		//cout << "  1 " <<  argv[parsedArg] << endl;
	}

	//cout << (argc - parsedArg) << endl;

#ifdef _DEBUG
	
	OptionMap::iterator it = options.begin();

	cout << "Option DUMP:" << endl;

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

		cout << " -" << opt.id << " " << opt.value << " : " << flags << endl;

		it++;
	}
	
#endif
	


	return 0;
}
