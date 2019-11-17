#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

enum ThermoLogLevel
{
	LOG_TMP,
	LOG_STD,
	LOG_DEV
};

class ThermoLog
{
	public:
		ThermoLog(ThermoLogLevel, const char*, int tempInterval=60);
		void log(std::string, ThermoLogLevel level=LOG_STD);
		void temp(float);
		std::ostringstream& log(ThermoLogLevel);
	private:
		ThermoLogLevel level;
		std::ostringstream os;
		std::string logPath;
		int tempInterval;
		time_t lastTmpLog = 0;
		float temperature;

};