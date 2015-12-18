// svhttp_example.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include <iostream>

#define ENABLE_LIBICONV
#include "ip138.hpp"
#include "joker.hpp"
#include "joker_qiubai.hpp"
#include "sina_blog.hpp"
#include <windows.h>

//#include "svhttp.h"

void data_time_utest();
void joker_utest();
void sina_blog_utest();
void filesystem_utest();

int main(int argc, char** argv)
{
	SVLOGGER_INIT_PATH(".");
//	SVLOGGER_DBG << ip138().get_reponse_str();

	// sina blog test.
	sina_blog_utest();

	// date_time utest.
	//data_time_utest();

	// joker utest.
	//joker_utest();

	// filesystem utest.
	//filesystem_utest();

	getchar();
	return 0;
}


void data_time_utest()
{
	//date_time 日期时间串解析测试
	SVLOGGER_DBG << svtime::date_time("2015-9-29 1:51:50").to_string(true);
	SVLOGGER_DBG << svtime::date_time("2015-2-29 24:44").to_string(true);
	SVLOGGER_DBG << svtime::date_time("2015-2-30").to_string(true);
	SVLOGGER_DBG << svtime::date_time("2015-9-29 1:51:50.300").to_string(true);
	SVLOGGER_DBG << svtime::date_time("2015-9-29 1:51").to_string(true);
	SVLOGGER_DBG << svtime::date_time("2015-9-161:10:50.300").to_string(true);
	SVLOGGER_DBG << svtime::date_time("11:51:50.300").to_string(true);
	SVLOGGER_DBG << svtime::date_time("11:51").to_string(true);

	SVLOGGER_WARN << svtime::date_time().to_string(true);

	svtime::date_time data1("2015-10-17 00:00:00.120");
	svtime::date_time data2("2015-10-17 23:59:50.200");
	svtime::date_time data3(time_t(0));

	data1.add_any_seconds(-10);
	SVLOGGER_WARN << data1.to_string(true);
	data2.add_any_seconds(400000000);
	SVLOGGER_WARN << data2.to_string(true);
	data3.add_any_seconds(10);
	SVLOGGER_WARN << data3.to_string(true);

}

void joker_utest()
{
	joker joker_;
	SVLOGGER_DBG << joker_.get_random_article_content();
	SVLOGGER_WARN << "press enter to continue...";
	while (getchar())
	{
		SVLOGGER_DBG << joker_.get_next_article_content();
		SVLOGGER_WARN << "press enter to continue...";
	}

// 	joker_qiubai qiubai_;
// 	//SVLOGGER_DBG << qiubai_.get_article_content();
// 	qiubai_.get_article();
// 	SVLOGGER_DBG << qiubai_.get_article_content();
// 	SVLOGGER_WARN << "press enter to continue...";
// 	while (getchar())
// 	{
// 		qiubai_.get_article();
// 		SVLOGGER_DBG << qiubai_.get_article_content();
// 		SVLOGGER_WARN << "press enter to continue...";
// 	}
}

void sina_blog_utest()
{
	sina_blog blog;
	while (1)
	{
		SVLOGGER_DBG << "update article list...";
		blog.get_article_list();
		//blog.print_atc_list();
		blog.read_newest_article();
		//Sleep(30000);
		svtime::sleep(30000);
	}
}

void filesystem_utest()
{
	SVLOGGER_INFO << svhttp::filesystem::getcwd_t();
	SVLOGGER_INFO << svhttp::filesystem::get_special_path();

	std::string file_ctn;
	svhttp::filesystem::read_file(svhttp::filesystem::getcwd_t().append("/blog.txt"), file_ctn);
	//SVLOGGER_ERR << file_ctn;
	//svhttp::unicode_to_ansi(svhttp::utf8_to_unicode(file_ctn));
	SVLOGGER_DBG << svhttp::unicode_to_ansi(svhttp::utf8_to_unicode(file_ctn));
// 	if (svhttp::filesystem::access_file(svhttp::filesystem::getcwd_t().append("/blog_bk.txt")))
// 	{
// 		svhttp::filesystem::save_file(svhttp::filesystem::getcwd_t().append("/blog_bk1.txt"),file_ctn);
// 	}
// 	svhttp::filesystem::save_file(svhttp::filesystem::getcwd_t().append("/blog_bk.txt"),file_ctn);

	svhttp::filesystem::create_direct(svhttp::filesystem::getcwd_t().append("/data"));
	svhttp::filesystem::remove_file(svhttp::filesystem::getcwd_t().append("/blog_bk1.txt"));
	svhttp::filesystem::remove_direct(svhttp::filesystem::getcwd_t().append("/data"));
}




