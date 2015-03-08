#ifndef _svhttp_bencode_hpp_
#define _svhttp_bencode_hpp_

#include <string>
#include "libiconv/include/iconv.h"

namespace svhttp
{
	/**
	 *  convert(...) string封装
	 *  [in|out]put string : src_str
	 *  s[from|to]: 编码名称
	 */
	int convert_str(const std::string& sfrom, const std::string& sto, std::string& src_str);

	int convert(const char *from, const char *to, char* save, int savelen, const char *src, int srclen);

	/**
	 *	base64 encode&decode
	 */
	std::string base64_encode(const std::string& toEncode);
	std::string base64_decode(const std::string& toDecode);

	int base64_encode(const void *data, int size, char **str);
	int base64_decode(const char *str, void *data);
} //@ end namespace svhttp.

#endif