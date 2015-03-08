#include "svlogger.h"
#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

namespace svlogger
{
	file_aux* file_aux::_sv_http = NULL;

	// 将数据str_input写入到文件file_path中,flag: 打开文件的方式，默认以'wb'只写二进制模式,'ab':追加二进制模式...
	bool save_file(const std::string file_path, const std::string& str_input,const std::string flag/* = "wb"*/)
	{
		// Open file to write binary
		bool ret = true;
		FILE* fp;

#if _MSC_VER >= 1400
		fopen_s(&fp,file_path.c_str(),flag.c_str());
#else
		fp = fopen(file_path.c_str(), flag.c_str());
#endif

		if ( ! fp )
		{
			return false;
		}
		// Get document length
		int doc_len = str_input.size();

		if ( doc_len )
		{
			ret = ( fwrite( str_input.c_str(),doc_len, 1, fp ) == 1 );
		}

		fclose(fp);
		return ret;
	}
	// 读取文件file_path的数据保存在str_output中。
	bool read_file(const std::string file_path, std::string& str_output)
	{
		// Open file to read binary
		FILE* fp;

#if _MSC_VER >= 1400
		fopen_s(&fp, file_path.c_str(), "rb");
#else
		fp = fopen(file_path.c_str(), "rb");
#endif

		if ( !fp )
		{
			return false;
		}
		// Get file length
		fseek( fp, 0, SEEK_END );
		int file_len = ftell( fp );
		fseek( fp, 0, SEEK_SET );

		if ( file_len )
		{
			char* szbuffer = new char[file_len+1];
			fread( szbuffer, file_len, 1, fp );
			szbuffer[file_len] = '\0';
			str_output = szbuffer;
			delete [] szbuffer;
		}
		fclose( fp );

		return true;
	}

	void output_console( std::string& level, const std::string& prefix, const std::string& message )
	{
#ifdef WIN32
		HANDLE handle_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(handle_stdout, &csbi);
		if (level == LOGGER_INFO_STR)
			SetConsoleTextAttribute(handle_stdout, FOREGROUND_GREEN);
		else if (level == LOGGER_DEBUG_STR)
			SetConsoleTextAttribute(handle_stdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		else if (level == LOGGER_WARN_STR)
			SetConsoleTextAttribute(handle_stdout, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		else if (level == LOGGER_ERR_STR)
			SetConsoleTextAttribute(handle_stdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << prefix;
		SetConsoleTextAttribute(handle_stdout, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
		std::cout << message;
		SetConsoleTextAttribute(handle_stdout, csbi.wAttributes);
#else
		if (level == LOGGER_INFO_STR)
			std::cout << "\033[32m" << prefix << "\033[0m" << message;
		else if (level == LOGGER_DEBUG_STR)
			std::cout << "\033[1;32m" << prefix << "\033[0m" << message;
		else if (level == LOGGER_WARN_STR)
			std::cout << "\033[1;33m" << prefix << "\033[0m" << message;
		else if (level == LOGGER_ERR_STR)
			std::cout << "\033[1;31m" << prefix << "\033[0m" << message;
#endif
		std::cout.flush();
	}

	void logger_writer( std::string& level, std::string& message, bool disable_cout /*= false*/ )
	{
		std::string prefix = svhttp::date_time().to_string() + std::string("[") + level + std::string("]: ");
		std::string tmp = message + "\n";
		std::string whole = prefix + tmp;
		
		if (!file_aux::get_instance()->get_file_path().empty())
		{
			save_file(file_aux::get_instance()->get_file_path(), whole, "ab");
		}

		// 宏控制日志不打印到控制台，而只写到日志文件中。
#ifndef SVLOGGER_DISABLE_CONSOLE
		if (!disable_cout)
			output_console(level, prefix, tmp);
#endif
	}


}	//namespace svlog
