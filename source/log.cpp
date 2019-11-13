#include <log.h>
using namespace std;

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