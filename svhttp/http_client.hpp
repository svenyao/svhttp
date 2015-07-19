#ifndef svhttp_client_hpp__
#define svhttp_client_hpp__

#ifndef SVHTTP_DISABLE_HEADER_ONLY
#	define SVHTTP_DECL inline
# else
#	define SVHTTP_DECL
#endif // defined(SVHTTP_DISABLE_HEADER_ONLY)

#include "svlogger.hpp"
#include "settings.hpp"
#include "curl/curl.h"
#include <string>
#include <vector>
#include <fstream>

namespace svhttp
{	
	/**
	 *	http_client. define
	 */
	class http_client
	{
	public:
		SVHTTP_DECL http_client();
		SVHTTP_DECL ~http_client();

		// http_curl选项添加通用函数
		template <class T>
		bool set_option(CURLoption option, T data)
		{
			return CURLE_OK == curl_easy_setopt(_curl, option, data);
		}
		// 设置请求头
		SVHTTP_DECL bool set_headers(request_header& request_opt);
		// 设置cookies文件地址
		SVHTTP_DECL void set_cookies(const std::string& cookies_file);
		// 设置https请求时的ssl ca证书文件
		SVHTTP_DECL void set_ssl_verification(const std::string& ssl_ca_file);
		// 设置代理服务器
		SVHTTP_DECL bool set_proxy(http_proxy &proxy);
		SVHTTP_DECL bool set_proxy(std::string &single_proxy);
		// 设置post域数据
		SVHTTP_DECL bool set_post_fields(const std::string& post_str);
		// 执行请求url
		SVHTTP_DECL bool open(const std::string& url);
		// 读取请求响应数据到字符串
		SVHTTP_DECL std::string read_some() 
		{ 
			return std::string(_reponse_stream.begin(), _reponse_stream.end()); 
		};
		// 读取请求响应数据到文件
		SVHTTP_DECL void read_to_file(const std::string &file_path)
		{
			std::fstream file_;
			file_.open(file_path.c_str(), std::ios::binary | std::ios::in | std::ios::trunc);
			std::string content = std::string(_reponse_stream.begin(), _reponse_stream.end());
			file_.write(content.c_str(), content.size());
		}
		// 获得响应头字符串
		SVHTTP_DECL std::string read_reponse_header() 
		{ 
			return std::string(_reponse_header_stream.begin(), _reponse_header_stream.end()); 
		};
		// 设置连接超时时间
		SVHTTP_DECL void set_timeout_connect(int val)
		{
			_timeout_connect = val;
		};
		SVHTTP_DECL int get_timeout_connect()
		{
			return _timeout_connect;
		};
		// 设置读数据超时时间
		SVHTTP_DECL void set_timeout_read(int val)
		{
			_timeout_read = val;
		};
		SVHTTP_DECL int get_timeout_read()
		{
			return _timeout_read;
		};

		// 获取错误信息
		SVHTTP_DECL std::string get_error_buffer()
		{
			return ((!_error_buffer)?"":_error_buffer);
		}
		// 获取当前http_curl 版本信息
		SVHTTP_DECL std::string get_version();
		// url字符串编/解码(convert to: %xx)
		SVHTTP_DECL std::string str_escape(const std::string& str_in)
		{
			return curl_easy_escape(_curl, str_in.c_str(), str_in.length());
		}
		SVHTTP_DECL std::string str_unescape(const std::string& str_in)
		{
			int len;
			char *lp_str_out = curl_easy_unescape(_curl, str_in.c_str(), str_in.length(), &len);
			return std::string(lp_str_out, len);
		}

		// 开启运行日志
		SVHTTP_DECL bool set_verbose(bool val = true)
		{ 
			return set_option(CURLOPT_VERBOSE, val?1L:0L); 
		}
		// 是否支持 gzip 数据格式传输
		SVHTTP_DECL void enable_accept_encoding(bool val){ _accept_encoding = val; }
	private:
		SVHTTP_DECL bool request_init();

		// 读取请求返回数据内容，回调函数
		SVHTTP_DECL static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
		// 读取请求返回数据头，回调函数
		SVHTTP_DECL static size_t write_header_data(void *ptr, size_t size, size_t nmemb, void *stream);

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

#ifndef SVHTTP_DISABLE_HEADER_ONLY
#include "http_client.ipp"
#endif // defined(SVHTTP_DISABLE_HEADER_ONLY)

#endif