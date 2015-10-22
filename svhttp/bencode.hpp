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
	/**
	 *	base64 encode&decode
	 */
	SVHTTP_DECL std::string base64_encode(const std::string& toEncode);
	SVHTTP_DECL std::string base64_decode(const std::string& toDecode);

	SVHTTP_DECL int base64_encode(const void *data, int size, char **str);
	SVHTTP_DECL int base64_decode(const char *str, void *data);

	// string replace all 
	SVHTTP_DECL std::string& replace_all(std::string& str,const std::string& old_value, const std::string& new_value);
} //@ end namespace svhttp.

#ifndef SVHTTP_DISABLE_HEADER_ONLY
#include "bencode.ipp"
#endif

#endif