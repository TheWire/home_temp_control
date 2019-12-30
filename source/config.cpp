#include <config.h>
#include <algorithm>
#include <cstring>

using namespace std;

const string HomeThermoConfig::PYTHON_CMD = "python_command";
const string HomeThermoConfig::TRANS_PATH = "transmitter_script_path";
const string HomeThermoConfig::TRANS_CMD = "transmitter_scipt";
const string HomeThermoConfig::APP_KEY = "app_key";
const string HomeThermoConfig::TRANS_PIN = "transmitter_pin";
const string HomeThermoConfig::CODE_BITS = "code_bits";
const string HomeThermoConfig::TRANS_REPEAT = "transmit_repeat";
const string HomeThermoConfig::HEATING_CODE_ON = "heating_on_code";
const string HomeThermoConfig::HEATING_CODE_OFF = "heating_off_code";
const string HomeThermoConfig::DEFAULT_TEMP = "default_temp_target";
const string HomeThermoConfig::PATH_DATA = "path_data";
const string HomeThermoConfig::PATH_LOG = "path_log";

const string ThermoList::THERMO_FILE = "thermometers";
const string TimeTempList::TT_FILE = "times";


FileParser::FileParser(const char* path)
: configPath(path)
{
}

void FileParser::parse()
{
	ifstream configFile;
	configFile.open(configPath, fstream::in);
	///////
	if(configFile.fail()) {
		cout << "error opening file " << strerror(errno) << endl;
	}
	string line;
	while(getline(configFile, line))
	{
		line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
		if(line[0] == '#' || line.empty())
		{
			continue;
		}
		int delPos = line.find("=");
		setField(line.substr(0, delPos), parseValue(line.substr(delPos + 1)));

	}
	configFile.close();
	afterParse();
}

vector<string> FileParser::parseValue(string value)
{
	vector<string> valueList;
	int pos = 0;
	while(pos < value.length()) {
		value = value.substr(pos, value.length());
		pos = value.find("|");
		if(pos == string::npos)
		{
			pos = value.length();
		}
		valueList.push_back(value.substr(0, pos));
		pos++;
	}
	return valueList;
}



HomeThermoConfig::HomeThermoConfig(const char* path)
: FileParser(path),
app_key("0"), trans_pin(2), code_bits(24), trans_repeat(3),
heating_code_on("947871"), heating_code_off("947863"),
python_command("python3"), trans_path("trans/"), 
trans_command("XY_send_code.py"), default_temp_target(20)
{
	parse();
}

void HomeThermoConfig::setField(string field, vector<string> values)
{
	if(values.empty()) return;
	if(field.compare(PYTHON_CMD) == 0)
	{
		python_command = values[0];
	}
	else if(field.compare(TRANS_PATH) == 0)
	{
		trans_path = values[0];
	}
	else if(field.compare(TRANS_CMD) == 0)
	{
		trans_command = values[0];
	}
	else if(field.compare(TRANS_PIN) == 0)
	{
		trans_pin = stoi(values[0]);
	}
	else if(field.compare(APP_KEY) == 0)
	{
		app_key = values[0];
	}
	else if(field.compare(CODE_BITS) == 0)
	{
		code_bits = stoi(values[0]);
	}
	else if(field.compare(TRANS_REPEAT) == 0)
	{
		trans_repeat = stoi(values[0]);
	}
	else if(field.compare(HEATING_CODE_ON) == 0)
	{
		heating_code_on = values[0];
	}
	else if(field.compare(HEATING_CODE_OFF) == 0)
	{
		heating_code_off = values[0];
	}
	else if(field.compare(DEFAULT_TEMP) == 0)
	{
		default_temp_target = stof(values[0]);
	}
	else if(field.compare(PATH_DATA) == 0)
	{
		path_data = values[0];
	}
	else if(field.compare(PATH_LOG) == 0)
	{
		path_log = values[0];
	}

}

void HomeThermoConfig::print()
{
	cout << "python_command " << "[" << python_command << "]" << endl;
	cout << "trans_path " << "[" << trans_path << "]" << endl;
	cout << "trans_command " << "[" << trans_command << "]" << endl;
	cout << "trans_pin " << "[" << trans_pin << "]" << endl;
	cout << "app_key " << "[" << app_key << "]" << endl;
	cout << "code_bits " << "[" << code_bits << "]" << endl;
	cout << "trans_repeat " << "[" << trans_repeat << "]" << endl;
	cout << "heating_code_on " << "[" << heating_code_on << "]" << endl;
	cout << "heating_code_off " << "[" << heating_code_off << "]" << endl;
	cout << "default_temp_target " << "[" << default_temp_target << "]" << endl;
	cout << "path_data " << "[" << path_data << "]" << endl;
	cout << "path_log " << "[" << path_log << "]" << endl;
}

ThermoList::ThermoList(const char* path)
: FileParser((path + THERMO_FILE).c_str()), tList()
{

}

void ThermoList::setField(string field, vector<string> values)
{
	ThermoTrans trans;
	trans.stamp = 0;
	trans.temp = 99.9;
	trans.id = stoi(field);
	trans.name = values[0];
	trans.pipe = stoi(values[1]);
	trans.weight = stoi(values[2]);
	tList.push_back(trans);
}


TimeTempList::TimeTempList(const char* path)
: FileParser((path + TT_FILE).c_str()), ttList(), active(NULL)
{
}

void TimeTempList::afterParse()
{
	if(!ttList.empty())
	{
		ttList.sort();
		//active = &ttList.front();
	}
}

bool TimeTempList::empty()
{
	return ttList.empty();
}

void TimeTempList::setField(string field, vector<string> values)
{
	TimeTemp tt(convertToSecs(stoi(values[0]), stoi(values[1]), stoi(values[2])), stoi(field), false);
	ttList.push_back(tt);
}

int TimeTempList::convertToSecs(int hour, int mins, int secs)
{
	return (((hour * 60) + mins) * 60) + secs;
}

TimeTemp* TimeTempList::updateTargetTemp(time_t t)
{
	TimeTemp* tmp;
	bool changed = false;
	tm *local = localtime(&t);
	int inSecs = convertToSecs(local->tm_hour, local->tm_min, local->tm_sec);
	ttList.sort();
	list<TimeTemp>::iterator it;
	tmp = &ttList.front();
	for(it = ttList.begin(); it != ttList.end(); it++)
	{
		if(it->getTime() <= inSecs)
		{
			tmp = &*it;
		}
	}
	if(tmp != active) 
	{
		active = tmp;
		return active;
	}
	return NULL;
}

