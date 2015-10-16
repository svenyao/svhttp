/**	Copyright (c) 2015-20XX jianyaoy. all rights reserved.
 *	A simple logging system
 *	Author: jianyaoy@gmail.com
 *	$Date:  2015-02-10 23:00:00 +0800 $
 */
#ifndef svlogger_logger_hpp__
#define svlogger_logger_hpp__

#ifndef SVLOGGER_DISABLE_HEADER_ONLY
#	define SVLOGGER_DECL inline
# else
#	define SVLOGGER_DECL
#endif // defined(SVTIMER_DISABLE_HEADER_ONLY)

#include "posix_time.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// 启用线程安全宏，需要编译器支持c++11标准
#ifdef ENABLE_LOGGER_THREAD_SAFE
#include <mutex>
#endif

#ifdef WIN32
#	include <Windows.h>	 // for win32 Console api.
#endif // WIN32

#define DEBUG	0
#define INFO	1
#define WARNING	2
#define ERROR	3
#define FILE	4

/**
 *	svlogger 使用简介
	//1. 启用线程安全宏，需要编译器支持c++11标准
	#define ENABLE_LOGGER_THREAD_SAFE
	//2. 宏控制日志不打印到控制台，而只写到日志文件中。
	#define SVLOGGER_DISABLE_CONSOLE
	//3. 引入头文件
	#include "svlogger.hpp"

	//4. 设置日志输出文件的目录(路径文件夹必须保证存在)，不设置则只将日志信息打印到控制台
	SVLOGGER_INIT_PATH(".");
	//5. 设置控制台打印输出的最低日志等级，低于level等级的日志将不输出到控制台
	SVLOGGER_PRINT_LEVEL(level);
	
	//6. 日志输出(1). SVLOGGER_XXX 系列.
	SVLOGGER_DBG << "cout run log...0";
	SVLOGGER_INFO << "cout run log...1";
	SVLOGGER_WARN << "cout run log...2";
	SVLOGGER_ERROR << "cout run log...3";
	SVLOGGER_FILE << "此调用只输出到文件";
	SVLOGGER << "default cout debug level...0";
	...
	//6. 日志输出(2). SVLOG(level).level为输出等级
	SVLOG(level) << "cout run log...";
 */
namespace svlogger
{
	//  Private copy constructor and copy assignment ensure classes derived from
	//  class noncopyable cannot be copied.
	namespace noncopyable_  // protection from unintended ADL
	{
		class noncopyable
		{
		protected:
			noncopyable() {}
			~noncopyable() {}
		private:  // emphasize the following members are private
			noncopyable(const noncopyable&);
			const noncopyable& operator=(const noncopyable&);
		};
	}	// namespace noncopyable_
	typedef noncopyable_::noncopyable noncopyable;

	namespace aux
	{
		template<class Lock>
		Lock& lock_single()
		{
			static Lock lock_instance;
			return lock_instance;
		}

		template<class Writer>
		Writer& writer_single()
		{
			static Writer writer_instance;
			return writer_instance;
		}

		class scoped_lock : public noncopyable
		{
		public:
			scoped_lock(){
#ifdef ENABLE_LOGGER_THREAD_SAFE
				std::mutex& mutex_ = aux::lock_single<std::mutex>();
				mutex_.lock();
#endif
			};
			~scoped_lock(){
#ifdef ENABLE_LOGGER_THREAD_SAFE
				std::mutex& mutex_ = aux::lock_single<std::mutex>();
				mutex_.unlock();
#endif
			};
		};

#ifdef ENABLE_LOGGER_THREAD_SAFE
#	define LOGGER_LOCKS_() aux::scoped_lock lock_
#else
#	define LOGGER_LOCKS_() ((void)0)
#endif // DISABLE_LOGGER_THREAD_SAFE

		class logger_print_level
		{
			template<class Lock>
			friend Lock& lock_single();
		public:
			void set_print_level(unsigned int level){ level_ = level; };
			unsigned int get_print_level(){ return level_; };
		protected:
			logger_print_level()
				:level_(0)
			{};
			~logger_print_level(){};
		private:
			unsigned int level_;
		};
	}

	// 定义logger 信息等级(字符串类型)
	static const std::string LOG_LEVEL_STR[] = { "DEBUG", "#INFO", "WARNG", "ERROR","FILE" };

	class auto_logger_file
	{
		template<class Writer>
		friend Writer& aux::writer_single();
	protected:
		auto_logger_file()
		{}
		~auto_logger_file()
		{
			if (file_.is_open())
				file_.close();
		}
	public:
		void open(const std::string& filename, std::ios_base::openmode flag = std::ios::in | std::ios::out | std::ios::app)
		{
			if (filename.find('*') == filename.npos)
			{
				std::string filename_ = filename;

				std::string str = "svlogger." + LOG_LEVEL_STR[aux::lock_single<aux::logger_print_level>().get_print_level()];
				str += ".";
				svtime::date_time date;
				std::string str_t = date.to_string(true);
				date.replace_all(str_t, "-", "");
				date.replace_all(str_t, " ", ".");
				date.replace_all(str_t, ":", "");
				str += str_t + ".log";
				filename_ += "/" + str;

				file_.open(filename_.c_str(), flag);
			}

			std::string start_string = "\n\n********  starting log  ********\n\n\n";
			write(start_string);
		}

		bool is_open() const
		{
			return file_.is_open();
		}

		void write(const std::string& str)
		{
			if (file_.is_open())
				file_.write(str.c_str(), str.size());
			return;
		}

		void flush()
		{
			file_.flush();
		}

	private:
		std::fstream file_;
		std::string log_path_;
	};

	// 控制台日志输出
	SVLOGGER_DECL void output_console(unsigned int level, const std::string& prefix, const std::string& message)
	{
#ifdef WIN32
		HANDLE handle_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(handle_stdout, &csbi);
		if (level == INFO)
			SetConsoleTextAttribute(handle_stdout, FOREGROUND_GREEN);
		else if (level == DEBUG)
			SetConsoleTextAttribute(handle_stdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		else if (level == WARNING)
			SetConsoleTextAttribute(handle_stdout, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		else if (level == ERROR)
			SetConsoleTextAttribute(handle_stdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << prefix;
		SetConsoleTextAttribute(handle_stdout, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
		std::cout << message;
		SetConsoleTextAttribute(handle_stdout, csbi.wAttributes);
#else
		if (level == INFO)
			std::cout << "\033[32m" << prefix << "\033[0m" << message;
		else if (level == DEBUG)
			std::cout << "\033[1;32m" << prefix << "\033[0m" << message;
		else if (level == WARNING)
			std::cout << "\033[1;33m" << prefix << "\033[0m" << message;
		else if (level == ERROR)
			std::cout << "\033[1;31m" << prefix << "\033[0m" << message;
#endif
		std::cout.flush();
	}

	SVLOGGER_DECL void logger_writer(unsigned int level, std::string& message, bool disable_cout /*= false*/)
	{
		LOGGER_LOCKS_();
		std::string level_str = LOG_LEVEL_STR[level];
		std::string prefix = svtime::date_time().to_string(true) + std::string("[") + level_str + std::string("]: ");
		std::string tmp = message + "\n";
		std::string whole = prefix + tmp;

		if (aux::writer_single<auto_logger_file>().is_open())
		{
			aux::writer_single<auto_logger_file>().write(whole);
			aux::writer_single<auto_logger_file>().flush();
		}

		
		// 宏控制日志不打印到控制台，而只写到日志文件中。
#ifndef SVLOGGER_DISABLE_CONSOLE
		if (!disable_cout)
		{
			if (level >= aux::lock_single<aux::logger_print_level>().get_print_level())
			{
				output_console(level, prefix, tmp);
			}
		}
#endif
	}

	class logger :public noncopyable
	{
	public:
		logger(int level)
			: level_(level)
			, m_disable_cout(false)
		{}
		logger(int level, bool disable_cout)
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

	private:
		std::ostringstream oss_;
		int level_;
		bool m_disable_cout;
	};

	// 空白日志类,不打印输出任何信息
	class empty_logger :public noncopyable
	{
	public:
		template <class T>
		empty_logger& operator << (T const& v)
		{
			return *this;
		}
	};
}	//namespace svlogger

// 设置日志最低打印输出等级
#define SVLOGGER_PRINT_LEVEL(level) svlogger::aux::lock_single<svlogger::aux::logger_print_level>().set_print_level(level)

#define SVLOGGER_INIT_PATH(logfile) do \
	{ \
		svlogger::auto_logger_file& file = svlogger::aux::writer_single<svlogger::auto_logger_file>(); \
		std::string filename = logfile; \
		if (!filename.empty()) \
			file.open(filename, std::ios::in | std::ios::out | std::ios::app); \
	} while (0)

#if /*(defined(DEBUG) || defined(_DEBUG) || defined(ENABLE_LOGGER)) && */!defined(DISABLE_LOGGER)

#define SVLOGGER_DBG svlogger::logger(DEBUG)
#define SVLOGGER_INFO svlogger::logger(INFO)
#define SVLOGGER_WARN svlogger::logger(WARNING)
#define SVLOGGER_ERR svlogger::logger(ERROR)
#define SVLOGGER_FILE svlogger::logger(FILE, true)
#define SVLOG(level) svlogger::logger(level)
#define SVLOGGER svlogger::logger(DEBUG)
#else

#define SVLOGGER_DBG svlogger::empty_logger()
#define SVLOGGER_INFO svlogger::empty_logger()
#define SVLOGGER_WARN svlogger::empty_logger()
#define SVLOGGER_ERR svlogger::empty_logger()
#define SVLOGGER_OUT svlogger::empty_logger()
#define SVLOGGER_FILE svlogger::empty_logger()
#define SVLOG(level) svlogger::empty_logger()
#define SVLOGGER svlogger::empty_logger()
#endif

#endif