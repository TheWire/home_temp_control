#include <string>
#include <iostream>
#include <fstream>
#include <list>

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
	private:
		std::string configPath;

		virtual void setField(std::string, std::string){}

};

class HomeThermoConfig : public FileParser
{
	public:
		HomeThermoConfig(const char*);
		void print();

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