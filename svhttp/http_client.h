#ifndef _svhttp_client_h
#define _svhttp_client_h

#include <string>
#include <vector>

#include "curl/curl.h"
#include "svlogger.h"
#include "settings.hpp"

namespace svhttp
{	
	/**
	 *	http_global define & describe
	 */
	class http_global
	{
	public:
		static http_global* get_instance();
		std::string get_version();

	private:
		http_global(){ global_init(); };
		~http_global(){ global_clean(); };	

		bool global_init();
		void global_clean();

		static http_global *_sv_http;
	};

	/**
	 *	http_client. define
	 */
	class http_client
	{
	public:
		http_client();
		~http_client();

		// http_curl选项添加通用函数
		template <class T>
		bool set_option(CURLoption option, T data)
		{
			return CURLE_OK == curl_easy_setopt(_curl, option, data);
		}
		// 设置请求头
		bool set_headers(request_header& request_opt);
		// 设置cookies文件地址
		void set_cookies(const std::string& cookies_file);
		// 设置https请求时的ssl ca证书文件
		void set_ssl_verification(const std::string& ssl_ca_file);
		// 设置代理服务器
		bool set_proxy(http_proxy &proxy);
		bool set_proxy(std::string &single_proxy);
		// 设置post域数据
		bool set_post_fields(const std::string& post_str);
		// 执行请求url
		bool open(const std::string& url);
		// 读取请求响应数据到字符串
		std::string read_some() 
		{ 
			return std::string(_reponse_stream.begin(), _reponse_stream.end()); 
		};
		// 读取请求响应数据到文件
		void read_to_file(const std::string &file_path)
		{
			svlogger::save_file(file_path, 
				std::string(_reponse_stream.begin(), _reponse_stream.end()));
		}
		// 获得响应头字符串
		std::string read_reponse_header() 
		{ 
			return std::string(_reponse_header_stream.begin(), _reponse_header_stream.end()); 
		};
		// 设置连接超时时间
		void set_timeout_connect(int val)
		{
			_timeout_connect = val;
		};
		int get_timeout_connect()
		{
			return _timeout_connect;
		};
		// 设置读数据超时时间
		void set_timeout_read(int val)
		{
			_timeout_read = val;
		};
		int get_timeout_read()
		{
			return _timeout_read;
		};

		// 获取错误信息
		std::string get_error_buffer()
		{
			return ((!_error_buffer)?"":_error_buffer);
		}
		// 获取当前http_curl 版本信息
		std::string get_version();
		// url字符串编/解码(convert to: %xx)
		std::string str_escape(const std::string& str_in)
		{
			return curl_easy_escape(_curl, str_in.c_str(), str_in.length());
		}
		std::string str_unescape(const std::string& str_in)
		{
			int len;
			char *lp_str_out = curl_easy_unescape(_curl, str_in.c_str(), str_in.length(), &len);
			return std::string(lp_str_out, len);
		}

		// 开启运行日志
		bool set_verbose(bool val = true)
		{ 
			return set_option(CURLOPT_VERBOSE, val?1L:0L); 
		}
		// 是否支持 gzip 数据格式传输
		void enable_accept_encoding(bool val){ _accept_encoding = val; }
	private:
		bool request_init();

		// 读取请求返回数据内容，回调函数
		static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
		// 读取请求返回数据头，回调函数
		static size_t write_header_data(void *ptr, size_t size, size_t nmemb, void *stream);

	private:
		CURL *_curl;
		curl_slist *_headers;
		std::string _cookie_file;
		std::string _ssl_ca_file;

		std::vector<char> _reponse_stream;
		std::vector<char> _reponse_header_stream;
		
		char _error_buffer[CURL_ERROR_SIZE];
		int _timeout_connect;
		int _timeout_read;

		bool _accept_encoding;
	};

	
}	//namespace svhttp


#endif