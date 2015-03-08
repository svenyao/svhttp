#ifndef _svlogger_log_h
#define _svlogger_log_h

#include <string>
#include <sstream>
#include "posix_time.h"

namespace svlogger
{
	// 将数据str_input写入到文件file_path中,flag: 打开文件的方式，默认以'wb'只写二进制模式,'ab':追加二进制模式...
	bool save_file(const std::string file_path, const std::string& str_input,const std::string flag = "wb");

	// 读取文件file_path的数据保存在str_output中。
	bool read_file(const std::string file_path, std::string& str_output);

	// 定义logger 信息等级
	static std::string LOGGER_DEBUG_STR = "DEBUG";
	static std::string LOGGER_INFO_STR = "INFO";
	static std::string LOGGER_WARN_STR = "WARNING";
	static std::string LOGGER_ERR_STR = "ERROR";
	static std::string LOGGER_FILE_STR = "FILE";
	// 控制台日志输出
	void output_console(std::string& level, const std::string& prefix, const std::string& message);
	
	void logger_writer(std::string& level, std::string& message, bool disable_cout = false);

	class logger
	{
	public:
		logger(std::string& level)
			: level_(level)
			, m_disable_cout(false)
		{}
		logger(std::string& level, bool disable_cout)
			: level_(level)
			, m_disable_cout(disable_cout)
		{}
		~logger()
		{
			std::string message = oss_.str();
			logger_writer(level_, message, m_disable_cout);
		}

		template <class T>
		logger& operator << (T const& v)
		{
			oss_ << v;
			return *this;
		}

		std::ostringstream oss_;
		std::string& level_;
		bool m_disable_cout;
	};

	// 空白日志类,不打印输出任何信息
	class empty_logger
	{
	public:
		template <class T>
		empty_logger& operator << (T const& v)
		{
			return *this;
		}
	};

	// 单例文件类，用来保存日志文件的路径
	class file_aux
	{
	public:
		static file_aux* get_instance()
		{
			if(!_sv_http)
			{
				_sv_http = new file_aux;
			}
			return _sv_http;
		}

		std::string get_file_path()
		{
			return _file_path;
		}

		void set_file_path(std::string file_path)
		{
			_file_path = file_path;

			std::string start_string = "\n\n\n*** starting log ***\n\n\n";
			save_file(_file_path, start_string);
		}
	private:
		file_aux(){};
		~file_aux(){};	
		
		static file_aux *_sv_http;
		std::string _file_path;
	};
}	//namespace svlog

#define SVLOGGER_INIT_PATH(logfile) do \
	{ \
	std::string filename = logfile; \
	if (!filename.empty()) \
		svlogger::file_aux::get_instance()->set_file_path(filename); \
	} while (0)

#if (defined(DEBUG) || defined(_DEBUG) || defined(ENABLE_LOGGER)) && !defined(DISABLE_LOGGER)

#define SVLOGGER_DBG svlogger::logger(svlogger::LOGGER_DEBUG_STR)
#define SVLOGGER_INFO svlogger::logger(svlogger::LOGGER_INFO_STR)
#define SVLOGGER_WARN svlogger::logger(svlogger::LOGGER_WARN_STR)
#define SVLOGGER_ERR svlogger::logger(svlogger::LOGGER_ERR_STR)
#define SVLOGGER_FILE svlogger::logger(svlogger::LOGGER_FILE_STR, true)

#else

#define SVLOGGER_DBG svlogger::empty_logger()
#define SVLOGGER_INFO svlogger::empty_logger()
#define SVLOGGER_WARN svlogger::empty_logger()
#define SVLOGGER_ERR svlogger::empty_logger()
#define SVLOGGER_OUT svlogger::empty_logger()
#define SVLOGGER_FILE svlogger::empty_logger()

#endif

#endif