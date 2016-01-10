#ifndef svhttp_bencode_hpp__
#define svhttp_bencode_hpp__

#ifndef SVHTTP_DISABLE_HEADER_ONLY
#	define SVHTTP_DECL inline
# else
#	define SVHTTP_DECL
#endif // defined(SVHTTP_DISABLE_HEADER_ONLY)

#ifdef ENABLE_LIBICONV
#include "iconv.h"
#endif

#include <string>
#include <vector>

namespace svhttp
{
#ifdef ENABLE_LIBICONV
	/**
	 *  convert(...) string封装
	 *  [in|out]put string : src_str
	 *  s[from|to]: 编码名称
	 */
	SVHTTP_DECL int convert_str(const std::string& sfrom, const std::string& sto, std::string& src_str);

	SVHTTP_DECL int convert(const char *from, const char *to, char* save, int savelen, char *src, int srclen);
#endif

#ifdef _WIN32
	/**	
	 *	ANSI  <-->  UNICODE  <-->  UTF8
	 */
	std::wstring ansi_to_unicode(const std::string& sz_ansi);		//ansi to unicode
	std::string	unicode_to_ansi(const std::wstring& sz_unicode);	// unicode to ansi
	std::wstring utf8_to_unicode(const std::string& sz_utf);		// utf8 to unicode
	std::string	unicode_to_utf8(const std::wstring& sz_unicode);	//unicode to utf8
#endif
	/**
	 *	base64 encode&decode
	 */
#if 0
	SVHTTP_DECL std::string base64_encode(const std::string& toEncode);
	SVHTTP_DECL std::string base64_decode(const std::string& toDecode);

	SVHTTP_DECL int base64_encode(const void *data, int size, char **str);
	SVHTTP_DECL int base64_decode(const char *str, void *data);
#else
	SVHTTP_DECL std::string base64_encode(const std::string& toEncode);
	SVHTTP_DECL std::string base64_decode(const std::string& toDecode);
#endif

	// 扩展字符串处理方法
	// string replace all 
	SVHTTP_DECL std::string& replace_all(std::string& str,const std::string& old_value, const std::string& new_value);
	// string split.
	SVHTTP_DECL std::vector<std::string> &split(const std::string &s, const std::string& delim, std::vector<std::string> &elems);
	SVHTTP_DECL std::vector<std::string> split(const std::string& input, const std::string& delim);
	
	// ltrim, rtrim, trim
	SVHTTP_DECL std::string &ltrim(std::string &s);
	SVHTTP_DECL std::string &rtrim(std::string &s);
	SVHTTP_DECL std::string &trim(std::string &s);

} //@ end namespace svhttp.

#ifndef SVHTTP_DISABLE_HEADER_ONLY
#include "bencode.ipp"
#endif

#endif