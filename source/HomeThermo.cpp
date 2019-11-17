#include <HomeThermo.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
using namespace std;


TimeTemp::TimeTemp(int time, float temp, bool isSingle)
: _time(time), _temp(temp), _single(isSingle)
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


BlynkTransportSocket HomeThermo::_blynkTransport;

HomeThermo::HomeThermo(const char* configPath, ThermoLogLevel level = LOG_STD)
: config(configPath), thermoLst(config.path_thermo.c_str()),
log(level, config.path_log.c_str()), rf24(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, 
BCM2835_SPI_SPEED_8MHZ), rf24network(rf24), app(HomeThermo::_blynkTransport),
lower_bound(0.5), upper_bound(0.5), main_temp(20), heatingOn(false), 
target(20.0), timeLst(), thermoConnected(0), thermoTO(60)
{
	config.print();
	thermoLst.parse();
	target = config.default_temp_target;
}

 
[[ noreturn ]] void HomeThermo::begin()
{
	log.log("Thermostat starting...", LOG_STD);
	bool ret = rf24.begin();
	if(!ret) {
		log.log("error starting rf24", LOG_STD);
		exit(1);
	}
	rf24network.begin(90, BASE_NODE);
	rf24.printDetails();
	log.log("rf24 started", LOG_STD);
	app.begin(config.app_key.c_str());
	log.log("app started", LOG_STD);
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
		if(thermo != NULL) 
		{
			thermo->temp = temp;
			thermo->stamp = time(NULL);
		}
		
	}
}

void HomeThermo::updateMainTemp()
{
	float sum = 0;
	int weight_sum = 0;
	thermoConnected = 0;
	list<ThermoTrans>::iterator it;
	for(it = thermoLst.tList.begin(); it != thermoLst.tList.end(); it++)
	{
		//only use thermometer if temp is valid and it wasn't updated too long ago
		if(it->temp < 99.9 && it->stamp + thermoTO >= time(NULL)) 
		{
			sum += it->temp * (float) it->weight;
			weight_sum += it->weight;
			thermoConnected++;
			cout << "con" << endl;
		}
	}
	if(thermoConnected > 0) 
	{
		main_temp = sum / (float) weight_sum;
		log.temp(main_temp);
	}
}

string HomeThermo::transCommand(string code, int bits, int repeat)
{
	ostringstream command;
	command << config.python_command << ' ' << config.trans_path  << config.trans_command;
	command << " -c " << code;
	command << " -b " << bits;
	command << " -r " << repeat;
	cout << command.str() << endl;
	string cmd = command.str();
	log.log("command issued: " + command.str(), LOG_DEV);
	command.str("");
	return cmd;
}

void HomeThermo::turnHeatingOn()
{
	log.log("heating on", LOG_STD);
	system(transCommand(config.heating_code_on, config.code_bits, config.trans_repeat).c_str());
	heatingOn = true;
}

void HomeThermo::turnHeatingOff()
{
	log.log("heating off", LOG_STD);
	system(transCommand(config.heating_code_off, config.code_bits, config.trans_repeat).c_str());
	heatingOn = false;
}

void HomeThermo::checkTempOn()
{
	if(main_temp >= (target + upper_bound))
	{
		log.log("temperature target reached", LOG_STD);
		log.log("heating off at: " + to_string(main_temp), LOG_DEV);
		turnHeatingOff();
	}
}

void HomeThermo::checkTempOff()
{
	if(main_temp <= (target - lower_bound))
	{
		log.log("temperature bellow threshold", LOG_STD);
		log.log("heating on at: " + to_string(main_temp), LOG_DEV);
		turnHeatingOn();
	}

}

void HomeThermo::checkTemp()
{
	cout << main_temp << "|" << target << endl;
	if(heatingOn)
	{
		checkTempOn();
	}
	else
	{
		checkTempOff();
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
			log.log("time reached, new target temp: " + to_string(target), LOG_STD);
		}
	}

}

[[ noreturn ]] void HomeThermo::main_loop()
{
	bool noTherm = true;
	while(1)
	{
		if(!timeLst.empty())
		{
			updateTargetTemp();
		}
		getRF24();
		updateMainTemp();
		cout << thermoConnected << endl;
		if(thermoConnected > 0)
		{
			checkTemp();
			updateApp();
			noTherm = false;
		}
		else
		{
			if(noTherm)
			{
				log.log("no thermometers connected", LOG_STD);
				main_temp = 99.9;
			}
			noTherm = true;
		}
		
		delay(UPDATE_POLL_TIME);
	}
}