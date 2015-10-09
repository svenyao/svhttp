#ifndef joker_qiubai_hpp__
#define joker_qiubai_hpp__

#define ENABLE_LIBICONV
#include "svhttp.h"

#include <regex>
#include <stdlib.h>
#include <time.h>

#if (defined (WIN32))
#   define srandom srand
#   define random rand
#endif

//  Provide random number from 0..(num-1)
#define randof(num)  (int) ((float) (num) * random () / (RAND_MAX + 1.0))


class joker_qiubai
{
public:
	joker_qiubai()
	{
		srandom((unsigned)time(nullptr));
		joke_init();
	}
	~joker_qiubai()
	{
		;
	}

	std::string get_article_content()
	{
		return joker_content_;
	}

	void get_article()
	{
		std::ostringstream oss;
		oss << "http://www.qiushibaike.com/8hr/page/" << randof(total_page_num_) << "?s=" << randof(1000000);
		svhttp::request_header opt;
		opt.insert(std::string("Referer: http://www.qiushibaike.com/"));
		opt.insert(std::string("Accept-Encoding: gzip,deflate,sdch"));
		opt.insert(std::string("User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/34.0.1838.2 Safari/537.36"));
		client_.set_headers(opt);
		client_.open(oss.str());
		SVLOGGER_INFO << oss.str();
		std::string reponse_str = client_.read_some();

		//client_.read_to_file("qiubai_html.txt");
		svhttp::convert_str("utf-8", "gbk", reponse_str);
		joker_content_ = get_joke_content(reponse_str);
	}
private:
	bool joke_init()
	{
		std::ostringstream oss;
		oss << "http://www.qiushibaike.com";
		svhttp::request_header opt;
		opt.insert(std::string("Referer: http://www.qiushibaike.com/"));
		opt.insert(std::string("Accept-Encoding: gzip,deflate,sdch"));
		opt.insert(std::string("User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/34.0.1838.2 Safari/537.36"));
		client_.set_headers(opt);
		client_.open(oss.str());
		//SVLOGGER_INFO << oss.str();
		std::string reponse_str = client_.read_some();

		//client_.read_to_file("qiubai_html.txt");
		svhttp::convert_str("utf-8", "gbk", reponse_str);
		joker_content_ = get_joke_content(reponse_str);

		//////////////////////////////////////////////////////////////////////////
		// get page numbeer

		svhttp::replace_all(reponse_str, "\r", "");
		svhttp::replace_all(reponse_str, "\n", "");

		std::regex rgx("<div class=\"pagenumber\">(.*?)</div>");
		std::smatch match_str;
		std::string::const_iterator startpos_r = reponse_str.begin();
		std::string::const_iterator endpos_r = reponse_str.end();
		if (std::regex_search(startpos_r, endpos_r, match_str, rgx))
		{
			std::string page_str(match_str[1].first, match_str[1].second);
			//SVLOGGER_INFO << page_str;
			//startpos_r = match_str[1].second;
			// 
			std::regex page_rgx("<a href=.*?>([0-9]+)</a>");
			std::string page_num_str;
			std::string::const_iterator startpos = page_str.begin();
			std::string::const_iterator endpos = page_str.end();
			while (std::regex_search(startpos, endpos, match_str, page_rgx))
			{
				std::string msg(match_str[1].first, match_str[1].second);
				startpos = match_str[1].second;
				page_num_str = msg;
			}

			total_page_num_ = atoi(page_num_str.c_str());
			//SVLOGGER_ERR << page_num_str;
		}

		return 0;
	}

	std::string get_joke_content(std::string& reponse_str)
	{
		svhttp::replace_all(reponse_str, "\r", "");
		svhttp::replace_all(reponse_str, "\n", "");
		std::smatch match_str;
		std::regex regx("<div class=\"content\">(.*?)(<!--.*?>)</div><div class=\"(.*?)\"");
		
		std::vector<std::string> v;

		std::string::const_iterator startpos = reponse_str.begin();
		std::string::const_iterator endpos = reponse_str.end();
		while (std::regex_search (startpos, endpos, match_str,regx)) 
		{
			std::string msg(match_str[1].first, match_str[1].second);
			startpos = match_str[1].second;
			svhttp::replace_all(msg, "<br/>", "");
			v.push_back(msg);
		}

		int seed = randof(1000)%(v.size()==0?1:v.size());
		//SVLOGGER_ERR << "vsize = " << v.size() << " seed = " << seed << " rand=" << rand();
		
		if (v.empty())
		{
			return "get joker error£¬ v.size() == 0 ";
		}
		return v.at(seed);
	}

private:
	svhttp::http_client client_;
	std::string joker_content_;

	int total_page_num_;
};

#endif // JOKER_QIUBAI_H__
