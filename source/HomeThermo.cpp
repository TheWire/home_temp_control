#include <HomeThermo.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <sstream>
//#include <cstlib>
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

ThermoLog::ThermoLog(ThermoLogLevel logLevel, const char* path)
: level(logLevel), logPath(path)
{
}



void ThermoLog::log(const char* str, ThermoLogLevel logLevel=LOG_STD)
{
	if(logLevel >= level)
	{
		time_t now = time(NULL);
		tm *local = localtime(&now);
		os << local->tm_year + 1900 << "-" << local->tm_mon + 1 
				<< "-" << local->tm_mday;
		os << "|" << local->tm_hour << ":" << local->tm_min 
				<< ":" << local->tm_sec << " - ";
		os << str << endl;
	}
	ofstream logFile;
	logFile.open(logPath);
	logFile << os.str();
	logFile.close();
	cout << os.str();
}



TimeTemp::TimeTemp(int time, float temp)
: _time(time), _temp(temp)
{
}

int TimeTemp::getTime() const
{
	return _time;
}

float TimeTemp::getTemp() const
{
	return _temp;
}

bool TimeTemp::operator< (const TimeTemp &a) const
{
	if(_time < a.getTime())
	{
		return true;
	}
	else
	{
		return false;
	}
}

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

ThermoList::ThermoList(const char* path)
: FileParser(path), tList()
{

}

void ThermoList::parseValue(ThermoTrans &trans, string value)
{
	int delPos1, delPos2; 
	delPos1 = value.find("|");
	trans.name = value.substr(0, delPos1);
	delPos2 = value.substr(delPos1 + 1).find("|");
	trans.pipe = stoi(value.substr(delPos1 + 1, delPos2));
	trans.weight = stoi(value.substr(delPos2 + 1));

}

void ThermoList::setField(string field, string value)
{
	ThermoTrans trans;
	trans.id = stoi(field);
	parseValue(trans, value);

}


BlynkTransportSocket HomeThermo::_blynkTransport;

HomeThermo::HomeThermo(const char* configPath, ThermoLogLevel level = LOG_STD)
: config(configPath), thermoLst(config.path_thermo.c_str()),
log(level, config.path_log.c_str()), rf24(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, 
BCM2835_SPI_SPEED_8MHZ), rf24network(rf24), app(HomeThermo::_blynkTransport),
lower_bound(0.5), upper_bound(0.5), main_temp(20), heatingOn(false), 
target(20.0), timeLst()
{
	config.parse();
	thermoLst.parse();
	target = config.default_temp_target;
}

 
[[ noreturn ]] void HomeThermo::begin()
{
	log.log("Thermostat starting...", LOG_STD);
	rf24.begin();
	rf24network.begin(BASE_NODE);
	app.begin(config.app_key.c_str());
	main_loop();
}

void HomeThermo::updateApp()
{
	list<ThermoTrans>::iterator it;
	for(it = thermoLst.tList.begin(); it != thermoLst.tList.end(); it++)
	{
		app.virtualWrite(it->pipe, it->temp);
		app.run(); // don't know when this should be called
	}
}

ThermoTrans* HomeThermo::getThermoByID(uint16_t id)
{
	list<ThermoTrans>::iterator it;
	for(it = thermoLst.tList.begin(); it != thermoLst.tList.end(); it++)
	{
		if(it->id == id)
		{
			return &*it;
		}
	}
	return NULL;
}

void HomeThermo::getRF24()
{
	RF24NetworkHeader header;
	float temp;
	rf24network.update();
	while(rf24network.available())
	{
		rf24network.read(header, &temp, sizeof(temp));
		ThermoTrans *thermo = getThermoByID(header.from_node);
		thermo->temp = temp;
	}
}

void HomeThermo::updateMainTemp()
{
	float sum;
	int weight_sum;
	list<ThermoTrans>::iterator it;
	for(it = thermoLst.tList.begin(); it != thermoLst.tList.end(); it++)
	{
		sum += it->temp * (float) it->weight;
		weight_sum += it->weight;
	}
	main_temp = sum / (float) weight_sum;
}

string HomeThermo::transCommand(string code, int bits, int repeat)
{
	ostringstream command;
	command << config.python_command << ' ' << config.trans_path  << config.trans_command;
	command << " -c " << code;
	command << " -b " << bits;
	command << " -r " << repeat;
	cout << command.str() << endl;
	return command.str();
}

void HomeThermo::turnHeatingOn()
{
	system(transCommand(config.heating_code_on, config.code_bits, config.trans_repeat).c_str());
}

void HomeThermo::turnHeatingOff()
{
	system(transCommand(config.heating_code_off, config.code_bits, config.trans_repeat).c_str());
}

void HomeThermo::checkTempOn()
{
	if(main_temp >= (target + upper_bound))
	{
		turnHeatingOff();
	}
}

void HomeThermo::checkTempOff()
{
	if(main_temp <= (target - lower_bound))
	{
		turnHeatingOn();
	}

}

void HomeThermo::checkTemp()
{
	if(heatingOn)
	{
		checkTempOff();
	}
	else
	{
		checkTempOn();
	}
}

int HomeThermo::convertToSecs(int hour, int mins, int secs)
{
	return (((hour * 60) + mins) * 60) + secs;
}

void HomeThermo::updateTargetTemp()
{
	time_t now = time(NULL);
	tm *local = localtime(&now);
	int inSecs = convertToSecs(local->tm_hour, local->tm_min, local->tm_sec);
	timeLst.sort();
	target = timeLst.front().getTemp();
	list<TimeTemp>::iterator it;
	for(it = timeLst.begin(); it != timeLst.end(); it++)
	{
		if(it->getTime() >= inSecs)
		{
			target = it->getTemp();
		}
	}

}

[[ noreturn ]] void HomeThermo::main_loop()
{
	
	while(1)
	{
		if(!timeLst.empty())
		{
			updateTargetTemp();
		}
		getRF24();
		updateMainTemp();
		checkTemp();
		updateApp();
		delay(UPDATE_POLL_TIME);
	}
}