#include <log.h>
using namespace std;

ThermoLog::ThermoLog(ThermoLogLevel logLevel, const char* path, int tempInterval)
: level(logLevel), logPath(path)
{
	this->tempInterval = tempInterval;
}



void ThermoLog::log(string str, ThermoLogLevel logLevel)
{
	if(logLevel <= level)
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
	logFile.open(logPath, std::ios_base::app);
	logFile << os.str();
	logFile.close();
	cout << os.str();
	os.str("");

}

void ThermoLog::temp(float temp)
{
	if(lastTmpLog + tempInterval < time(NULL))
	{
		lastTmpLog = time(NULL);
		log("temp: " + to_string(temp), LOG_TMP);
	}
}