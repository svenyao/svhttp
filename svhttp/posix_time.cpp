#include "posix_time.h"

#include <time.h>

#if _WIN32
#include <windows.h>
#endif

/**	Copyright (c) 2015-20XX jianyaoy. all rights reserved.
 *	datetime deal class. 
 *	Author: jianyaoy@gmail.com
 *	$Date:  2015-02-10 23:00:00 +0800 $
 */

namespace svhttp
{
/**
 *	全局函数
 */
// 字符替换
std::string& string_replace(std::string& str,const std::string& old_value, const std::string& new_value)
{
	for(std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length() )
	{
		if( (pos = str.find(old_value,pos)) != std::string::npos )
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return str;
}

/**
 *	date_time 类 实现
 */

date_time::date_time()
{
	struct tm tmp;
	time_t timep;
	time(&timep);
	
#if _MSC_VER >= 1400
	localtime_s(&tmp,&timep);
#else
	tmp = *(localtime(&timep));
#endif

	nyear = tmp.tm_year + 1900;
	nmonth = tmp.tm_mon + 1;
	nday = tmp.tm_mday;
	
	nhour = tmp.tm_hour;
	nminuter = tmp.tm_min;
	nseconds = tmp.tm_sec;

#if _WIN32
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	nmilliseconds = wtm.wMilliseconds;
#else
	nmilliseconds = 0;
#endif

	nisdst = tmp.tm_isdst;
	nweekday = tmp.tm_wday; //get_day_of_week(nyear, nmonth, nday);
	nyearday = tmp.tm_yday;
};

date_time::date_time(std::string date_str)
{
	parse_date_string(date_str, true);
}
date_time::date_time(const char* szdate_str)
{
	parse_date_string(szdate_str, true);
}
date_time::date_time(time_t sec)
{
	set_time(sec);
}

date_time::~date_time()
{
}
// locale
// [yyyy-mm-dd hh:mi:ss.ms] msec: 毫秒级 (true: 返回时间字符精确到毫秒)
std::string date_time::to_string(bool msec /*= false*/)
{
	char time_str[64] = {0};
	if (msec)
	{
#if _MSC_VER >= 1400
		sprintf_s(time_str, "%04d-%02d-%02d %02d:%02d:%02d.%03d", nyear, nmonth, nday, nhour, nminuter, nseconds, nmilliseconds);
#else
		sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d.%03d", nyear, nmonth, nday, nhour, nminuter, nseconds, nmilliseconds);
#endif
	}
	else
	{
#if _MSC_VER >= 1400
		sprintf_s(time_str, "%04d-%02d-%02d %02d:%02d:%02d", nyear, nmonth, nday, nhour, nminuter, nseconds);
#else
		sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d", nyear, nmonth, nday, nhour, nminuter, nseconds);
#endif
	}

	return std::string(time_str);
}
// [yyyy-mm-dd]
std::string date_time::to_date_string()
{
	char date_str[64] = {0};
#if _MSC_VER >= 1400
	sprintf_s(date_str, "%04d-%02d-%02d", nyear, nmonth, nday);
#else
	sprintf(date_str, "%04d-%02d-%02d", nyear, nmonth, nday);
#endif
	
	return std::string(date_str);
}
// [hh:mi:ss.ms] msec: 毫秒级 (true: 返回时间字符精确到毫秒)
std::string date_time::to_time_string(bool msec /*= false*/)
{
	char time_str[64] = {0};
	if (msec)
	{
#if _MSC_VER >= 1400
		sprintf_s(time_str, "%02d:%02d:%02d.%03d", nhour, nminuter, nseconds, nmilliseconds);
#else
		sprintf(time_str, "%02d:%02d:%02d.%03d", nhour, nminuter, nseconds, nmilliseconds);
#endif
	}
	else
	{
#if _MSC_VER >= 1400
		sprintf_s(time_str, "%02d:%02d:%02d", nhour, nminuter, nseconds);
#else
		sprintf(time_str, "%02d:%02d:%02d", nhour, nminuter, nseconds);
#endif
	}
	return std::string(time_str);
}

// utc
// utc时区日期和时间 [yyyy-mm-dd hh:mi:ss.ms] msec: 毫秒级 (true: 返回时间字符精确到毫秒)
std::string date_time::to_utc_string(bool msec /*= false*/)
{
	time_t lsec = get_time();
	date_time _date(lsec);
	_date.set_time(lsec, true);

	return _date.to_string(msec);
}
// utc日期 [yyyy-mm-dd] 获取日期字符串
std::string date_time::to_utc_date_string()
{
	time_t lsec = get_time();
	date_time _date(lsec);
	_date.set_time(lsec, true);
	
	return _date.to_date_string();
}
// utc时间 [hh:mi:ss.ms] msec: 毫秒级 (true: 返回时间字符精确到毫秒)
std::string date_time::to_utc_time_string(bool msec/* = false*/)
{
	time_t lsec = get_time();
	date_time _date(lsec);
	_date.set_time(lsec, true);
	
	return _date.to_time_string(msec);
}

// 要设置的日期和时间与GMT时间1970年1月1日午夜之间的秒数。 // utc： 是否转换为utc时间
void date_time::set_time(time_t sec, bool utc /*= false*/)
{
	struct tm tmp;

	if (utc)
	{
#if _MSC_VER >= 1400
		gmtime_s(&tmp,&sec);
#else
		tmp = *(gmtime(&sec));
#endif
	}
	else
	{
#if _MSC_VER >= 1400
		localtime_s(&tmp,&sec);
#else
		tmp = *(localtime(&sec));
#endif
	}
	
	nyear = tmp.tm_year + 1900;
	nmonth = tmp.tm_mon + 1;
	nday = tmp.tm_mday;
	
	nhour = tmp.tm_hour;
	nminuter = tmp.tm_min;
	nseconds = tmp.tm_sec;
	nmilliseconds = 0;
	
	nisdst = tmp.tm_isdst;
	nweekday = tmp.tm_wday; //get_day_of_week(nyear, nmonth, nday);
	nyearday = tmp.tm_yday;
}

// 返回1970年1月1日至今的秒数。
time_t date_time::get_time()
{
	struct tm tmp;
	tmp.tm_year = nyear - 1900;
	tmp.tm_mon = nmonth - 1;
	tmp.tm_mday = nday;
	tmp.tm_hour = nhour;
	tmp.tm_min = nminuter;
	tmp.tm_sec = nseconds;
	tmp.tm_wday = nweekday;
	tmp.tm_yday = nyearday;
	tmp.tm_isdst = nisdst;
	
	time_t _time;
	_time = mktime(&tmp);
	return _time;
}

// 重置当前datetime中的毫秒
void date_time::reset_milliseconds()
{
#if _WIN32
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	nmilliseconds = wtm.wMilliseconds;
#endif
}

/**
 *	set operator
 */
void date_time::set_year(const unsigned int& val)
{
	nyear = val;
}
void date_time::set_month(const unsigned int& val)
{
	nmonth = val;
}
void date_time::set_day(const unsigned int& val)
{
	nday = val;
}
void date_time::set_hour(const unsigned int& val)
{
	nhour = val;
}
void date_time::set_minuter(const unsigned int& val)
{
	nminuter = val;
}
void date_time::set_seconds(const unsigned int& val)
{
	nseconds = val;
}
void date_time::set_milli_seconds(const unsigned int& val)
{
	nmilliseconds = val;
}

/**
 *	get operator
 */
unsigned int date_time::get_year()
{
	return nyear;
}
unsigned int date_time::get_month()
{
	return nmonth;
}
unsigned int date_time::get_day()
{
	return nday;
}
unsigned int date_time::get_hour()
{
	return nhour;
}
unsigned int date_time::get_minuter()
{
	return nminuter;
}
unsigned int date_time::get_seconds()
{
	return nseconds;
}
unsigned int date_time::get_milli_seconds()
{
	return nmilliseconds;
}

// 获取date_time日期对应的星期x <nweekday: days since Sunday - [0,6] >
std::string date_time::get_week_day_abbr()
{
	const std::string weeks[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	return weeks[nweekday];
}
// 获取date_time日期对应的月份 英文缩写
std::string date_time::get_month_abbr()
{
	const std::string months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	return months[nmonth-1];
}

// days since January 1 - [0,365]
unsigned int date_time::get_year_day()
{
	//return nyearday;
	return get_day_of_year(nyear, nmonth, nday);
}

// 计算出某天 前/后 idays 天的日期
void date_time::add_any_days(int idays)
{
	if (idays == 0)
	{
		return;
	}
	
	if (idays < 0)
	{
		for (int i = 0; i < abs(idays); i++)
		{
			nday--;
			if (nday == 0)
			{
				nmonth--;
				if (nmonth == 0)
				{
					nyear--;
					nmonth = 12;
				}
				nday = get_day_of_month(nyear, nmonth);
			}
		}
	}
	if (idays > 0)
	{
		for (int i = 0; i < idays; i++)
		{
			nday++;
			if (nday > get_day_of_month(nyear,nmonth) )
			{
				nmonth++;
				nday = 1;
				if (nmonth >12)
				{
					nyear++;
					nmonth = 1;
				}
			}
		}
	}
}

// 蔡勒公式: 计算指定日期对应的星期几 since Sunday - [0,6]
unsigned int date_time::get_day_of_week( const unsigned int& iyear, const unsigned int& imonth, const unsigned int& iday )
{
	int iweek = 0;
	unsigned int y = 0, c = 0, m = 0, d = 0;
	
	if ( imonth == 1 || imonth == 2 )
	{
		c = ( iyear - 1 ) / 100;
		y = ( iyear - 1 ) % 100;
		m = imonth + 12;
		d = iday;
	}
	else
	{
		c = iyear / 100;
		y = iyear % 100;
		m = imonth;
		d = iday;
	}
	
	iweek = y + y / 4 + c / 4 - 2 * c + 26 * ( m + 1 ) / 10 + d - 1;    //蔡勒公式
	iweek = iweek >= 0 ? ( iweek % 7 ) : ( iweek % 7 + 7 );    // iweek 为负时取模
	
	return (unsigned)iweek;
}
// 计算某月的天数
unsigned int date_time::get_day_of_month(const unsigned int& iyear, const unsigned int& imonth)
{
	unsigned iday = 0;
	if (imonth == 1 || 
		imonth == 3 || 
		imonth == 5 || 
		imonth == 7 || 
		imonth == 8 || 
		imonth == 10 || 
		imonth == 12 ){
		iday = 31;
	}
	if (imonth == 4 || 
		imonth == 6 || 
		imonth == 9 || 
		imonth == 11 ){
		iday = 30;
	}
	if(imonth == 2){	//二月要分闰年/平年
		iday = (unsigned int)( ((iyear%4 == 0) && (iyear%100 != 0 ||iyear%400 == 0)) ? 29:28 );
	}
	return iday;
}

// days since January 1 - [0,365]
unsigned int date_time::get_day_of_year(const unsigned int& iyear, const unsigned int& imonth, const unsigned int& iday)
{
	unsigned int ndays = 0;
	for (unsigned int idx = 0; idx < imonth-1; ++idx)
	{
		ndays += get_day_of_month(iyear, idx+1);
	}
	ndays += iday-1;
	
	return ndays;
}

int date_time::get_diff_date( date_time date1, date_time date2,const unsigned int& itype /*= 0*/)	
{
	int nret = 0;
	int compare_ret = compare_date(date1, date2);
	if ( compare_ret == 0)
	{
		return 0;
	}
	
	if (itype == 1)
	{
		nret = (date2.nmonth - date1.nmonth) + 12 * (date2.nyear - date1.nyear);
		if (date2.nday >= date1.nday)
		{
			if (get_day_of_month(date1.nyear, date1.nmonth) == date1.nday &&
				get_day_of_month(date2.nyear, date2.nmonth) >= date2.nday )
			{
			}
			else
			{
				nret++;
			}
		}
		return nret;
	}

	//取相差天数
	int ndays = 0;
	if (compare_ret < 0)
	{
		while (! (date1.nyear == date2.nyear && date1.nmonth == date2.nmonth && date1.nday == date2.nday) )
		{
			date1.add_any_days(1);
			ndays++;
		}
	}
	else
	{
		while (! (date1.nyear == date2.nyear && date1.nmonth == date2.nmonth && date1.nday == date2.nday) )
		{
			date1.add_any_days(-1);
			ndays--;
		}
	}

	//设置返回值
	switch(itype)
	{
	case 2:
		nret = ndays/365;
		break;
	case 0:
		nret = ndays;
		break;
	}

	return nret;
}

int date_time::compare_date(const date_time& date1, const date_time& date2 )
{
	int compare_ret = 0;

	if (date1.nyear == date2.nyear && date1.nmonth == date2.nmonth && date1.nday == date2.nday)
	{
		return 0;
	}
	
	if (date1.nyear < date2.nyear)
	{
		compare_ret = -1;
	}
	else if (date1.nyear > date2.nyear)
	{
		compare_ret = 1;
	}
	else if (date1.nyear == date2.nyear)
	{
		if (date1.nmonth < date2.nmonth)
		{
			compare_ret = -1;
		}
		else if (date1.nmonth > date2.nmonth)
		{
			compare_ret = 1;
		}
		else if (date1.nmonth == date2.nmonth)
		{
			if (date1.nday < date2.nday)
			{
				compare_ret = -1;
			}
			else if (date1.nday > date2.nday)
			{
				compare_ret = 1;
			}
			else if (date1.nday == date2.nday)
			{
				compare_ret = 0;
			}
		}
	}

	return compare_ret;
}
// global formatting. 
void date_time::date_global_format()
{
	nyear = 1900;
	nmonth = 1;
	nday = 1;
	
	nhour = 0;
	nminuter = 0;
	nseconds = 0;
	nmilliseconds = 0;
	
	nisdst = 0;
}
// sz_date_time format: [yyyy-mm-dd hh:mi:ss.ms] 
void date_time::parse_date_string(const std::string& sz_date_time, bool reformat/* = false*/)
{
	int x_pos, d_pos;
	std::string sub_str;
	std::string date_str = sz_date_time;

	// 是否重新格初始化
	if (reformat)
	{
		date_global_format();
	}

	// 格式转换，防止日期分隔符传入的是/, 导致无法解析
	string_replace(date_str, "/", "-");
	
	x_pos = date_str.find(" ");	// 查找空格，用于判断传入的参数类型
	
	// 存在空格(传入的是完整日期时间格式字符串)
	if (x_pos != -1)
	{
		int x_splid = x_pos;
		sub_str = date_str.substr(0, x_pos);
		// 日期字符串处理
		d_pos = sub_str.find("-");
		if (d_pos != -1)
		{
			nyear = (unsigned)atoi(sub_str.substr(0, d_pos).c_str());
			x_pos = d_pos+1;
			d_pos = sub_str.find("-", x_pos);
			if (d_pos != -1)	// 含月
			{
				nmonth = (unsigned)atoi(sub_str.substr(x_pos, d_pos-x_pos).c_str());
				nday = (unsigned)atoi(sub_str.substr(d_pos+1, sub_str.length()-d_pos-1).c_str());
			}
			else	// 不含月
			{
				nmonth = (unsigned)atoi(sub_str.substr(x_pos, sub_str.length()-x_pos).c_str());
			}
		}
		
		sub_str = date_str.substr(x_splid+1, date_str.length()-x_splid-1);
		// 时间字符串处理
		d_pos = sub_str.find(":");
		if (d_pos != -1)
		{
			nhour = (unsigned)atoi(sub_str.substr(0, d_pos).c_str());
			x_pos = d_pos+1;
			d_pos = sub_str.find(":", x_pos);
			if (d_pos != -1)	// 含秒
			{
				nminuter = (unsigned)atoi(sub_str.substr(x_pos, d_pos-x_pos).c_str());
				x_pos = d_pos+1;
				d_pos = sub_str.find(".");
				if (d_pos != -1)	// 含有毫秒
				{
					nseconds = (unsigned)atoi(sub_str.substr(x_pos, d_pos-x_pos).c_str());
					nmilliseconds = (unsigned)atoi(sub_str.substr(d_pos+1, sub_str.length()-d_pos-1).c_str());
				}
				else	//不含毫秒
				{
					nseconds = (unsigned)atoi(sub_str.substr(x_pos, sub_str.length()-x_pos).c_str());
				}
			}
			else	// 不含秒
			{
				nminuter = (unsigned)atoi(sub_str.substr(x_pos, sub_str.length()-x_pos).c_str());
			}
		}

	}
	else	// 不存在空格(传入的为日期或时间字符串)
	{
		x_pos = date_str.find("-");	// 查找日期分割符
		if (x_pos != -1)	// 参数为日期格式
		{
			sub_str = date_str;
			// 日期字符串处理
			d_pos = sub_str.find("-");
			if (d_pos != -1)
			{
				nyear = (unsigned)atoi(sub_str.substr(0, d_pos).c_str());
				x_pos = d_pos+1;
				d_pos = sub_str.find("-", x_pos);
				if (d_pos != -1)	// 含月
				{
					nmonth = (unsigned)atoi(sub_str.substr(x_pos, d_pos-x_pos).c_str());
					nday = (unsigned)atoi(sub_str.substr(d_pos+1, sub_str.length()-d_pos-1).c_str());
				}
				else	// 不含月
				{
					nmonth = (unsigned)atoi(sub_str.substr(x_pos, sub_str.length()-x_pos).c_str());
				}
			}
		}
		else // 参数为时间格式
		{
			sub_str = date_str;
			// 时间字符串处理
			d_pos = sub_str.find(":");
			if (d_pos != -1)
			{
				nhour = (unsigned)atoi(sub_str.substr(0, d_pos).c_str());
				x_pos = d_pos+1;
				d_pos = sub_str.find(":", x_pos);
				if (d_pos != -1)	// 含秒
				{
					nminuter = (unsigned)atoi(sub_str.substr(x_pos, d_pos-x_pos).c_str());
					x_pos = d_pos+1;
					d_pos = sub_str.find(".");
					if (d_pos != -1)	// 含有毫秒
					{
						nseconds = (unsigned)atoi(sub_str.substr(x_pos, d_pos-x_pos).c_str());
						nmilliseconds = (unsigned)atoi(sub_str.substr(d_pos+1, sub_str.length()-d_pos-1).c_str());
					}
					else	//不含毫秒
					{
						nseconds = (unsigned)atoi(sub_str.substr(x_pos, sub_str.length()-x_pos).c_str());
					}
				}
				else	// 不含秒
				{
					nminuter = (unsigned)atoi(sub_str.substr(x_pos, sub_str.length()-x_pos).c_str());
				}
			}
		}
	}

	// [日期|时间] 错误数据修正
	if (nmonth > 12 || nmonth == 0)
	{
		nmonth = 1;
	}
	if (nday == 0 || nday > 31)
	{
		nday = 1;
	}
	if (nhour > 24)
	{
		nhour = 0;
	}
	if (nminuter > 60)
	{
		nminuter = 0;
	}
	if (nseconds > 60)
	{
		nseconds = 0;
	}
	if (nmilliseconds > 999)
	{
		nmilliseconds = 0;
	}

	// 使用修正的数据计算星期x和当前天数(年)
	nweekday = get_day_of_week(nyear, nmonth, nday);
	nyearday = get_day_of_year(nyear, nmonth, nday);
}


} // @end namespace svhttp
