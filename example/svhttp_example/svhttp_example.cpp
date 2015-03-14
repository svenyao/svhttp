// svhttp_example.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>

//#define ENABLE_LIBICONV

#include "ip138.hpp"

int _tmain(int argc, _TCHAR* argv[])
{
	ip138 _ip;
	SVLOGGER_ERR << _ip.get_reponse_str();

	return 0;
}

