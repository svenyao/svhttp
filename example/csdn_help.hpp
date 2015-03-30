// svhttp_basic.cpp : 定义控制台应用程序的入口点。
//
#ifndef _csdn_help_hpp
#define _csdn_help_hpp

#define ENABLE_LIBICONV
#include "svhttp.h"
#include <iostream>
#include <sstream>
#include <vector>

using std::string;
using namespace svhttp;

class resource_info
{
public:
	bool get_comment()
	{
		return _is_comment;
	}
	std::string get_comment_url()
	{
		return _is_comment?_comment_url:"";
	}
	std::string get_res_name()
	{
		return _res_name;
	}
	std::string get_res_url()
	{
		return _res_url;
	}
	std::string get_res_title()
	{
		return _res_title;
	}
	std::string get_res_id()
	{
		return _res_id;
	}
	
	void resource_info_parse(std::string& _res_str)
	{
		int x_pos = 0;
		int d_pos = 0;
		x_pos = _res_str.find("<h3>");
		d_pos = _res_str.find("</a>", x_pos);
		x_pos = _res_str.rfind(">", d_pos);
		
		SVLOGGER_DBG << "------------------------------";
		_res_name = _res_str.substr(x_pos+1, d_pos-x_pos-1);
		//SVLOGGER_INFO << _res_name;

		d_pos = x_pos - 1;
		x_pos = _res_str.rfind("href=\"", d_pos);
		_res_url = _res_str.substr(x_pos+6, d_pos-x_pos-6);
		//SVLOGGER_INFO << _res_url;

		x_pos = _res_str.rfind("/", d_pos);
		_res_id = _res_str.substr(x_pos+1, d_pos-x_pos-1);
		//SVLOGGER_INFO << _res_id;
		
		x_pos = _res_str.find("#comment");
		if (x_pos == -1)
		{
			_is_comment = true;
			//SVLOGGER_INFO << "已评价";
		}
		else
		{
			_is_comment = false;

			d_pos = x_pos+8;
			x_pos = _res_str.rfind("href=\"", d_pos);
			_comment_url = _res_str.substr(x_pos+6, d_pos-x_pos-6);
			//SVLOGGER_INFO << _comment_url;
			//SVLOGGER_INFO << "立即评价，通过可返分";
		}

		x_pos = _res_str.find("title=\"");
		if (x_pos != -1)
		{
			d_pos = _res_str.find("\"", x_pos+7);
			_res_title = _res_str.substr(x_pos+7, d_pos-x_pos-7);
			SVLOGGER_INFO << _res_title;
		}
	}

	bool add_comments(http_client& _client,std::string _str_content, int leve)
	{
		_error_buf = "";
		if (_is_comment)
		{
			_error_buf = "该资源已经评价!";
			SVLOGGER_WARN << _error_buf;
			return false;
		}

		std::ostringstream url;
		url << "http://download.csdn.net/index.php/comment/post_comment?jsonpcallback=jsonp1385304626524&sourceid=" 
			<< _res_id << "&content=" 
			<< "%E9%9D%9E%E5%B8%B8%E6%84%9F%E8%B0%A2%EF%BC%8C%E8%BF%99%E8%B5%84%E6%BA%90%E6%88%91%E6%89%BE%E4%BA%86%E5%A5%BD%E4%B9%85%E4%BA%86%EF%BC%81&rating="
			<< leve << "&t=1385304679900";

		request_header opts;
		std::string str_referer = "Referer: http://download.csdn.net" + _res_url;
		opts.insert( str_referer );
		_client.set_headers(opts);

		if(!_client.open(url.str()))
		{
			_error_buf = _client.get_error_buffer();
			return false;
		}

		std::string ret_content = _client.read_some();

		SVLOGGER_ERR << ret_content;
		_error_buf = ret_content;

		int pos = ret_content.find("\"succ\":1");
		if (pos != -1)
		{
			_is_comment = true;
			return true;
		}
		else
		{
			return false;
		}
	}

	std::string get_error_buf()
	{
		return _error_buf;
	}

private:
	std::string _res_name;
	std::string _res_url;
	std::string _res_title;
	std::string _comment_url;
	bool _is_comment;

	std::string _res_id;

	std::string _error_buf;
};

class csdn_manager
{
public:
	csdn_manager()
	{
		_user_name = "";
		_password = "";
		_page_count = 0;
		_res_info_list.clear();
		_login_state = false;

		//SVLOGGER_INIT_PATH("csdnAssist.log");
		SVLOGGER_DBG << _client.get_version();
	}
	~csdn_manager(){
		_res_info_list.clear();
	};

	bool csdn_login(std::string user_name, std::string password)
	{
		_error_buf = "";
		if (_login_state)
		{
			return true;
		}
		if (!_str_proxy.empty())
		{
			_client.set_proxy(_str_proxy);
		}
		// 开启日志
		//client.set_option(CURLOPT_VERBOSE, 1L);
		_client.set_option(CURLOPT_FOLLOWLOCATION, true);
		_client.set_cookies("cookie.txt");
		
		request_header opts;
		opts.insert(std::string("Referer:https://passport.csdn.net/account/loginbox?callback=logined&hidethird=1&from=http%3a%2f%2fwww.csdn.net%2f"));
		opts.insert(std::string("Accept-Language: zh-cn"));
		opts.insert(std::string("User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET4.0C; .NET4.0E)"));
		opts.insert(std::string("Connection: Keep-Alive"));
		_client.set_headers(opts);
		
		std::ostringstream str_url;
		str_url << "http://passport.csdn.net/ajax/accounthandler.ashx?t=log&u=" << user_name << "&p=" << password << "&remember=0&f=http%3A%2F%2Fwww.csdn.net%2F&rand=0.47590136872096434";
		
		if(!_client.open(str_url.str()))
		{
			_error_buf = _client.get_error_buffer();
			return false;
		}
		
		//_client.read_to_file("login.txt");
		std::string sContent = _client.read_some();
		convert_str("utf-8", "gb2312", sContent);
		//SVLOGGER_INFO << sContent;

		// 登录状态 解析
		int pos = sContent.find("\"status\":true");
		if ( pos != -1){
			int x_pos = sContent.find("data\":");
			int d_pos	  = sContent.rfind("\"}}");
			_login_rcv_buf = sContent.substr(x_pos+6, d_pos - x_pos - 4);
			
			SVLOGGER_INFO << "登陆成功!";
			_login_state = true;

			//登录成功， 解析csdn用户信息
			x_pos = sContent.find("nickName\":\"");
			d_pos = sContent.find("\"", x_pos + 11);
			_nick_name = sContent.substr(x_pos+11, d_pos-x_pos-11);
			
			x_pos = sContent.find("userId\":");
			d_pos = sContent.find(",", x_pos + 8);
			_user_id = sContent.substr(x_pos+8, d_pos-x_pos-8);

			x_pos = sContent.find("email\":\"");
			d_pos = sContent.find("\"", x_pos+8);
			_email_addr = sContent.substr(x_pos+8, d_pos-x_pos-8);
		}else{
			int x_pos = sContent.find("error\":");
			if (x_pos != -1)
			{
				int d_pos = sContent.find("\"", x_pos+8);
				_login_rcv_buf = sContent.substr(x_pos+8, d_pos-x_pos -8);
			}
			else
			{
				_login_rcv_buf = "登录失败，未知错误!";
			}
			_login_state = false;
			SVLOGGER_ERR << _login_rcv_buf;
		}


		return _login_state;
	}

	bool csdn_res_init()
	{
		_error_buf = "";
		get_page_count_net();
		
		return get_res_list_net();
	}

	std::vector<resource_info *> get_res_list()
	{
		return _res_info_list;
	}
	int get_page_count()
	{
		return _page_count;
	}
	std::string get_login_info()
	{
		return (_login_rcv_buf.empty()?_error_buf:_login_rcv_buf);
	}
	
	bool add_comments(std::string _res_id, std::string content = "", int leve = 5)
	{
		bool ret = false;
		std::vector<resource_info *>::iterator iter = _res_info_list.begin();
		for (iter; iter != _res_info_list.end(); ++iter)
		{
			resource_info *_info = *iter;
			if (_info->get_res_id() == _res_id /*&& !_info->get_comment()*/)
			{
				ret = _info->add_comments(_client, content, leve);
				if (ret)
				{
					SVLOGGER_WARN << "评价成功!";
				}
				else
				{
					_error_buf = _info->get_error_buf();
					SVLOGGER_ERR << "评价失败!";
				}
			}
		}

		return ret;
	}


	std::string get_error_buf()
	{
		return _error_buf;
	}

	std::string get_nick_name()
	{
		return _nick_name;
	}

	std::string get_user_id()
	{
		return _user_id;
	}
	std::string get_email_addr()
	{
		return _email_addr;
	}
	std::string get_user_name()
	{
		return _user_name;
	}

	std::string get_csdn_integral()
	{
		return _csdn_integral;
	}

	std::string get_csdn_ranking()
	{
		return _csdn_ranking;
	}

	std::string get_csdn_level()
	{
		return _csdn_level;
	}

	void set_proxy(std::string proxy){
		_str_proxy = proxy;
	}
private:
	/**
	 *	获取页码、 csdn资源信息
	 */
	int get_page_count_net()
	{
		string url = "http://download.csdn.net/my/downloads/1";
		
		if(!_client.open(url))
		{
			_error_buf = _client.get_error_buffer();
			return false;
		}
		string get_str = _client.read_some();
		convert_str("utf-8", "gb2312", get_str);
		string html_str = get_str;

		//debug log..
		//_client.read_to_file("page.txt");
		
		// 页码数
		int x_pos = html_str.find("<div class=\"page_nav\">");
		int d_pos = 0;
		if (x_pos == -1)
		{
			_page_count = 1;
		}
		else
		{
			std::string str_px = "&nbsp; 共";
			x_pos = html_str.find(str_px) + str_px.length();
			if (x_pos == -1)
			{
				_page_count = 1;
			}
			else
			{
				d_pos = html_str.find("页&nbsp;", x_pos);
				std::string strTotal = html_str.substr(x_pos, d_pos - x_pos);
				
				_page_count = atoi(strTotal.c_str());
			}
		}

		// csdn 资源分数信息
		x_pos = html_str.find("<div class=\"points\">");
		if (x_pos != -1)
		{
			d_pos = html_str.find("</div>", x_pos+20);
			std::string res_str = html_str.substr(x_pos+20, d_pos-x_pos-20);
			parse_csdn_res_info(res_str);
		}
		
		// csdn等级
		std::string str_px = "alt=\"等级：";
		x_pos = html_str.find(str_px);
		if (x_pos != -1)
		{
			d_pos = html_str.find("\"", x_pos+str_px.length());
			_csdn_level = html_str.substr(x_pos+str_px.length(), d_pos-x_pos-str_px.length());
			SVLOGGER_INFO << "等级" << _csdn_level;
		}

		return _page_count;
	}

	void parse_csdn_res_info(std::string res_str)
	{
		std::string str_px = "积 分：";
		int x_pos = res_str.find(str_px);
		int d_pos = 0;
		if (x_pos != -1)
		{
			d_pos = res_str.find("</em>", x_pos+str_px.length());
			_csdn_integral = res_str.substr(x_pos+str_px.length(), d_pos-x_pos-str_px.length());
			SVLOGGER_INFO << "积分:" << _csdn_integral;
		}
		

		str_px = "<em>总 排 名</em>：第<span class=\"red\">";
		x_pos = res_str.find(str_px);
		if (x_pos != -1)
		{
			d_pos = res_str.find("</span>", x_pos+str_px.length());
			_csdn_ranking = res_str.substr(x_pos+str_px.length(), d_pos-x_pos-str_px.length());
			SVLOGGER_INFO << "总排行:" << _csdn_ranking;
		}
	}
		
	bool get_page_list_net(int ipage)
	{
		if (ipage > _page_count || ipage < 1)
		{
			_error_buf = "请求页码不正确!";
			SVLOGGER_ERR << _error_buf;
			return false;
		}
		
		std::stringstream _url;
		_url << "http://download.csdn.net/my/downloads/" << ipage;
		if(!_client.open(_url.str()))
		{
			_error_buf = _client.get_error_buffer();
			return false;
		}
		
		std::string page_str = _client.read_some();
		convert_str("utf-8", "gb2312", page_str);
		
		SVLOGGER_WARN << "===================================[" << ipage << "]";
		
		int x_pos = page_str.find("<dl>");
		int d_pos = page_str.find("</dl>", x_pos);
		std::string page_str_xx = page_str.substr(x_pos, d_pos-x_pos+5);
		
		int pre_pos = 0;
		
		while ( (x_pos = page_str_xx.find("<dt>", pre_pos)) != -1 )
		{
			d_pos = page_str_xx.find("</dt>", x_pos);
			pre_pos = d_pos;
			
			//SVLOGGER_INFO << page_str_xx.substr(x_pos, d_pos-x_pos+5);
			
			std::string page_str_xx_sub = page_str_xx.substr(x_pos, d_pos-x_pos+5);
			
			resource_info *_info = new resource_info;
			_info->resource_info_parse(page_str_xx_sub);
			
			_res_info_list.push_back(_info);
		}

		return true;
	}
	
	bool get_res_list_net()
	{
		_res_info_list.clear();
		for (int i = 0; i < _page_count; ++i)
		{
			if (!get_page_list_net( i+1))
			{
				return false;
			}
		}
		return true;
	}
private:
	http_client _client;
	
	std::string _error_buf;

	std::string _user_name;
	std::string _password;

	bool _login_state;
	std::string _login_rcv_buf;

	// 页数统计
	int _page_count;

	std::vector<resource_info *> _res_info_list;

	// 获取的csdn用户信息
	std::string _nick_name;
	std::string _user_id;
	std::string _email_addr;

	std::string _csdn_integral;	// csdn 积分
	std::string _csdn_ranking;	// csdn 排名
	std::string _csdn_level;	// csdn 等级

	std::string _str_proxy;
};


// int main(int argc, char* argv[])
// {
// 	http_client _client;
// 	csdn_manager _manage(_client);
// 	if(!_manage.csdn_login("xxx", "xxx"))
// 	{
// 		getchar();
// 		return 1;
// 	}
// 	_manage.csdn_res_init();
// 
// 	int nCount = _manage.get_page_count();
// 	SVLOGGER_DBG << "总页数[" << nCount << "]";
// 
// 	_manage.add_comments("8009385");
// 
// 	getchar();
// 	return 0;
// }

#endif