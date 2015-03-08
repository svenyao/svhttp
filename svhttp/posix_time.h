#ifndef _time_posix_hpp_svhttp_
#define _time_posix_hpp_svhttp_

/**	Copyright (c) 2015-20XX jianyaoy. all rights reserved.
 *	datetime deal class. 
 *	Author: jianyaoy@gmail.com
 *	$Date:  2015-02-10 23:00:00 +0800 $
 */

#include <string>

namespace svhttp
{

class date_time
{
public:
	// constructor
	date_time();
	date_time(std::string date_str);
	date_time(const char* szdate_str);
	date_time(time_t sec);	// 以秒设置Date对象 <1970年1月1日至今的秒数>
	// destructor
	~date_time();

	// locale 时间
	// [yyyy-mm-dd hh:mi:ss.ms] msec: 毫秒级 (true: 返回时间字符精确到毫秒)
	std::string to_string(bool msec = false);
	// [yyyy-mm-dd] 获取日期字符串
	std::string to_date_string();
	// [hh:mi:ss.ms] msec: 毫秒级 (true: 返回时间字符精确到毫秒)
	std::string to_time_string(bool msec = false);

	// utc 时间
	// utc时区日期和时间 [yyyy-mm-dd hh:mi:ss.ms] msec: 毫秒级 (true: 返回时间字符精确到毫秒)
	std::string to_utc_string(bool msec = false);
	// utc日期 [yyyy-mm-dd] 获取日期字符串
	std::string to_utc_date_string();
	// utc时间 [hh:mi:ss.ms] msec: 毫秒级 (true: 返回时间字符精确到毫秒)
	std::string to_utc_time_string(bool msec = false);

	// 返回1970年1月1日至今的秒数。
	time_t get_time();
	// 重置当前datetime中的毫秒 <tip: 当前只有在windows有效>
	void reset_milliseconds();

	// set operator
	void set_year(const unsigned int& val);
	void set_month(const unsigned int& val);
	void set_day(const unsigned int& val);
	void set_hour(const unsigned int& val);
	void set_minuter(const unsigned int& val);
	void set_seconds(const unsigned int& val);
	void set_milli_seconds(const unsigned int& val);
	// get operator
	unsigned int get_year();
	unsigned int get_month();
	unsigned int get_day();
	unsigned int get_hour();
	unsigned int get_minuter();
	unsigned int get_seconds();
	unsigned int get_milli_seconds();	

	// 获取date_time日期对应的星期x 英文缩写 <nweekday: days since Sunday - [0,6] >
	std::string get_week_day_abbr();
	// 获取date_time日期对应的月份 英文缩写
	std::string get_month_abbr();
	// days since January 1 - [0,365]
	unsigned int get_year_day();
	// add idays (+/-)
	void add_any_days(int idays);

	/**
	 *	static operation
	 */
	// 计算比较两个日期相差的年/月/日 <type：{0: 日; 1: 月;  2: 年}>
	static int get_diff_date( date_time date1, date_time date2,const unsigned int& itype = 0);
	//  日期比较 (return 1:0:-1)
	static int compare_date(const date_time& date1, const date_time& date2);

	// 使用蔡勒公式: 计算指定日期对应的星期x since Sunday - [0,6]
	static unsigned int get_day_of_week(const unsigned int& iyear, const unsigned int& imonth, const unsigned int& iday );
	// 计算某月的天数
	static unsigned int get_day_of_month(const unsigned int& iyear, const unsigned int& imonth);
	// days since January 1 - [0,365]
	static unsigned int get_day_of_year(const unsigned int& iyear, const unsigned int& imonth, const unsigned int& iday);

	/**
	 *	operator Override
	 */
	friend date_time operator + (date_time& a, const int& v)
	{
		date_time d = a;
		d.add_any_days(v);
		return d;
	}
	friend date_time operator + (const int& v, date_time& a)
	{
		date_time d = a;
		d.add_any_days(v);
		return d;
	}
	friend date_time operator - (date_time& a, const int& v)
	{
		date_time d = a;
		d.add_any_days(-v);
		return d;
	}
	friend date_time operator - (const int& v, date_time& a)
	{
		date_time d = a;
		d.add_any_days(-v);
		return d;
	}
	// 计算两日期之差(返回相差天数)
	int operator - (date_time& b)
	{
		return get_diff_date(b, *this);
	}

	date_time& operator += ( const int& v)
	{
		add_any_days(v);
		return *this;
	}
	date_time& operator -= ( const int& v)
	{
		add_any_days(-v);
		return *this;
	}
	date_time& operator ++ ()
	{
		add_any_days(1);
		return *this;
	}
	date_time& operator ++ (int)
	{
		add_any_days(1);
		return *this;
	}
	date_time& operator -- ()
	{
		add_any_days(-1);
		return *this;
	}
	date_time& operator -- (int)
	{
		add_any_days(-1);
		return *this;
	}

	/**
	 *	日期大小比较操作符
	 */
	bool operator > ( const date_time& b)
	{
		return compare_date(*this, b) > 0;
	}
 	bool operator >=( const date_time& b)
 	{
 		return compare_date(*this, b) >= 0;
 	}
 	bool operator < ( const date_time& b)
 	{
 		return compare_date(*this, b) < 0;
 	}
 	bool operator <=( const date_time& b)
 	{
 		return compare_date(*this, b) <= 0;
 	}
 	bool operator ==( const date_time& b)
 	{
 		return compare_date(*this, b) == 0;
 	}
 	bool operator !=( const date_time& b)
 	{
 		return compare_date(*this, b) != 0;
 	}

private:
	// 要设置的日期和时间与GMT时间1970年1月1日午夜之间的秒数。 // utc： 是否转换为utc时间
	void set_time(time_t sec, bool utc = false);

	// date_time 全局格式化(date_time初始化)
	void date_global_format();
	// reformat: 是否重新做格式化处理
	// date_str format: [yyyy-mm-dd hh:mi:ss.ms] 
	void parse_date_string(const std::string& date_str, bool reformat = false);

	unsigned int nyear;
	unsigned int nmonth;
	unsigned int nday;

	unsigned int nhour;
	unsigned int nminuter;
	unsigned int nseconds;
	unsigned int nmilliseconds;

	unsigned int nisdst;	//夏令时当前是否生效
	unsigned int nweekday;	// day of week.
	unsigned int nyearday;	// day of year.

};

} // @end namespace svhttp


#endif
