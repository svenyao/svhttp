#ifndef _svhttp_joker_hpp_
#define _svhttp_joker_hpp_

#define ENABLE_LIBICONV
#include "svhttp.h"
#include "json_helper.hpp"

#include <stdlib.h>
#include <time.h>

#if (defined (WIN32))
#   define srandom srand
#   define random rand
#endif

//  Provide random number from 0..(num-1)
#define randof(num)  (int) ((float) (num) * random () / (RAND_MAX + 1.0))

class joker
{
public:
	joker()
	{
		srandom((unsigned)time(nullptr));
		if (joker_init())
		{
			get_article(article_id_);
		}
	}
	~joker(){};

	bool get_random_article()
	{
		while (!get_article(get_random_article_id()))
		{
			;
		}
		return true;
	}
	bool get_pre_article()
	{
		if (!article_id_pre_.empty())
		{
			return get_article(article_id_pre_);
		}
		return false;
	}
	bool get_next_article()
	{
		if (!article_id_next_.empty())
		{
			return get_article(article_id_next_);
		}
		return false;
	}

	std::string get_article_title()
	{
		return title_;
	}
	std::string get_article_desc()
	{
		return desc_;
	}
	std::string get_article_url()
	{
		return url_;
	}
	std::string get_article_content()
	{
		return article_content_;
	}

	// 
	std::string get_next_article_content()
	{
		if (!get_next_article())
		{
			get_random_article();
		}
		return article_content_;
	}
	std::string get_pre_article_content()
	{
		if (!get_pre_article())
		{
			get_random_article();
		}
		return article_content_;
	}
	std::string get_random_article_content()
	{
		get_random_article();
		return article_content_;
	}
private:
	bool joker_init()
	{
		client_.open("http://www.laifudao.com/wangwen/");

		std::string reponse_str = client_.read_some();
		std::string temp_str;
		int x_pos, d_pos;
		x_pos = reponse_str.find("articleList");
		if (x_pos != -1)
		{
			temp_str = "href=\"/wangwen/";
			d_pos = reponse_str.find(temp_str, x_pos);
			if (d_pos != -1)
			{
				x_pos = d_pos + temp_str.size();
				temp_str = ".htm";
				d_pos = reponse_str.find(temp_str, x_pos);
				if (d_pos != -1)
				{
					temp_str = reponse_str.substr(x_pos, d_pos-x_pos);
					//SVLOGGER_DBG << temp_str;

					article_id_ = temp_str;
					return true;
				}
			}
		}

		return false;
	}

	bool get_article(std::string article_id)
	{
		joker_member_init();
		std::ostringstream oss;
		oss << "http://www.laifudao.com/wangwen/" << article_id << ".html";
		if (!client_.open(oss.str()))
		{
			return false;
		}

		std::string reponse_str = client_.read_some();
		std::string temp_str;
		int x_pos, d_pos;
		x_pos = reponse_str.find("get-codes-bdshare");
		if (x_pos != -1)
		{
			temp_str = "data=\"";
			d_pos = reponse_str.find(temp_str, x_pos);
			x_pos = d_pos + temp_str.size();
			d_pos = reponse_str.find("\">", x_pos);

			temp_str = reponse_str.substr(x_pos, d_pos-x_pos);
			svhttp::convert_str("utf-8", "gbk", temp_str);
			//SVLOGGER_DBG << temp_str;

			// ' -> "
			svtime::date_time::string_replace(temp_str, "'", "\"");
			//SVLOGGER_DBG << temp_str;
			rapidjson::Document doc;
			if (json::ParseFromString(doc, temp_str))
			{
				title_ = json::GetMemberValueString(doc, "title");
				desc_ = json::GetMemberValueString(doc, "desc");
				url_ = json::GetMemberValueString(doc, "url");
				//SVLOGGER_DBG << "title: " << title_;
				//SVLOGGER_DBG << "desc: " << desc_;
				//SVLOGGER_DBG << "url: " << url_;
			}
			else
			{
				return false;
			}

			// get pre article id
			temp_str = "tooltip-left";
			x_pos = reponse_str.find(temp_str);
			temp_str = "href=\"/wangwen/";
			d_pos = reponse_str.find(temp_str, x_pos);
			if (d_pos != -1)
			{
				x_pos = d_pos + temp_str.size();
				temp_str = ".htm";
				d_pos = reponse_str.find(temp_str, x_pos);
				if (d_pos != -1 && d_pos-x_pos < 20)
				{
					temp_str = reponse_str.substr(x_pos, d_pos-x_pos);
					article_id_pre_ = temp_str;
					//SVLOGGER_DBG << article_id_pre_;
				}
				else
				{
					article_id_pre_ = "";
					//SVLOGGER_DBG << article_id_pre_;
				}
			}

			// get next article id
			temp_str = "tooltip-right";
			x_pos = reponse_str.find(temp_str);
			temp_str = "href=\"/wangwen/";
			d_pos = reponse_str.find(temp_str, x_pos);
			if (d_pos != -1)
			{
				x_pos = d_pos + temp_str.size();
				temp_str = ".htm";
				d_pos = reponse_str.find(temp_str, x_pos);
				if (d_pos != -1 && d_pos-x_pos < 20)
				{
					temp_str = reponse_str.substr(x_pos, d_pos-x_pos);
					article_id_next_ = temp_str;
					//SVLOGGER_DBG << article_id_next_;
				}
				else
				{
					article_id_next_ = "";
					//SVLOGGER_DBG << article_id_next_;
				}
			}

			// article-content
			x_pos = reponse_str.find("article-content");
			if (x_pos != -1)
			{
				d_pos = reponse_str.find("<p>", x_pos);
				x_pos = d_pos + 3;
				d_pos = reponse_str.find("</p>", x_pos);
				temp_str = reponse_str.substr(x_pos, d_pos-x_pos);
				svhttp::convert_str("utf-8", "gbk", temp_str);
				//SVLOGGER_ERR << temp_str;

				// replace <>
				article_content_ = temp_str;
				x_pos = article_content_.find("<a");
				while (x_pos != -1)
				{
					d_pos = article_content_.find(">", x_pos);
					temp_str = article_content_.substr(x_pos, d_pos-x_pos+1);
					//SVLOGGER_WARN << temp_str;

					svtime::date_time::string_replace(article_content_, temp_str, "");
					svtime::date_time::string_replace(article_content_, "</a>", "");
					x_pos = article_content_.find("<a");
				}
				svtime::date_time::string_replace(article_content_, "<br/>", "");
				//SVLOGGER_DBG << article_content_;
			}
		}
		else
		{
			return false;
		}

		return true;
	}

	std::string get_random_article_id()
	{
		std::ostringstream oss;
		int article_id = atoi(article_id_.c_str());
		oss << (int)randof(article_id == 0?100000:article_id);
		return oss.str();
	}

	void joker_member_init()
	{
		article_id_pre_ = "";
		article_id_next_ = "";

		title_ = "";
		desc_ = "";
		url_ = "";
		article_content_ = "";
	}
private:
	svhttp::http_client client_;
	std::string article_id_;
	std::string article_id_next_;
	std::string article_id_pre_;

	std::string title_;
	std::string desc_;
	std::string url_;
	
	std::string article_content_;
};

#endif