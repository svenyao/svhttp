#ifndef _http_options_hpp_svhttp_
#define _http_options_hpp_svhttp_

#include <string>
#include <vector>


namespace svhttp
{
	/**
	 *	http header options.
	 */ 
	class http_options
	{
	public:
		http_options()
		{ 
			options.clear(); 
		}
		~http_options()
		{ 
			options.clear(); 
		}

		void insert(std::string &str_option)
		{
			options.push_back(str_option);
		}
		//bool insert(std::vector<std::string>& list_option);

		std::vector<std::string> option_all() { return options; };

		// 得到Header字符串.
		std::string header_string() const
		{
			std::string str;
			for (std::vector<std::string>::const_iterator f = options.begin(); f != options.end(); f++)
			{
				str += (*f + "\r\n");
			}
			return str;
		}

	private:
		std::vector<std::string> options;
	};

	typedef http_options request_header;
	typedef http_options reponse_header;


	class http_proxy
	{
	public:
		void set_proxy_type(curl_proxytype &type){ proxy_type = type; };
		curl_proxytype get_proxy_type(){ return proxy_type; }

		void set_hostname(const std::string& shostname){ hostname = shostname; }
		void set_port(const std::string& sport) { port = sport; }
		void set_username(const std::string &susername) { username = susername; }
		void set_password(const std::string &spassword) { password = spassword; }

		std::string get_hostname() { return hostname; }
		std::string get_port() { return port; }
		std::string get_username() { return username; }
		std::string get_password() { return password; }

	private:
		curl_proxytype proxy_type;

		std::string hostname;
		std::string port;

		std::string username;
		std::string password;
	};


}	//namespace svhttp


#endif