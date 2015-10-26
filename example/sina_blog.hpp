#ifndef sina_blog_hpp__
#define sina_blog_hpp__

#include "svhttp.h"
#include <regex>
#include <vector>

struct article_info 
{
	std::string href;
	std::string title;
	std::string tm;
	std::string data;
	std::string ctx;
};

class sina_blog
{
public:
	sina_blog() :atc_list_update(false)
	{
		atc_info_list_.clear();
		client_.set_timeout_connect(4000);
		client_.set_timeout_read(8000);
		client_.set_verbose();
		//client_.set_option(CURLOPT_FOLLOWLOCATION, true);
		client_.enable_accept_encoding(true);
	};

	void get_article_list()
	{
		svhttp::request_header opt;
		opt.insert(std::string("User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/34.0.1838.2 Safari/537.36"));
		opt.insert(std::string("Accept-Encoding:gzip,deflate"));
		opt.insert(std::string("Connection:keep-alive"));
		opt.insert(std::string("Cache-Control:max-age=0"));
		client_.set_headers(opt);
		client_.open("http://blog.sina.com.cn/s/articlelist_1894465110_0_1.html");
		//client_.read_to_file("blog.txt");

		std::string reponse_str = client_.read_some();
		svhttp::convert_str("utf-8", "gbk", reponse_str);
		svhttp::replace_all(reponse_str, "\r", "");
		svhttp::replace_all(reponse_str, "\n", "");
		svhttp::replace_all(reponse_str, "\t", "");
		svhttp::replace_all(reponse_str, "&lt;", "<");
		svhttp::replace_all(reponse_str, "&gt;", ">");
		svhttp::replace_all(reponse_str, "&nbsp;", " ");

		std::regex rgx_article_list("<!-- 列表 START -->(.*?)<!-- 列表END -->");
		std::regex rgx_article_cell("<div .*?>(.*?)</div>");
		std::regex rgx_atc_title("<a title=\"\" target=\"_blank\" href=\"(.*?)\">(.*?)</a>");
		std::regex rgx_atc_tm("<span class=\"atc_tm SG_txtc\">(.*?)</span>");
		std::regex rgx_atc_data("<span class=\"atc_data\" id=\"(.*?)\"></span>");

		std::smatch match_article_list;
		if (std::regex_search(reponse_str, match_article_list, rgx_article_list))
		{
			std::string atc_list(match_article_list[1].first, match_article_list[1].second);

			std::smatch match_article_cell;
			std::string::const_iterator startpos = atc_list.begin();
			std::string::const_iterator endpos = atc_list.end();
			int cell_idx = 0;
			while (std::regex_search(startpos, endpos, match_article_cell, rgx_article_cell))
			{
				cell_idx++;
				std::string atc_cell(match_article_cell[1].first, match_article_cell[1].second);
				article_info atc_info;

				std::smatch match_atc;
				//std::smatch match_atc_data;
				if (std::regex_search(atc_cell, match_atc, rgx_atc_data))
				{
					atc_info.data = std::string(match_atc[1].first, match_atc[1].second);
					if (atc_info_list_.size() > 0 && atc_info.data == atc_info_list_[0].data)
					{
						// blog列表无更新，直接返回，不往下处理。
						return;
					}
					else if( /*atc_info_list_.size() > 0 && */cell_idx == 1)
					{
						// blog有更新。清空之前的类别。并向推送更新消息。
						atc_info_list_.clear();
						article_list_update();
					}
				}
				//std::smatch match_atc_title;
				if (std::regex_search(atc_cell, match_atc, rgx_atc_title))
				{
					atc_info.href = std::string(match_atc[1].first, match_atc[1].second);
					atc_info.title = std::string(match_atc[2].first, match_atc[2].second);
				}
				//std::smatch match_atc_tm;
				if (std::regex_search(atc_cell, match_atc, rgx_atc_tm))
				{
					atc_info.tm = std::string(match_atc[1].first, match_atc[1].second);
				}

				atc_info_list_.push_back(atc_info);
				startpos = match_article_cell[1].second;
			}
		}

	}

	void print_atc_list()
	{
		for (auto atc_info : atc_info_list_)
		{
			SVLOGGER_DBG << atc_info.title << "\t" /*<< atc_info.href << "\t"*/ << atc_info.tm ; 
		}
	}

	void article_list_update()
	{
		atc_list_update = true;
	}

	bool read_newest_article(int idx = 0)
	{
		if (!atc_list_update && idx == 0)
		{
			return false;
		}
		atc_list_update = false;
		article_info atc_info = atc_info_list_[idx];

		SVLOGGER_WARN << atc_info.title << " " /*<< atc_info.href << "\t"*/ ;
		SVLOGGER_WARN << atc_info.tm ;  

		svhttp::request_header opt;
		opt.insert(std::string("Accept-Language:zh-CN,zh;q=0.8,en;q=0.6"));
		opt.insert(std::string("Accept-Encoding:gzip,deflate,sdch"));
		opt.insert(std::string("Connection:keep-alive"));
		opt.insert(std::string("Cache-Control:max-age=0"));
		client_.set_headers(opt);
		client_.open(atc_info.href);
		//client_.read_to_file("blog_atc.txt");

		std::string reponse_str = client_.read_some();
		svhttp::convert_str("utf-8", "gbk", reponse_str);
		svhttp::replace_all(reponse_str, "\r", "");
		svhttp::replace_all(reponse_str, "\n", "");
		svhttp::replace_all(reponse_str, "\t", "");

		std::regex rgx_atc_content("<!-- 正文开始 -->(.*?)<!-- 正文结束 -->");
		std::smatch match_atc_content;
		if (std::regex_search(reponse_str, match_atc_content, rgx_atc_content))
		{
			reponse_str = std::string(match_atc_content[1].first, match_atc_content[1].second);
			
			std::regex rgx_atc_ctx_clean(" *<.*?[/]?> *");
			newest_atc_ctx = std::regex_replace(reponse_str, rgx_atc_ctx_clean, "");
			svhttp::replace_all(newest_atc_ctx, "&nbsp;", " ");
			SVLOGGER_ERR << newest_atc_ctx;
			atc_info_list_[idx].ctx = newest_atc_ctx;

			return true;
		}
		return false;
	}

	std::string get_newest_article_ctx()
	{
		return newest_atc_ctx;
	}
	
	article_info get_newest_article(int idx = 0)
	{
		return atc_info_list_[idx];
	}

private:
	svhttp::http_client client_;

	std::vector<article_info> atc_info_list_;
	bool atc_list_update;
	std::string newest_atc_ctx;
};

#endif // sina_blog_hpp__
