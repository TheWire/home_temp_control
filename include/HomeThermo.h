#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <BlynkApiLinux.h>
#include <BlynkSocket.h>
#include <list>
#include <set>
#include <string>
#include <sstream>
#include <iostream>

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


class TimeTemp
{
	public:
		TimeTemp(int, float);
		int getTime() const;
		float getTemp() const;
		bool operator< (const TimeTemp&) const;
	private:
		int _time;
		float _temp;
};

struct ThermoTrans
{
	uint16_t id;
	float temp;
	std::string name;
	int pipe;
	int weight;
};

class FileParser
{
	public:
		FileParser(const char* path);
		void parse();
	private:
		std::string configPath;

		virtual void setField(std::string, std::string){}

};

class HomeThermoConfig : public FileParser
{
	public:
		HomeThermoConfig(const char*);

		std::string python_command;
		std::string trans_path;
		std::string trans_command;
		std::string path_thermo;
		std::string app_key;
		std::string path_log;
		int trans_pin;
		int code_bits;
		int trans_repeat;
		float default_temp_target;
		std::string heating_code_on;
		std::string heating_code_off;

	private:
	
		static const std::string PYTHON_CMD;
		static const std::string TRANS_PATH;
		static const std::string TRANS_CMD;
		static const std::string APP_KEY;
		static const std::string TRANS_PIN;
		static const std::string CODE_BITS;
		static const std::string TRANS_REPEAT;
		static const std::string HEATING_CODE_ON;
		static const std::string HEATING_CODE_OFF;
		static const std::string DEFAULT_TEMP;
		static const std::string PATH_THERMO;
		static const std::string PATH_LOG;

		void setField(std::string, std::string);
};

class ThermoList : public FileParser
{
	public:
		ThermoList(const char*);
		std::list<ThermoTrans>tList;
	private:
		void setField(std::string, std::string);
		void parseValue(ThermoTrans&, std::string);
};

class HomeThermo
{
	public:
		HomeThermo(const char*, ThermoLogLevel);
		void begin();
	private:
		const uint16_t BASE_NODE = 00;
		const int UPDATE_POLL_TIME = 1000; //ms
		HomeThermoConfig config;
		ThermoList thermoLst;
		ThermoLog log;
		static BlynkTransportSocket _blynkTransport;
		RF24 rf24;
		RF24Network rf24network;
		BlynkSocket app;
		float lower_bound, upper_bound;
		float main_temp;
		int main_temp_pipe;

		bool heatingOn;

		std::list<TimeTemp>timeLst;

		float target;
		void updateApp();
		ThermoTrans* getThermoByID(uint16_t);
		void getRF24();
		void updateMainTemp();
		std::string transCommand(std::string, int, int);
		void turnHeatingOn();
		void turnHeatingOff();
		void checkTempOn();
		void checkTempOff();
		void checkTemp();
		int convertToSecs(int, int, int);
		void updateTargetTemp();
		void main_loop();
};