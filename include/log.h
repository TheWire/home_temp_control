#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

enum ThermoLogLevel
{
	LOG_TMP,
	LOG_STD
};

class ThermoLog
{
	public:
		ThermoLog(ThermoLogLevel, const char*);
		void log(const char*, ThermoLogLevel);
		std::ostringstream& log(ThermoLogLevel);
	private:
		ThermoLogLevel level;
		std::ostringstream os;
		std::string logPath;

};