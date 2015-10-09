// svhttp_example.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include <iostream>

#define ENABLE_LIBICONV
#include "ip138.hpp"
#include "joker.hpp"
#include "joker_qiubai.hpp"

int main(int argc, char** argv)
{
	SVLOGGER_INIT_PATH(".");
	SVLOGGER_DBG << ip138().get_reponse_str();

// 	joker joker_;
// 	SVLOGGER_DBG << joker_.get_random_article_content();
// 	SVLOGGER_WARN << "press enter to continue...";
// 	while (getchar())
// 	{
// 		SVLOGGER_DBG << joker_.get_next_article_content();
// 		SVLOGGER_WARN << "press enter to continue...";
// 	}

	joker_qiubai qiubai_;
	//SVLOGGER_DBG << qiubai_.get_article_content();
	qiubai_.get_article();
	SVLOGGER_DBG << qiubai_.get_article_content();
	SVLOGGER_WARN << "press enter to continue...";
	while (getchar())
	{
		qiubai_.get_article();
		SVLOGGER_DBG << qiubai_.get_article_content();
		SVLOGGER_WARN << "press enter to continue...";
	}
	getchar();
	return 0;
}

