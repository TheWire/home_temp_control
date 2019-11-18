#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <temp.h>

struct ThermoTrans
{
	uint16_t id;
	float temp;
	std::string name;
	int pipe;
	int weight;
	time_t stamp;
};

class FileParser
{
	public:
		FileParser(const char* path);
		void parse();
		std::vector<std::string> parseValue(std::string value);
	private:
		std::string configPath;

		virtual void setField(std::string, std::vector<std::string>){}
		virtual void afterParse(){}

};

class HomeThermoConfig : public FileParser
{
	public:
		HomeThermoConfig(const char*);
		void print();

		std::string python_command;
		std::string trans_path;
		std::string trans_command;
		std::string path_data;
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
		static const std::string PATH_DATA;
		static const std::string PATH_LOG;

		void setField(std::string, std::vector<std::string>);
};

class ThermoList : public FileParser
{
	public:
		ThermoList(const char*);
		std::list<ThermoTrans>tList;
	private:
		static const std::string THERMO_FILE;
		void setField(std::string, std::vector<std::string>);
};

class TimeTempList : public FileParser
{
	public: 
		TimeTempList(const char*);
		bool empty();
		TimeTemp* updateTargetTemp(time_t);
	private:
		static const std::string TT_FILE;
		std::list<TimeTemp>ttList;
		void afterParse();
		void setField(std::string, std::vector<std::string>);
		int convertToSecs(int, int, int);

		TimeTemp *active;
};