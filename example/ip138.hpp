#ifndef _svhttp_ip138_hpp_
#define _svhttp_ip138_hpp_

#include <string>
#include <sstream>
#include "svhttp.h"

class ip138
{
public:
	ip138()
	{
		svhttp::http_client _client;
		_client.open("http://1111.ip138.com/ic.asp");
		
		reponse_str = _client.read_some();
		
		int x_pos, d_pos;
		x_pos = reponse_str.find("<center>");
		if (x_pos != -1)
		{
			d_pos = reponse_str.find("</center>", x_pos+8);
			
			reponse_str = reponse_str.substr(x_pos+8, d_pos-x_pos-8);
			x_pos = reponse_str.find("[");
			if (x_pos != -1)
			{
				d_pos = reponse_str.find("]", x_pos+1);
				ip_addr = reponse_str.substr(x_pos+1, d_pos-x_pos-1);
			}
		}
	}
	~ip138(){};

	std::string get_reponse_str(){ return reponse_str; }
	std::string get_ip_addr() { return ip_addr; }
	
private:
	std::string reponse_str;
	std::string ip_addr;
};

#endif