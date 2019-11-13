#include <config.h>
#include <algorithm>

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
const string HomeThermoConfig::PATH_THERMO = "path_thermo";
const string HomeThermoConfig::PATH_LOG = "path_log";


FileParser::FileParser(const char* path)
: configPath(path)
{
}

void FileParser::parse()
{
	ifstream configFile;
	configFile.open(configPath);
	string line;
	while(getline(configFile, line))
	{
		line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
		if(line[0] == '#' || line.empty())
		{
			continue;
		}
		int delPos = line.find("=");
		setField(line.substr(0, delPos), line.substr(delPos + 1));

	}
	configFile.close();
}

HomeThermoConfig::HomeThermoConfig(const char* path)
: FileParser(path),
app_key("0"), trans_pin(2), code_bits(24), trans_repeat(3),
heating_code_on("947871"), heating_code_off("947863"),
python_command("python3"), trans_path("trans/"), 
trans_command("XY_send_code.py"), default_temp_target(20)
{
}

void HomeThermoConfig::setField(string field, string value)
{
	if(field.compare(PYTHON_CMD) == 0)
	{
		python_command = value;
	}
	else if(field.compare(TRANS_PATH) == 0)
	{
		trans_path = value;
	}
	else if(field.compare(TRANS_CMD) == 0)
	{
		trans_command = value;
	}
	else if(field.compare(TRANS_PIN) == 0)
	{
		trans_pin = stoi(value);
	}
	else if(field.compare(APP_KEY) == 0)
	{
		app_key = value;
	}
	else if(field.compare(CODE_BITS) == 0)
	{
		code_bits = stoi(value);
	}
	else if(field.compare(TRANS_REPEAT) == 0)
	{
		trans_repeat = stoi(value);
	}
	else if(field.compare(HEATING_CODE_ON) == 0)
	{
		heating_code_on = value;
	}
	else if(field.compare(HEATING_CODE_OFF) == 0)
	{
		heating_code_off = value;
	}
	else if(field.compare(DEFAULT_TEMP) == 0)
	{
		default_temp_target = stof(value);
	}
	else if(field.compare(PATH_THERMO) == 0)
	{
		path_thermo = value;
	}
	else if(field.compare(PATH_LOG) == 0)
	{
		path_log = value;
	}

}