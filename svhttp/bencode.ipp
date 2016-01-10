#ifndef svhttp_bencode_ipp__
#define svhttp_bencode_ipp__

#include "bencode.hpp"

#include <stdlib.h>
#include <cctype>
#include <functional>
#include <fcntl.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace svhttp
{
#ifdef ENABLE_LIBICONV
	/**
	 *  convert(...) string封装
	 *  [in|out]put string : src_str
	 *  s[from|to]: 编码名称
	 */
	int convert_str(const std::string& sfrom, const std::string& sto, std::string& src_str)
	{
		int ilen = src_str.length();
		int ioutlen = ilen * 3;
		char *inbuf = (char*)malloc(ilen+1);
		memset(inbuf, 0, ilen+1);
		strncpy(inbuf, src_str.c_str(), ilen);
		char *outbuf = (char *)malloc(ioutlen);
		memset(outbuf, 0, ioutlen);

		int nret = convert(sfrom.c_str(), sto.c_str(), outbuf, ioutlen, inbuf, ilen);
		if (nret < 0)
		{
			free(inbuf);
			inbuf = nullptr;
			free(outbuf);
			outbuf = nullptr;
			return nret;
		}

		free(inbuf);
		inbuf = nullptr;
		outbuf[nret] = '\0';
		src_str = outbuf;
		free(outbuf);
		outbuf = nullptr;

		return nret;
	}

	/**
	 *	对字符串进行语言编码转换
	 *	param from  原始编码，比如"GB2312",的按照iconv支持的写
	 *	param to    转换的目的编码
	 *	param save  转换后的数据保存到这个指针里，需要在外部分配内存
	 *	param savelen  存储转换后数据的内存大小
	 *	param src      原始需要转换的字符串
	 *	param srclen   原始字符串长度
	 */
	int convert(const char *from, const char *to, char* save, int savelen, char *src, int srclen)
	{
		iconv_t cd;
		char *inbuf = src;
		char *outbuf = save;
		size_t outbufsize = savelen;
		int status = 0;
		size_t  savesize = 0;
		size_t inbufsize = srclen;
		char* inptr = inbuf;
		size_t insize = inbufsize;
		char* outptr = outbuf;
		size_t outsize = outbufsize;

		cd = iconv_open(to, from);
		iconv(cd, NULL, NULL, NULL, NULL);
		if (inbufsize == 0) {
			status = -1;
			goto done;
		}
		while (insize > 0) {
#if defined(_WIN32)
			size_t res = iconv(cd, (const char**)&inptr, &insize, &outptr, &outsize);
#else
			size_t res = iconv(cd, &inptr, &insize, &outptr, &outsize);
#endif
			if (outptr != outbuf) {
				int saved_errno = errno;
				int outsize = outptr - outbuf;
				strncpy(save + savesize, outbuf, outsize);
				errno = saved_errno;
			}
			if (res == (size_t)(-1)) {
				if (errno == EILSEQ) {
					int one = 1;
					iconvctl(cd, ICONV_SET_DISCARD_ILSEQ, &one);
					status = -3;
				}
				else if (errno == EINVAL) {
					if (inbufsize == 0) {
						status = -4;
						goto done;
					}
					else {
						break;
					}
				}
				else if (errno == E2BIG) {
					status = -5;
					goto done;
				}
				else {
					status = -6;
					goto done;
				}
			}
		}
		status = strlen(save);
done:
		iconv_close(cd);
		return status;
	}

#endif

#ifdef _WIN32
	/**	
	 *	ANSI  <-->  UNICODE  <-->  UTF8
	 */
	std::wstring ansi_to_unicode(const std::string& sz_ansi)
	{
		int wcslen = ::MultiByteToWideChar(CP_ACP, NULL, sz_ansi.c_str(), sz_ansi.length(), NULL, 0);
		//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
		wchar_t* sz_unicode = new wchar_t[wcslen + 1];
		//转换
		::MultiByteToWideChar(CP_ACP, NULL, sz_ansi.c_str(), sz_ansi.length(), sz_unicode, wcslen);
		//最后加上 '\0'
		sz_unicode[wcslen] = '\0';
		std::wstring unicode_str = sz_unicode;
		delete[] sz_unicode;
		return unicode_str;
	}
	std::string	unicode_to_ansi(const std::wstring& sz_unicode)
	{
		//预转换，得到所需空间的大小
		int ansilen = ::WideCharToMultiByte(CP_ACP, NULL, sz_unicode.c_str(), sz_unicode.length(), NULL, 0, NULL, NULL);
		//分配空间要给'\0'留个空间
		char* sz_ansi = new char[ansilen + 1];
		//转换
		//unicode版对应的strlen是wcslen
		::WideCharToMultiByte(CP_ACP, NULL, sz_unicode.c_str(), sz_unicode.length(), sz_ansi, ansilen, NULL, NULL);
		//最后加上'\0'
		sz_ansi[ansilen] = '\0';

		std::string ansi_str = sz_ansi;
		delete[] sz_ansi;
		return ansi_str;
	}
	std::wstring utf8_to_unicode(const std::string& sz_utf)
	{
		//预转换，得到所需空间的大小
		int wcslen = ::MultiByteToWideChar(CP_UTF8, NULL, sz_utf.c_str(), sz_utf.length(), NULL, 0);
		//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
		wchar_t* sz_unicode = new wchar_t[wcslen + 1];
		//转换
		::MultiByteToWideChar(CP_UTF8, NULL, sz_utf.c_str(), sz_utf.length(), sz_unicode, wcslen);
		//最后加上'\0'
		sz_unicode[wcslen] = '\0';

		std::wstring unicode_str = sz_unicode;
		delete[] sz_unicode;
		return unicode_str;
	}
	std::string	unicode_to_utf8(const std::wstring& sz_unicode)
	{
		//预转换，得到所需空间的大小，这次用的函数和上面名字相反
		int u8len = ::WideCharToMultiByte(CP_UTF8, NULL, sz_unicode.c_str(), sz_unicode.length(), NULL, 0, NULL, NULL);
		//UTF8虽然是Unicode的压缩形式，但也是多字节字符串，所以可以以char的形式保存
		char* sz_utf8 = new char[u8len + 1];
		//转换
		//unicode版对应的strlen是wcslen
		::WideCharToMultiByte(CP_UTF8, NULL, sz_unicode.c_str(), sz_unicode.length(), sz_utf8, u8len, NULL, NULL);
		//最后加上'\0'
		sz_utf8[u8len] = '\0';

		std::string utf8_str = sz_utf8;
		delete[] sz_utf8;
		return utf8_str;
	}
#endif

#if 0
	/**
	 * Encodiert einen std::string base64
	 */
	std::string base64_encode(const std::string& toEncode) 
	{
		char* buf = 0;

		base64_encode(toEncode.c_str(), toEncode.size(), &buf);
		std::string result(buf);
		free(buf);

		return result;
	}

	/**
	 * Entfernt die base64 Maskierung von einem String
	 */
	std::string base64_decode(const std::string& toDecode) 
	{
		char* buf = (char*)malloc(sizeof(char)*toDecode.size());
		memset(buf, 0, sizeof(char)*toDecode.size());
	
		base64_decode(toDecode.c_str(), buf);
		std::string result(buf);
		free(buf);

		return result;
	}

	static char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	static int pos(char c)
	{
		char *p;
		for (p = base64_chars; *p; p++)
		if (*p == c)
			return p - base64_chars;
		return -1;
	}

	int base64_encode(const void *data, int size, char **str)
	{
		char *s, *p;
		int i;
		int c;
		const unsigned char *q;

		p = s = (char *) malloc(size * 4 / 3 + 4);
		if (p == NULL)
		return -1;
		q = (const unsigned char *) data;
		i = 0;
		for (i = 0; i < size;) {
		c = q[i++];
		c *= 256;
		if (i < size)
			c += q[i];
		i++;
		c *= 256;
		if (i < size)
			c += q[i];
		i++;
		p[0] = base64_chars[(c & 0x00fc0000) >> 18];
		p[1] = base64_chars[(c & 0x0003f000) >> 12];
		p[2] = base64_chars[(c & 0x00000fc0) >> 6];
		p[3] = base64_chars[(c & 0x0000003f) >> 0];
		if (i > size)
			p[3] = '=';
		if (i > size + 1)
			p[2] = '=';
		p += 4;
		}
		*p = 0;
		*str = s;
		return strlen(s);
	}

	#define DECODE_ERROR 0xffffffff

	static unsigned int	token_decode(const char *token)
	{
		int i;
		unsigned int val = 0;
		int marker = 0;
		if (strlen(token) < 4)
		return DECODE_ERROR;
		for (i = 0; i < 4; i++) {
		val *= 64;
		if (token[i] == '=')
			marker++;
		else if (marker > 0)
			return DECODE_ERROR;
		else
			val += pos(token[i]);
		}
		if (marker > 2)
		return DECODE_ERROR;
		return (marker << 24) | val;
	}

	int base64_decode(const char *str, void *data)
	{
		const char *p;
		unsigned char *q;

		q = (unsigned char*)data;
		for (p = str; *p && (*p == '=' || strchr(base64_chars, *p)); p += 4) {
		unsigned int val = token_decode(p);
		unsigned int marker = (val >> 24) & 0xff;
		if (val == DECODE_ERROR)
			return -1;
		*q++ = (val >> 16) & 0xff;
		if (marker < 2)
			*q++ = (val >> 8) & 0xff;
		if (marker < 1)
			*q++ = val & 0xff;
		}
		return q - (unsigned char *) data;
	}
#else

	static const char base64EncodeChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static const int base64DecodeChars[] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
		-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
		-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1 };

	SVHTTP_DECL std::string base64_encode( const std::string& toEncode )
	{
		std::string out;
		unsigned int i, len;
		unsigned int c1, c2, c3;

		len = toEncode.length();
		i = 0;
		out = "";
		while (i < len) {
			c1 = toEncode[i++] & 0xff;
			if (i == len)
			{
				out += base64EncodeChars[(c1 >> 2)];
				out += base64EncodeChars[((c1 & 0x3) << 4)];
				out += "==";
				break;
			}
			c2 = toEncode[(i++)];
			if (i == len)
			{
				out += base64EncodeChars[(c1 >> 2)];
				out += base64EncodeChars[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];
				out += base64EncodeChars[((c2 & 0xF) << 2)];
				out += "=";
				break;
			}
			c3 = toEncode[(i++)];
			out += base64EncodeChars[(c1 >> 2)];
			out += base64EncodeChars[(((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4))];
			out += base64EncodeChars[(((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6))];
			out += base64EncodeChars[(c3 & 0x3F)];
		}
		return out;
	}

	SVHTTP_DECL std::string base64_decode(const std::string& toDecode)
	{
		unsigned int c1, c2, c3, c4;
		unsigned int i, len;
		std::string out;

		len = toDecode.length();
		i = 0;
		out = "";
		while (i < len) {
			/* c1 */
			do {
				c1 = base64DecodeChars[toDecode[(i++)] & 0xff];
			} while (i < len && c1 == -1);
			if (c1 == -1)
				break;

			/* c2 */
			do {
				c2 = base64DecodeChars[toDecode[(i++)] & 0xff];
			} while (i < len && c2 == -1);
			if (c2 == -1)
				break;

			out += (char)((c1 << 2) | ((c2 & 0x30) >> 4));

			/* c3 */
			do {
				c3 = toDecode[(i++)] & 0xff;
				if (c3 == 61)
					return out;
				c3 = base64DecodeChars[c3];
			} while (i < len && c3 == -1);
			if (c3 == -1)
				break;

			out += (char)(((c2 & 0XF) << 4) | ((c3 & 0x3C) >> 2));

			/* c4 */
			do {
				c4 = toDecode[(i++)] & 0xff;
				if (c4 == 61)
					return out;
				c4 = base64DecodeChars[c4];
			} while (i < len && c4 == -1);
			if (c4 == -1)
				break;
			out += (char)(((c3 & 0x03) << 6) | c4);
		}
		return out;
	}
#endif

	// 字符替换
	std::string& replace_all(std::string& str,const std::string& old_value, const std::string& new_value)
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
	// string split
	std::vector<std::string> &split(const std::string &input, const std::string& delim, std::vector<std::string> &elems)
	{
		elems.clear();
		size_t last = 0;
		size_t index = input.find_first_of(delim, last);
		while (index != std::string::npos)
		{
			elems.push_back(input.substr(last, index - last));
			last = index + delim.size();
			index = input.find_first_of(delim, last);
		}
		if (index - last > 0)
		{
			elems.push_back(input.substr(last, index - last));
		}
		return elems;
	}
	std::vector<std::string> split(const std::string& input, const std::string& delim)
	{
		std::vector<std::string> elems;
		split(input, delim, elems);
		return elems;
	}

	// ltrim, rtrim, trim
	std::string &ltrim(std::string &s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}
	std::string &rtrim(std::string &s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}
	std::string &trim(std::string &s)
	{
		return ltrim(rtrim(s));
	}

} //@ end namespace svhttp.



#endif 