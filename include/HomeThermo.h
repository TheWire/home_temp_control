#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <BlynkApiLinux.h>
#include <BlynkSocket.h>
#include <list>
#include <set>
#include <string>
#include <sstream>
#include <log.h>
#include <config.h>


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